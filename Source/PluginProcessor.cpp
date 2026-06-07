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

struct ProgramValue
{
    const char* id;
    float value;
};

struct Program
{
    const char* name;
    std::initializer_list<ProgramValue> values;
};

const Program programs[] {
    { "Init Bass", {
        { "Waveform", 1.0f }, { "Osc1Level", 0.9f }, { "Osc2Level", 0.45f }, { "Osc3Level", 0.25f },
        { "Osc3Tune", -12.0f }, { "FilterCutoff", 850.0f }, { "FilterResonance", 0.22f },
        { "FilterEnvAmount", 2400.0f }, { "AmpSustain", 0.75f }, { "MixerDrive", 1.8f }
    } },
    { "Warm Lead", {
        { "Waveform", 1.0f }, { "Osc1Level", 0.75f }, { "Osc2Tune", 7.0f }, { "Osc2Level", 0.35f },
        { "Osc3Level", 0.0f }, { "FilterCutoff", 2200.0f }, { "FilterResonance", 0.32f },
        { "LfoRate", 5.0f }, { "LfoDepth", 0.12f }, { "GlideTime", 0.08f }
    } },
    { "Soft Pad", {
        { "VoiceMode", 1.0f }, { "Waveform", 3.0f }, { "Osc1Level", 0.5f }, { "Osc2Tune", 0.12f },
        { "Osc2Level", 0.45f }, { "Osc3Tune", -12.0f }, { "FilterCutoff", 3100.0f },
        { "AmpAttack", 0.75f }, { "AmpRelease", 1.8f }, { "ChorusMix", 0.35f }, { "ReverbMix", 0.25f }
    } },
    { "Noise Sweep", {
        { "Osc1Level", 0.0f }, { "Osc2Level", 0.0f }, { "Osc3Level", 0.0f }, { "NoiseLevel", 0.65f },
        { "FilterCutoff", 500.0f }, { "FilterResonance", 0.62f }, { "FilterEnvAmount", 6500.0f },
        { "FilterAttack", 0.4f }, { "FilterDecay", 1.2f }, { "DelayMix", 0.18f }
    } }
};

void setProgramValue(juce::AudioProcessorValueTreeState& state, const char* id, float value)
{
    if (auto* parameter = state.getParameter(id))
    {
        const auto normalised = parameter->convertTo0to1(value);
        parameter->setValueNotifyingHost(normalised);
    }
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
    filter2.prepare(spec);
    filter.setMode(juce::dsp::LadderFilterMode::LPF24);
    filter2.setMode(juce::dsp::LadderFilterMode::LPF24);
    filter.reset();
    filter2.reset();
}

