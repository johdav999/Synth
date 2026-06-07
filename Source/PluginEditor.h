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
    class EditableValueSlider final : public juce::Slider
    {
    public:
        void mouseDoubleClick(const juce::MouseEvent&) override
        {
            showTextBox();
        }
    };

    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ComboAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

    void addKnob(juce::Slider& slider, juce::Label& label, const juce::String& text);
    void addEnvelopeSlider(juce::Slider& slider, juce::Label& label, const juce::String& text);
    void addCombo(juce::ComboBox& combo, juce::Label& label, const juce::String& text);
    void refocusPianoKeyboard();
    void drawSection(juce::Graphics& g, juce::Rectangle<int> bounds, const juce::String& title);
    void drawScrew(juce::Graphics& g, int x, int y);

    SynthAudioProcessor& processor;
    std::unique_ptr<juce::LookAndFeel_V4> retroLookAndFeel;
    juce::MidiKeyboardComponent pianoKeyboard;

    juce::ComboBox waveform;
    juce::ComboBox voiceMode;
    juce::ComboBox lfoDestination;
    juce::ComboBox dualFilterMode;
    juce::Label waveformLabel;
    juce::Label voiceModeLabel;
    juce::Label lfoDestinationLabel;
    juce::Label dualFilterModeLabel;

    EditableValueSlider osc1Tune;
    EditableValueSlider osc2Tune;
    EditableValueSlider osc3Tune;
    EditableValueSlider osc1Level;
    EditableValueSlider osc2Level;
    EditableValueSlider osc3Level;
    EditableValueSlider noiseLevel;
    EditableValueSlider pulseWidth;
    EditableValueSlider mixerDrive;
    EditableValueSlider analogDrift;
    EditableValueSlider driftRate;
    EditableValueSlider filterCutoff;
    EditableValueSlider filterResonance;
    EditableValueSlider filterEnvAmount;
    EditableValueSlider filterDrive;
    EditableValueSlider filterAttack;
    EditableValueSlider filterDecay;
    EditableValueSlider filterSustain;
    EditableValueSlider filterRelease;
    EditableValueSlider ampAttack;
    EditableValueSlider ampDecay;
    EditableValueSlider ampSustain;
    EditableValueSlider ampRelease;
    EditableValueSlider lfoRate;
    EditableValueSlider lfoDepth;
    EditableValueSlider glideTime;
    EditableValueSlider chorusMix;
    EditableValueSlider delayMix;
    EditableValueSlider reverbMix;
    EditableValueSlider outputGain;

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
