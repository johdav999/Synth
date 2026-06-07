#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace
{
constexpr auto treeId = "SynthParameters";

std::atomic<float>* raw(juce::AudioProcessorValueTreeState& state, const juce::String& id)
{
    return state.getRawParameterValue(id);
}

juce::ADSR::Parameters envelopeFrom(juce::AudioProcessorValueTreeState& state, const char* prefix)
{
    return {
        raw(state, juce::String(prefix) + "Attack")->load(),
        raw(state, juce::String(prefix) + "Decay")->load(),
        raw(state, juce::String(prefix) + "Sustain")->load(),
        raw(state, juce::String(prefix) + "Release")->load()
    };
}
}

SynthAudioProcessor::AnalogVoice::AnalogVoice(juce::AudioProcessorValueTreeState& state)
    : parameters(state)
{
}

bool SynthAudioProcessor::AnalogVoice::canPlaySound(juce::SynthesiserSound* sound)
{
    return dynamic_cast<AnalogSound*>(sound) != nullptr;
}

void SynthAudioProcessor::AnalogVoice::prepare(double sampleRate, int samplesPerBlock, int outputChannels)
{
    currentSampleRate = sampleRate;
    voiceBuffer.setSize(outputChannels, samplesPerBlock, false, false, true);

    juce::dsp::ProcessSpec spec { sampleRate, static_cast<juce::uint32>(samplesPerBlock), static_cast<juce::uint32>(outputChannels) };
    filter.prepare(spec);
    filter.setMode(juce::dsp::LadderFilterMode::LPF24);
}

void SynthAudioProcessor::AnalogVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound*, int)
{
    baseFrequency = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
    noteVelocity = velocity;

    const auto drift = raw(parameters, "AnalogDrift")->load();
    for (auto i = 0; i < 3; ++i)
    {
        phases[static_cast<size_t>(i)] = random.nextDouble();
        driftOffsets[static_cast<size_t>(i)] = random.nextFloat() * drift * 2.0f - drift;
    }

    ampEnvelope.setParameters(envelopeFrom(parameters, "Amp"));
    filterEnvelope.setParameters(envelopeFrom(parameters, "Filter"));
    ampEnvelope.noteOn();
    filterEnvelope.noteOn();
}

void SynthAudioProcessor::AnalogVoice::stopNote(float, bool allowTailOff)
{
    ampEnvelope.noteOff();
    filterEnvelope.noteOff();

    if (! allowTailOff || ! ampEnvelope.isActive())
        clearCurrentNote();
}

void SynthAudioProcessor::AnalogVoice::pitchWheelMoved(int newPitchWheelValue)
{
    pitchWheelValue = newPitchWheelValue;
}

void SynthAudioProcessor::AnalogVoice::controllerMoved(int, int)
{
}

float SynthAudioProcessor::AnalogVoice::oscillatorSample(int waveform, double phase, double pulseWidth)
{
    switch (waveform)
    {
        case 0: return static_cast<float>(std::sin(juce::MathConstants<double>::twoPi * phase));
        case 1: return static_cast<float>(2.0 * phase - 1.0);
        case 2: return phase < pulseWidth ? 1.0f : -1.0f;
        default: return phase < 0.5 ? static_cast<float>(phase * 4.0 - 1.0) : static_cast<float>(3.0 - phase * 4.0);
    }
}

void SynthAudioProcessor::AnalogVoice::updateParameters()
{
    const auto bendSemitones = (static_cast<float>(pitchWheelValue) - 8192.0f) / 8192.0f * 2.0f;
    const std::array<float, 3> semitoneOffsets {
        raw(parameters, "Osc1Tune")->load(),
        raw(parameters, "Osc2Tune")->load(),
        raw(parameters, "Osc3Tune")->load()
    };

    for (auto i = 0; i < 3; ++i)
    {
        const auto semitones = semitoneOffsets[static_cast<size_t>(i)] + bendSemitones + driftOffsets[static_cast<size_t>(i)];
        const auto frequency = baseFrequency * std::pow(2.0, static_cast<double>(semitones) / 12.0);
        increments[static_cast<size_t>(i)] = frequency / currentSampleRate;
    }

    ampEnvelope.setParameters(envelopeFrom(parameters, "Amp"));
    filterEnvelope.setParameters(envelopeFrom(parameters, "Filter"));
}

