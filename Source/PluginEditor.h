#pragma once

#include "PluginProcessor.h"

class SynthAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit SynthAudioProcessorEditor(SynthAudioProcessor&);
    ~SynthAudioProcessorEditor() override = default;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ComboAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

    void addKnob(juce::Slider& slider, juce::Label& label, const juce::String& text);

    SynthAudioProcessor& processor;

    juce::ComboBox waveform;
    juce::Label waveformLabel;

    juce::Slider osc1Tune;
    juce::Slider osc2Tune;
    juce::Slider osc3Tune;
    juce::Slider osc1Level;
    juce::Slider osc2Level;
    juce::Slider osc3Level;
    juce::Slider pulseWidth;
    juce::Slider mixerDrive;
    juce::Slider analogDrift;
    juce::Slider filterCutoff;
    juce::Slider filterResonance;
    juce::Slider filterEnvAmount;
    juce::Slider filterAttack;
    juce::Slider filterDecay;
    juce::Slider filterSustain;
    juce::Slider filterRelease;
    juce::Slider ampAttack;
    juce::Slider ampDecay;
    juce::Slider ampSustain;
    juce::Slider ampRelease;
    juce::Slider outputGain;

    juce::Label osc1TuneLabel;
    juce::Label osc2TuneLabel;
    juce::Label osc3TuneLabel;
    juce::Label osc1LevelLabel;
    juce::Label osc2LevelLabel;
    juce::Label osc3LevelLabel;
    juce::Label pulseWidthLabel;
    juce::Label mixerDriveLabel;
    juce::Label analogDriftLabel;
    juce::Label filterCutoffLabel;
    juce::Label filterResonanceLabel;
    juce::Label filterEnvAmountLabel;
    juce::Label filterAttackLabel;
    juce::Label filterDecayLabel;
    juce::Label filterSustainLabel;
    juce::Label filterReleaseLabel;
    juce::Label ampAttackLabel;
    juce::Label ampDecayLabel;
    juce::Label ampSustainLabel;
    juce::Label ampReleaseLabel;
    juce::Label outputGainLabel;

    ComboAttachment waveformAttachment;
    SliderAttachment osc1TuneAttachment;
    SliderAttachment osc2TuneAttachment;
    SliderAttachment osc3TuneAttachment;
    SliderAttachment osc1LevelAttachment;
    SliderAttachment osc2LevelAttachment;
    SliderAttachment osc3LevelAttachment;
    SliderAttachment pulseWidthAttachment;
    SliderAttachment mixerDriveAttachment;
    SliderAttachment analogDriftAttachment;
    SliderAttachment filterCutoffAttachment;
    SliderAttachment filterResonanceAttachment;
    SliderAttachment filterEnvAmountAttachment;
    SliderAttachment filterAttackAttachment;
    SliderAttachment filterDecayAttachment;
    SliderAttachment filterSustainAttachment;
    SliderAttachment filterReleaseAttachment;
    SliderAttachment ampAttackAttachment;
    SliderAttachment ampDecayAttachment;
    SliderAttachment ampSustainAttachment;
    SliderAttachment ampReleaseAttachment;
    SliderAttachment outputGainAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SynthAudioProcessorEditor)
};