void SynthAudioProcessor::AnalogVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound*, int)
{
    targetFrequency = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
    if (! isVoiceActive())
        currentFrequency = targetFrequency;

    baseFrequency = targetFrequency;
    noteVelocity = velocity;

    const auto drift = raw(parameters, "AnalogDrift")->load();
    const auto voiceVariance = raw(parameters, "VoiceVariance")->load();
    for (auto i = 0; i < 3; ++i)
    {
        phases[static_cast<size_t>(i)] = random.nextDouble();
        driftOffsets[static_cast<size_t>(i)] = random.nextFloat() * (drift + voiceVariance) * 2.0f - (drift + voiceVariance);
    }

    ampEnvelope.setParameters(envelopeFrom(parameters, "Amp"));
    filterEnvelope.setParameters(envelopeFrom(parameters, "Filter"));
    filter.reset();
    filter2.reset();
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

void SynthAudioProcessor::AnalogVoice::aftertouchChanged(int newAftertouchValue)
{
    aftertouch = static_cast<float>(newAftertouchValue) / 127.0f;
}

void SynthAudioProcessor::AnalogVoice::channelPressureChanged(int newChannelPressureValue)
{
    aftertouch = static_cast<float>(newChannelPressureValue) / 127.0f;
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

float SynthAudioProcessor::AnalogVoice::modulationSource(int source, float lfo, float velocity, float pressure, float envelope) const
{
    switch (source)
    {
        case 1: return lfo;
        case 2: return velocity;
        case 3: return pressure;
        case 4: return envelope;
        default: return 0.0f;
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
        const auto frequency = currentFrequency * std::pow(2.0, static_cast<double>(semitones) / 12.0);
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
    const auto noiseLevel = raw(parameters, "NoiseLevel")->load();
    const auto drive = raw(parameters, "MixerDrive")->load();
    const auto cutoff = raw(parameters, "FilterCutoff")->load();
    const auto envAmount = raw(parameters, "FilterEnvAmount")->load();
    const auto resonance = raw(parameters, "FilterResonance")->load();
    const auto lfoRate = raw(parameters, "LfoRate")->load();
    const auto lfoDepth = raw(parameters, "LfoDepth")->load();
    const auto lfoDestination = static_cast<int>(raw(parameters, "LfoDestination")->load());
    const auto glideTime = raw(parameters, "GlideTime")->load();
    const auto velocityToCutoff = raw(parameters, "VelocityToCutoff")->load();
    const auto aftertouchToCutoff = raw(parameters, "AftertouchToCutoff")->load();
    const auto filterDrive = raw(parameters, "FilterDrive")->load();
    const auto outputSaturation = raw(parameters, "OutputSaturation")->load();
    const auto driftRate = raw(parameters, "DriftRate")->load();
    const auto dualFilterMode = static_cast<int>(raw(parameters, "DualFilterMode")->load());
    const auto filter2Cutoff = raw(parameters, "Filter2Cutoff")->load();
    const auto filter2Resonance = raw(parameters, "Filter2Resonance")->load();
    const auto mod1Source = static_cast<int>(raw(parameters, "Mod1Source")->load());
    const auto mod1Destination = static_cast<int>(raw(parameters, "Mod1Destination")->load());
    const auto mod1Amount = raw(parameters, "Mod1Amount")->load();
    const auto mod2Source = static_cast<int>(raw(parameters, "Mod2Source")->load());
    const auto mod2Destination = static_cast<int>(raw(parameters, "Mod2Destination")->load());
    const auto mod2Amount = raw(parameters, "Mod2Amount")->load();
    const auto glideCoefficient = glideTime <= 0.001f ? 0.0 : std::exp(-1.0 / (static_cast<double>(glideTime) * currentSampleRate));

    for (auto sample = 0; sample < numSamples; ++sample)
    {
        float mixed = 0.0f;
        const auto lfo = static_cast<float>(std::sin(juce::MathConstants<double>::twoPi * lfoPhase));
        lfoPhase += static_cast<double>(lfoRate) / currentSampleRate;
        lfoPhase -= std::floor(lfoPhase);

        currentFrequency = targetFrequency + (currentFrequency - targetFrequency) * glideCoefficient;
        baseFrequency = currentFrequency;

        const auto filterEnv = filterEnvelope.getNextSample();
        const auto mod1 = modulationSource(mod1Source, lfo, noteVelocity, aftertouch, filterEnv) * mod1Amount;
        const auto mod2 = modulationSource(mod2Source, lfo, noteVelocity, aftertouch, filterEnv) * mod2Amount;
        float pitchMod = lfoDestination == 1 ? lfo * lfoDepth * 12.0f : 0.0f;
        float cutoffMod = (lfoDestination == 2 ? lfo * lfoDepth * 5000.0f : 0.0f)
            + velocityToCutoff * noteVelocity
            + aftertouchToCutoff * aftertouch;
        float pulseWidthMod = lfoDestination == 3 ? lfo * lfoDepth * 0.4f : 0.0f;
        float ampMod = lfoDestination == 4 ? lfo * lfoDepth : 0.0f;
        float resonanceMod = 0.0f;

        auto applyMatrix = [&pitchMod, &cutoffMod, &pulseWidthMod, &ampMod, &resonanceMod](int destination, float value)
        {
            switch (destination)
            {
                case 1: pitchMod += value * 12.0f; break;
                case 2: cutoffMod += value * 5000.0f; break;
                case 3: pulseWidthMod += value * 0.4f; break;
                case 4: ampMod += value; break;
                case 5: resonanceMod += value * 0.5f; break;
                default: break;
            }
        };

        applyMatrix(mod1Destination, mod1);
        applyMatrix(mod2Destination, mod2);

        for (auto osc = 0; osc < 3; ++osc)
        {
            const auto drift = std::sin(lfoPhase * juce::MathConstants<double>::twoPi * (1.0 + osc)) * driftRate;
            const auto semitoneBend = pitchMod + static_cast<float>(drift);
            const auto frequencyRatio = std::pow(2.0, static_cast<double>(semitoneBend) / 12.0);
            const auto width = juce::jlimit(0.05, 0.95, pulseWidth + static_cast<double>(pulseWidthMod));
            mixed += oscillatorSample(waveform, phases[static_cast<size_t>(osc)], width) * levels[static_cast<size_t>(osc)];
            phases[static_cast<size_t>(osc)] += increments[static_cast<size_t>(osc)] * frequencyRatio;
            phases[static_cast<size_t>(osc)] -= std::floor(phases[static_cast<size_t>(osc)]);
        }

        mixed += (random.nextFloat() * 2.0f - 1.0f) * noiseLevel;

        const auto driven = juce::jlimit(-0.98f, 0.98f, std::tanh(mixed * drive));
        const auto amp = ampEnvelope.getNextSample() * noteVelocity;
        const auto modulatedCutoff = juce::jlimit(20.0f, 18000.0f, cutoff + envAmount * filterEnv + cutoffMod);

        filter.setCutoffFrequencyHz(modulatedCutoff);
        filter.setResonance(juce::jlimit(0.0f, 0.92f, resonance + resonanceMod));
        filter.setDrive(juce::jlimit(1.0f, 4.0f, filterDrive));
        filter2.setCutoffFrequencyHz(filter2Cutoff);
        filter2.setResonance(juce::jlimit(0.0f, 0.92f, filter2Resonance));
        filter2.setDrive(juce::jlimit(1.0f, 4.0f, filterDrive));

        const auto primary = filter.processSample(driven, 0);
        auto filtered = primary;

        if (dualFilterMode == 1)
            filtered = filter2.processSample(primary, 0);
        else if (dualFilterMode == 2)
            filtered = (primary + filter2.processSample(driven, 0)) * 0.5f;

        if (! std::isfinite(filtered))
        {
            filter.reset();
            filter2.reset();
            filtered = 0.0f;
        }

        const auto saturated = std::tanh(juce::jlimit(-8.0f, 8.0f, filtered * outputSaturation));
        const auto sampleValue = saturated * amp * juce::jlimit(0.0f, 2.0f, 1.0f + ampMod);

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
    auto logRoot = juce::File(juce::SystemStats::getEnvironmentVariable("LOCALAPPDATA", {}));
    if (! logRoot.isDirectory())
        logRoot = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory);

    const auto logDirectory = logRoot.getChildFile("Synth");
    logDirectory.createDirectory();
    logger = std::make_unique<juce::FileLogger>(logDirectory.getChildFile("debug.log"), "Synth debug log", 512 * 1024);

    for (auto* parameter : AudioProcessor::getParameters())
    {
        if (auto* ranged = dynamic_cast<juce::RangedAudioParameter*>(parameter))
        {
            parameterIds.add(ranged->paramID);
            parameters.addParameterListener(ranged->paramID, this);
        }
    }

    debugLog("Processor constructed. Log file: " + logger->getLogFile().getFullPathName());
    synth.addSound(new AnalogSound());
    configureVoiceCount();
}

SynthAudioProcessor::~SynthAudioProcessor()
{
    for (const auto& id : parameterIds)
        parameters.removeParameterListener(id, this);

    debugLog("Processor destroyed");
}

void SynthAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    debugLog("prepareToPlay sampleRate=" + juce::String(sampleRate)
        + " blockSize=" + juce::String(samplesPerBlock)
        + " outputs=" + juce::String(getTotalNumOutputChannels()));
    preparedSampleRate = sampleRate;
    preparedSamplesPerBlock = samplesPerBlock;
    preparedOutputChannels = getTotalNumOutputChannels();
    configureVoiceCount();
    synth.setCurrentPlaybackSampleRate(sampleRate);

    for (auto i = 0; i < synth.getNumVoices(); ++i)
        if (auto* voice = dynamic_cast<AnalogVoice*>(synth.getVoice(i)))
            voice->prepare(sampleRate, samplesPerBlock, getTotalNumOutputChannels());

    juce::dsp::ProcessSpec spec { sampleRate, static_cast<juce::uint32>(samplesPerBlock), static_cast<juce::uint32>(getTotalNumOutputChannels()) };
    chorus.prepare(spec);
    delayBuffer.setSize(getTotalNumOutputChannels(), static_cast<int>(sampleRate * 2.0), false, false, true);
    delayBuffer.clear();
    delayWritePosition = 0;
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
    configureVoiceCount();
    buffer.clear();
    keyboardState.processNextMidiBuffer(midiMessages, 0, buffer.getNumSamples(), true);
    synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
    applyEffects(buffer);

    const auto outputGain = raw(parameters, "OutputGain")->load();
    buffer.applyGain(outputGain);

    if (--processLogCountdown <= 0)
    {
        processLogCountdown = 120;
        debugLog("process snapshot: cutoff=" + juce::String(raw(parameters, "FilterCutoff")->load(), 1)
            + " osc1=" + juce::String(raw(parameters, "Osc1Level")->load(), 3)
            + " osc2=" + juce::String(raw(parameters, "Osc2Level")->load(), 3)
            + " osc3=" + juce::String(raw(parameters, "Osc3Level")->load(), 3)
            + " noise=" + juce::String(raw(parameters, "NoiseLevel")->load(), 3)
            + " output=" + juce::String(outputGain, 3)
            + " midiEvents=" + juce::String(midiMessages.getNumEvents()));
    }
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
int SynthAudioProcessor::getNumPrograms() { return static_cast<int>(std::size(programs)); }
int SynthAudioProcessor::getCurrentProgram() { return currentProgram; }
void SynthAudioProcessor::setCurrentProgram(int index) { loadProgram(index); }
const juce::String SynthAudioProcessor::getProgramName(int index)
{
    return juce::isPositiveAndBelow(index, static_cast<int>(std::size(programs))) ? programs[index].name : juce::String {};
}
void SynthAudioProcessor::changeProgramName(int, const juce::String&) {}
juce::AudioProcessorValueTreeState& SynthAudioProcessor::getParameters() { return parameters; }
juce::MidiKeyboardState& SynthAudioProcessor::getKeyboardState() { return keyboardState; }

void SynthAudioProcessor::configureVoiceCount()
{
    const auto desiredVoiceCount = raw(parameters, "VoiceMode")->load() < 0.5f ? 1 : 8;
    if (configuredVoiceCount == desiredVoiceCount)
        return;

    synth.clearVoices();
    for (auto i = 0; i < desiredVoiceCount; ++i)
    {
        auto* voice = new AnalogVoice(parameters);
        if (preparedSampleRate > 0.0)
            voice->prepare(preparedSampleRate, preparedSamplesPerBlock, preparedOutputChannels);

        synth.addVoice(voice);
    }

    configuredVoiceCount = desiredVoiceCount;
    debugLog("voice count configured: " + juce::String(configuredVoiceCount));
}

void SynthAudioProcessor::applyEffects(juce::AudioBuffer<float>& buffer)
{
    const auto chorusMix = raw(parameters, "ChorusMix")->load();
    const auto delayMix = raw(parameters, "DelayMix")->load();
    const auto delayFeedback = raw(parameters, "DelayFeedback")->load();
    const auto delayTime = raw(parameters, "DelayTime")->load();
    const auto reverbMix = raw(parameters, "ReverbMix")->load();

    if (chorusMix > 0.001f)
    {
        chorus.setRate(0.35f);
        chorus.setDepth(0.22f);
        chorus.setMix(chorusMix);
        juce::dsp::AudioBlock<float> block(buffer);
        chorus.process(juce::dsp::ProcessContextReplacing<float>(block));
    }

    if (delayMix > 0.001f && delayBuffer.getNumSamples() > 0)
    {
        const auto delaySamples = juce::jlimit(1, delayBuffer.getNumSamples() - 1, static_cast<int>(delayTime * getSampleRate()));

        for (auto sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            const auto readPosition = (delayWritePosition + delayBuffer.getNumSamples() - delaySamples) % delayBuffer.getNumSamples();

            for (auto channel = 0; channel < buffer.getNumChannels(); ++channel)
            {
                const auto input = buffer.getSample(channel, sample);
                const auto delayed = delayBuffer.getSample(channel, readPosition);
                buffer.setSample(channel, sample, input + delayed * delayMix);
                delayBuffer.setSample(channel, delayWritePosition, input + delayed * delayFeedback);
            }

            delayWritePosition = (delayWritePosition + 1) % delayBuffer.getNumSamples();
        }
    }

    if (reverbMix > 0.001f && buffer.getNumChannels() >= 2)
    {
        juce::Reverb::Parameters reverbParams;
        reverbParams.roomSize = 0.45f;
        reverbParams.damping = 0.35f;
        reverbParams.wetLevel = reverbMix;
        reverbParams.dryLevel = 1.0f - reverbMix * 0.35f;
        reverb.setParameters(reverbParams);
        reverb.processStereo(buffer.getWritePointer(0), buffer.getWritePointer(1), buffer.getNumSamples());
    }
}

void SynthAudioProcessor::loadProgram(int index)
{
    if (! juce::isPositiveAndBelow(index, static_cast<int>(std::size(programs))))
        return;

    currentProgram = index;
    debugLog("loading program: " + juce::String(programs[index].name));
    for (const auto& value : programs[index].values)
        setProgramValue(parameters, value.id, value.value);
}

void SynthAudioProcessor::parameterChanged(const juce::String& parameterID, float newValue)
{
    debugLog("parameterChanged " + parameterID + "=" + juce::String(newValue, 4));
}

void SynthAudioProcessor::debugLog(const juce::String& message)
{
    if (logger != nullptr)
        logger->logMessage(message);
}

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

    auto addFloat = [&params](const char* id, const char* name, float min, float max, float defaultValue, float centre = 0.0f)
    {
        auto range = juce::NormalisableRange<float>(min, max);
        if (centre > min && centre < max)
            range.setSkewForCentre(centre);

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
    addFloat("NoiseLevel", "Noise Level", 0.0f, 1.0f, 0.0f);
    addFloat("PulseWidth", "Pulse Width", 0.05f, 0.95f, 0.5f);
    addFloat("MixerDrive", "Mixer Drive", 0.5f, 4.0f, 1.5f);
    addFloat("AnalogDrift", "Analog Drift", 0.0f, 0.25f, 0.035f);
    addFloat("DriftRate", "Drift Rate", 0.0f, 0.2f, 0.015f);
    addFloat("VoiceVariance", "Voice Variance", 0.0f, 0.2f, 0.02f);

    addFloat("FilterCutoff", "Filter Cutoff", 20.0f, 18000.0f, 1200.0f, 1000.0f);
    addFloat("FilterResonance", "Filter Resonance", 0.0f, 1.0f, 0.25f);
    addFloat("FilterEnvAmount", "Filter Env Amount", 0.0f, 8000.0f, 2200.0f, 1000.0f);
    addFloat("FilterDrive", "Filter Drive", 1.0f, 8.0f, 1.2f);
    addFloat("Filter2Cutoff", "Filter 2 Cutoff", 20.0f, 18000.0f, 2400.0f, 1000.0f);
    addFloat("Filter2Resonance", "Filter 2 Resonance", 0.0f, 1.0f, 0.15f);
    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID { "DualFilterMode", 1 }, "Dual Filter Mode", juce::StringArray { "Off", "Serial", "Parallel" }, 0));

    addFloat("FilterAttack", "Filter Attack", 0.001f, 5.0f, 0.01f, 0.5f);
    addFloat("FilterDecay", "Filter Decay", 0.001f, 5.0f, 0.35f, 0.5f);
    addFloat("FilterSustain", "Filter Sustain", 0.0f, 1.0f, 0.25f);
    addFloat("FilterRelease", "Filter Release", 0.001f, 8.0f, 0.4f, 0.5f);

    addFloat("AmpAttack", "Amp Attack", 0.001f, 5.0f, 0.005f, 0.5f);
    addFloat("AmpDecay", "Amp Decay", 0.001f, 5.0f, 0.25f, 0.5f);
    addFloat("AmpSustain", "Amp Sustain", 0.0f, 1.0f, 0.8f);
    addFloat("AmpRelease", "Amp Release", 0.001f, 8.0f, 0.25f, 0.5f);

    addFloat("LfoRate", "LFO Rate", 0.05f, 20.0f, 4.5f, 2.0f);
    addFloat("LfoDepth", "LFO Depth", 0.0f, 1.0f, 0.0f);
    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID { "LfoDestination", 1 }, "LFO Destination", juce::StringArray { "Off", "Pitch", "Cutoff", "PWM", "Amp" }, 0));
    addFloat("GlideTime", "Glide Time", 0.0f, 2.0f, 0.0f, 0.25f);
    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID { "VoiceMode", 1 }, "Voice Mode", juce::StringArray { "Mono", "Poly" }, 0));

    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID { "Mod1Source", 1 }, "Mod 1 Source", juce::StringArray { "Off", "LFO", "Velocity", "Aftertouch", "Filter Env" }, 0));
    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID { "Mod1Destination", 1 }, "Mod 1 Destination", juce::StringArray { "Off", "Pitch", "Cutoff", "PWM", "Amp", "Resonance" }, 0));
    addFloat("Mod1Amount", "Mod 1 Amount", -1.0f, 1.0f, 0.0f);
    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID { "Mod2Source", 1 }, "Mod 2 Source", juce::StringArray { "Off", "LFO", "Velocity", "Aftertouch", "Filter Env" }, 0));
    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID { "Mod2Destination", 1 }, "Mod 2 Destination", juce::StringArray { "Off", "Pitch", "Cutoff", "PWM", "Amp", "Resonance" }, 0));
    addFloat("Mod2Amount", "Mod 2 Amount", -1.0f, 1.0f, 0.0f);

    addFloat("VelocityToCutoff", "Velocity To Cutoff", 0.0f, 8000.0f, 400.0f, 1000.0f);
    addFloat("AftertouchToCutoff", "Aftertouch To Cutoff", 0.0f, 8000.0f, 1200.0f, 1000.0f);
    addFloat("ChorusMix", "Chorus Mix", 0.0f, 1.0f, 0.0f);
    addFloat("DelayMix", "Delay Mix", 0.0f, 1.0f, 0.0f);
    addFloat("DelayFeedback", "Delay Feedback", 0.0f, 0.9f, 0.25f);
    addFloat("DelayTime", "Delay Time", 0.02f, 1.5f, 0.32f, 0.4f);
    addFloat("ReverbMix", "Reverb Mix", 0.0f, 1.0f, 0.0f);
    addFloat("OutputSaturation", "Output Saturation", 1.0f, 4.0f, 1.2f);
    addFloat("OutputGain", "Output Gain", 0.0f, 1.5f, 0.75f);

    return { params.begin(), params.end() };
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SynthAudioProcessor();
}