void SynthAudioProcessor::AnalogVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
    if (! isVoiceActive())
        return;

    updateParameters();
    voiceBuffer.setSize(outputBuffer.getNumChannels(), numSamples, false, false, true);
    voiceBuffer.clear();

    const std::array<float, 3> levels {
        raw(parameters, "Osc1Level")->load(),
        raw(parameters, "Osc2Level")->load(),
        raw(parameters, "Osc3Level")->load()
    };

    const auto waveform = static_cast<int>(raw(parameters, "Waveform")->load());
    const auto pulseWidth = static_cast<double>(raw(parameters, "PulseWidth")->load());
    const auto drive = raw(parameters, "MixerDrive")->load();
    const auto cutoff = raw(parameters, "FilterCutoff")->load();
    const auto envAmount = raw(parameters, "FilterEnvAmount")->load();
    const auto resonance = raw(parameters, "FilterResonance")->load();

    for (auto sample = 0; sample < numSamples; ++sample)
    {
        float mixed = 0.0f;

        for (auto osc = 0; osc < 3; ++osc)
        {
            mixed += oscillatorSample(waveform, phases[static_cast<size_t>(osc)], pulseWidth) * levels[static_cast<size_t>(osc)];
            phases[static_cast<size_t>(osc)] += increments[static_cast<size_t>(osc)];
            phases[static_cast<size_t>(osc)] -= std::floor(phases[static_cast<size_t>(osc)]);
        }

        const auto driven = std::tanh(mixed * drive);
        const auto amp = ampEnvelope.getNextSample() * noteVelocity;
        const auto filterEnv = filterEnvelope.getNextSample();
        const auto modulatedCutoff = juce::jlimit(20.0f, 18000.0f, cutoff + envAmount * filterEnv);

        filter.setCutoffFrequencyHz(modulatedCutoff);
        filter.setResonance(resonance);

        const auto filtered = filter.processSample(driven, 0);
        const auto sampleValue = filtered * amp;

        for (auto channel = 0; channel < voiceBuffer.getNumChannels(); ++channel)
            voiceBuffer.setSample(channel, sample, sampleValue);
    }

    for (auto channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
        outputBuffer.addFrom(channel, startSample, voiceBuffer, channel, 0, numSamples);

    if (! ampEnvelope.isActive())
        clearCurrentNote();
}

SynthAudioProcessor::SynthAudioProcessor()
    : AudioProcessor(BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      parameters(*this, nullptr, treeId, createParameterLayout())
{
    synth.addSound(new AnalogSound());
    synth.addVoice(new AnalogVoice(parameters));
}

void SynthAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    synth.setCurrentPlaybackSampleRate(sampleRate);

    for (auto i = 0; i < synth.getNumVoices(); ++i)
        if (auto* voice = dynamic_cast<AnalogVoice*>(synth.getVoice(i)))
            voice->prepare(sampleRate, samplesPerBlock, getTotalNumOutputChannels());
}

void SynthAudioProcessor::releaseResources()
{
}

bool SynthAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::mono()
        || layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

void SynthAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();
    synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());

    const auto outputGain = raw(parameters, "OutputGain")->load();
    buffer.applyGain(outputGain);
}

juce::AudioProcessorEditor* SynthAudioProcessor::createEditor()
{
    return new SynthAudioProcessorEditor(*this);
}

