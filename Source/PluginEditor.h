#pragma once

#include "PluginProcessor.h"

class SynthAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit SynthAudioProcessorEditor(SynthAudioProcessor&);
    ~SynthAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ComboAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

    void addKnob(juce::Slider& slider, juce::Label& label, const juce::String& text);
    void addEnvelopeSlider(juce::Slider& slider, juce::Label& label, const juce::String& text);
    void addCombo(juce::ComboBox& combo, juce::Label& label, const juce::String& text);
    void drawSection(juce::Graphics& g, juce::Rectangle<int> bounds, const juce::String& title);
    void drawScrew(juce::Graphics& g, int x, int y);

    SynthAudioProcessor& processor;
    std::unique_ptr<juce::LookAndFeel_V4> retroLookAndFeel;

    juce::ComboBox waveform;
    juce::ComboBox voiceMode;
    juce::ComboBox lfoDestination;
    juce::ComboBox dualFilterMode;
    juce::Label waveformLabel;
    juce::Label voiceModeLabel;
    juce::Label lfoDestinationLabel;
    juce::Label dualFilterModeLabel;

    juce::Slider osc1Tune;
    juce::Slider osc2Tune;
    juce::Slider osc3Tune;
    juce::Slider osc1Level;
    juce::Slider osc2Level;
    juce::Slider osc3Level;
    juce::Slider noiseLevel;
    juce::Slider pulseWidth;
    juce::Slider mixerDrive;
    juce::Slider analogDrift;
    juce::Slider driftRate;
    juce::Slider filterCutoff;
    juce::Slider filterResonance;
    juce::Slider filterEnvAmount;
    juce::Slider filterDrive;
    juce::Slider filterAttack;
    juce::Slider filterDecay;
    juce::Slider filterSustain;
    juce::Slider filterRelease;
    juce::Slider ampAttack;
    juce::Slider ampDecay;
    juce::Slider ampSustain;
    juce::Slider ampRelease;
    juce::Slider lfoRate;
    juce::Slider lfoDepth;
    juce::Slider glideTime;
    juce::Slider chorusMix;
    juce::Slider delayMix;
    juce::Slider reverbMix;
    juce::Slider outputGain;

    juce::Label osc1TuneLabel;
    juce::Label osc2TuneLabel;
    juce::Label osc3TuneLabel;
    juce::Label osc1LevelLabel;
    juce::Label osc2LevelLabel;
    juce::Label osc3LevelLabel;
    juce::Label noiseLevelLabel;
    juce::Label pulseWidthLabel;
    juce::Label mixerDriveLabel;
    juce::Label analogDriftLabel;
    juce::Label driftRateLabel;
    juce::Label filterCutoffLabel;
    juce::Label filterResonanceLabel;
    juce::Label filterEnvAmountLabel;
    juce::Label filterDriveLabel;
    juce::Label filterAttackLabel;
    juce::Label filterDecayLabel;
    juce::Label filterSustainLabel;
    juce::Label filterReleaseLabel;
    juce::Label ampAttackLabel;
    juce::Label ampDecayLabel;
    juce::Label ampSustainLabel;
    juce::Label ampReleaseLabel;
    juce::Label lfoRateLabel;
    juce::Label lfoDepthLabel;
    juce::Label glideTimeLabel;
    juce::Label chorusMixLabel;
    juce::Label delayMixLabel;
    juce::Label reverbMixLabel;
    juce::Label outputGainLabel;

    ComboAttachment waveformAttachment;
    ComboAttachment voiceModeAttachment;
    ComboAttachment lfoDestinationAttachment;
    ComboAttachment dualFilterModeAttachment;
    SliderAttachment osc1TuneAttachment;
    SliderAttachment osc2TuneAttachment;
    SliderAttachment osc3TuneAttachment;
    SliderAttachment osc1LevelAttachment;
    SliderAttachment osc2LevelAttachment;
    SliderAttachment osc3LevelAttachment;
    SliderAttachment noiseLevelAttachment;
    SliderAttachment pulseWidthAttachment;
    SliderAttachment mixerDriveAttachment;
    SliderAttachment analogDriftAttachment;
    SliderAttachment driftRateAttachment;
    SliderAttachment filterCutoffAttachment;
    SliderAttachment filterResonanceAttachment;
    SliderAttachment filterEnvAmountAttachment;
    SliderAttachment filterDriveAttachment;
    SliderAttachment filterAttackAttachment;
    SliderAttachment filterDecayAttachment;
    SliderAttachment filterSustainAttachment;
    SliderAttachment filterReleaseAttachment;
    SliderAttachment ampAttackAttachment;
    SliderAttachment ampDecayAttachment;
    SliderAttachment ampSustainAttachment;
    SliderAttachment ampReleaseAttachment;
    SliderAttachment lfoRateAttachment;
    SliderAttachment lfoDepthAttachment;
    SliderAttachment glideTimeAttachment;
    SliderAttachment chorusMixAttachment;
    SliderAttachment delayMixAttachment;
    SliderAttachment reverbMixAttachment;
    SliderAttachment outputGainAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SynthAudioProcessorEditor)
};
