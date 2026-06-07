#include "PluginProcessor.h"

#include <iostream>

namespace
{
void require(bool condition, const char* message)
{
    if (! condition)
    {
        std::cerr << message << "\n";
        std::exit(1);
    }
}

void setParameter(juce::AudioProcessorValueTreeState& state, const char* id, float value)
{
    auto* parameter = state.getParameter(id);
    require(parameter != nullptr, id);
    parameter->setValueNotifyingHost(parameter->convertTo0to1(value));
}

float renderEnergy(SynthAudioProcessor& processor, int note, int blocks)
{
    juce::AudioBuffer<float> buffer(2, 512);
    juce::MidiBuffer midi;
    midi.addEvent(juce::MidiMessage::noteOn(1, note, static_cast<juce::uint8>(110)), 0);
    midi.addEvent(juce::MidiMessage::channelPressureChange(1, 96), 64);

    float energy = 0.0f;

    for (auto block = 0; block < blocks; ++block)
    {
        processor.processBlock(buffer, midi);
        midi.clear();

        for (auto channel = 0; channel < buffer.getNumChannels(); ++channel)
        {
            const auto* data = buffer.getReadPointer(channel);
            for (auto sample = 0; sample < buffer.getNumSamples(); ++sample)
            {
                require(std::isfinite(data[sample]), "Rendered audio contains a non-finite sample");
                energy += std::abs(data[sample]);
            }
        }
    }

    midi.addEvent(juce::MidiMessage::noteOff(1, note), 0);
    processor.processBlock(buffer, midi);
    return energy;
}

float renderGuiKeyboardEnergy(SynthAudioProcessor& processor, int note, int blocks)
{
    juce::AudioBuffer<float> buffer(2, 512);
    juce::MidiBuffer midi;
    float energy = 0.0f;

    processor.getKeyboardState().noteOn(1, note, 0.9f);

    for (auto block = 0; block < blocks; ++block)
    {
        processor.processBlock(buffer, midi);
        midi.clear();

        for (auto channel = 0; channel < buffer.getNumChannels(); ++channel)
        {
            const auto* data = buffer.getReadPointer(channel);
            for (auto sample = 0; sample < buffer.getNumSamples(); ++sample)
            {
                require(std::isfinite(data[sample]), "GUI keyboard render contains a non-finite sample");
                energy += std::abs(data[sample]);
            }
        }
    }

    processor.getKeyboardState().noteOff(1, note, 0.0f);
    processor.processBlock(buffer, midi);
    return energy;
}
}

int main()
{
    SynthAudioProcessor processor;
    processor.setPlayConfigDetails(0, 2, 44100.0, 512);
    processor.prepareToPlay(44100.0, 512);

    auto& parameters = processor.getParameters();
    setParameter(parameters, "NoiseLevel", 0.1f);
    setParameter(parameters, "LfoDepth", 0.2f);
    setParameter(parameters, "LfoDestination", 2.0f);
    setParameter(parameters, "GlideTime", 0.05f);
    setParameter(parameters, "Mod1Source", 1.0f);
    setParameter(parameters, "Mod1Destination", 2.0f);
    setParameter(parameters, "Mod1Amount", 0.25f);
    setParameter(parameters, "DualFilterMode", 1.0f);
    setParameter(parameters, "ChorusMix", 0.1f);
    setParameter(parameters, "DelayMix", 0.08f);
    setParameter(parameters, "ReverbMix", 0.08f);

    auto energy = renderEnergy(processor, 48, 20);
    require(energy > 0.001f, "Mono render was silent");

    processor.setCurrentProgram(2);
    processor.prepareToPlay(44100.0, 512);
    energy = renderEnergy(processor, 60, 20);
    require(energy > 0.001f, "Preset render was silent");

    setParameter(parameters, "OutputGain", 0.0f);
    const auto mutedEnergy = renderEnergy(processor, 60, 10);
    require(mutedEnergy < 0.0001f, "OutputGain parameter did not mute rendered audio");

    setParameter(parameters, "OutputGain", 0.8f);
    setParameter(parameters, "Osc1Level", 1.0f);
    setParameter(parameters, "Osc2Level", 0.0f);
    setParameter(parameters, "Osc3Level", 0.0f);
    const auto unmutedEnergy = renderEnergy(processor, 60, 10);
    require(unmutedEnergy > 0.001f, "Parameter changes did not restore audible output");

    const auto guiKeyboardEnergy = renderGuiKeyboardEnergy(processor, 64, 10);
    require(guiKeyboardEnergy > 0.001f, "GUI piano roll did not produce audible output");

    setParameter(parameters, "Osc1Waveform", 1.0f);
    setParameter(parameters, "Osc2Waveform", 2.0f);
    setParameter(parameters, "Osc3Waveform", 1.0f);
    setParameter(parameters, "Osc1Octave", 4.0f);
    setParameter(parameters, "Osc2Octave", 3.0f);
    setParameter(parameters, "Osc3Octave", 2.0f);
    setParameter(parameters, "OutputGain", 1.0f);
    const auto highNoteEnergy = renderEnergy(processor, 96, 10);
    require(highNoteEnergy > 0.001f, "High-note PolyBLEP render was silent");

    juce::MemoryBlock state;
    processor.getStateInformation(state);
    require(state.getSize() > 0, "State serialisation produced no data");

    std::cout << "Synth smoke test passed\n";
    return 0;
}