bool SynthAudioProcessor::hasEditor() const { return true; }
const juce::String SynthAudioProcessor::getName() const { return JucePlugin_Name; }
bool SynthAudioProcessor::acceptsMidi() const { return true; }
bool SynthAudioProcessor::producesMidi() const { return false; }
bool SynthAudioProcessor::isMidiEffect() const { return false; }
double SynthAudioProcessor::getTailLengthSeconds() const { return 0.0; }
int SynthAudioProcessor::getNumPrograms() { return 1; }
int SynthAudioProcessor::getCurrentProgram() { return 0; }
void SynthAudioProcessor::setCurrentProgram(int) {}
const juce::String SynthAudioProcessor::getProgramName(int) { return {}; }
void SynthAudioProcessor::changeProgramName(int, const juce::String&) {}
juce::AudioProcessorValueTreeState& SynthAudioProcessor::getParameters() { return parameters; }

void SynthAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    if (auto state = parameters.copyState(); auto xml = state.createXml())
        copyXmlToBinary(*xml, destData);
}

void SynthAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary(data, sizeInBytes))
        if (xml->hasTagName(parameters.state.getType()))
            parameters.replaceState(juce::ValueTree::fromXml(*xml));
}

juce::AudioProcessorValueTreeState::ParameterLayout SynthAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    auto addFloat = [&params](const char* id, const char* name, float min, float max, float defaultValue, float skew = 1.0f)
    {
        auto range = juce::NormalisableRange<float>(min, max);
        range.setSkewForCentre(skew);
        params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID { id, 1 }, name, range, defaultValue));
    };

    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID { "Waveform", 1 }, "Waveform", juce::StringArray { "Sine", "Saw", "Pulse", "Triangle" }, 1));

    addFloat("Osc1Tune", "Osc 1 Tune", -24.0f, 24.0f, 0.0f);
    addFloat("Osc2Tune", "Osc 2 Tune", -24.0f, 24.0f, 0.0f);
    addFloat("Osc3Tune", "Osc 3 Tune", -24.0f, 24.0f, -12.0f);
    addFloat("Osc1Level", "Osc 1 Level", 0.0f, 1.0f, 0.85f);
    addFloat("Osc2Level", "Osc 2 Level", 0.0f, 1.0f, 0.35f);
    addFloat("Osc3Level", "Osc 3 Level", 0.0f, 1.0f, 0.25f);
    addFloat("PulseWidth", "Pulse Width", 0.05f, 0.95f, 0.5f);
    addFloat("MixerDrive", "Mixer Drive", 0.5f, 4.0f, 1.5f);
    addFloat("AnalogDrift", "Analog Drift", 0.0f, 0.25f, 0.035f);

    addFloat("FilterCutoff", "Filter Cutoff", 20.0f, 18000.0f, 1200.0f, 1000.0f);
    addFloat("FilterResonance", "Filter Resonance", 0.0f, 1.0f, 0.25f);
    addFloat("FilterEnvAmount", "Filter Env Amount", 0.0f, 8000.0f, 2200.0f, 1000.0f);

    addFloat("FilterAttack", "Filter Attack", 0.001f, 5.0f, 0.01f, 0.5f);
    addFloat("FilterDecay", "Filter Decay", 0.001f, 5.0f, 0.35f, 0.5f);
    addFloat("FilterSustain", "Filter Sustain", 0.0f, 1.0f, 0.25f);
    addFloat("FilterRelease", "Filter Release", 0.001f, 8.0f, 0.4f, 0.5f);

    addFloat("AmpAttack", "Amp Attack", 0.001f, 5.0f, 0.005f, 0.5f);
    addFloat("AmpDecay", "Amp Decay", 0.001f, 5.0f, 0.25f, 0.5f);
    addFloat("AmpSustain", "Amp Sustain", 0.0f, 1.0f, 0.8f);
    addFloat("AmpRelease", "Amp Release", 0.001f, 8.0f, 0.25f, 0.5f);

    addFloat("OutputGain", "Output Gain", 0.0f, 1.5f, 0.75f);

    return { params.begin(), params.end() };
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SynthAudioProcessor();
}
