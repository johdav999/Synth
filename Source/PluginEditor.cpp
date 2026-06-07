#include "PluginEditor.h"

SynthAudioProcessorEditor::SynthAudioProcessorEditor(SynthAudioProcessor& p)
    : AudioProcessorEditor(&p),
      processor(p),
      waveformAttachment(processor.getParameters(), "Waveform", waveform),
      voiceModeAttachment(processor.getParameters(), "VoiceMode", voiceMode),
      lfoDestinationAttachment(processor.getParameters(), "LfoDestination", lfoDestination),
      dualFilterModeAttachment(processor.getParameters(), "DualFilterMode", dualFilterMode),
      osc1TuneAttachment(processor.getParameters(), "Osc1Tune", osc1Tune),
      osc2TuneAttachment(processor.getParameters(), "Osc2Tune", osc2Tune),
      osc3TuneAttachment(processor.getParameters(), "Osc3Tune", osc3Tune),
      osc1LevelAttachment(processor.getParameters(), "Osc1Level", osc1Level),
      osc2LevelAttachment(processor.getParameters(), "Osc2Level", osc2Level),
      osc3LevelAttachment(processor.getParameters(), "Osc3Level", osc3Level),
      noiseLevelAttachment(processor.getParameters(), "NoiseLevel", noiseLevel),
      pulseWidthAttachment(processor.getParameters(), "PulseWidth", pulseWidth),
      mixerDriveAttachment(processor.getParameters(), "MixerDrive", mixerDrive),
      analogDriftAttachment(processor.getParameters(), "AnalogDrift", analogDrift),
      driftRateAttachment(processor.getParameters(), "DriftRate", driftRate),
      filterCutoffAttachment(processor.getParameters(), "FilterCutoff", filterCutoff),
      filterResonanceAttachment(processor.getParameters(), "FilterResonance", filterResonance),
      filterEnvAmountAttachment(processor.getParameters(), "FilterEnvAmount", filterEnvAmount),
      filterDriveAttachment(processor.getParameters(), "FilterDrive", filterDrive),
      filterAttackAttachment(processor.getParameters(), "FilterAttack", filterAttack),
      filterDecayAttachment(processor.getParameters(), "FilterDecay", filterDecay),
      filterSustainAttachment(processor.getParameters(), "FilterSustain", filterSustain),
      filterReleaseAttachment(processor.getParameters(), "FilterRelease", filterRelease),
      ampAttackAttachment(processor.getParameters(), "AmpAttack", ampAttack),
      ampDecayAttachment(processor.getParameters(), "AmpDecay", ampDecay),
      ampSustainAttachment(processor.getParameters(), "AmpSustain", ampSustain),
      ampReleaseAttachment(processor.getParameters(), "AmpRelease", ampRelease),
      lfoRateAttachment(processor.getParameters(), "LfoRate", lfoRate),
      lfoDepthAttachment(processor.getParameters(), "LfoDepth", lfoDepth),
      glideTimeAttachment(processor.getParameters(), "GlideTime", glideTime),
      chorusMixAttachment(processor.getParameters(), "ChorusMix", chorusMix),
      delayMixAttachment(processor.getParameters(), "DelayMix", delayMix),
      reverbMixAttachment(processor.getParameters(), "ReverbMix", reverbMix),
      outputGainAttachment(processor.getParameters(), "OutputGain", outputGain)
{
    setSize(1120, 620);

    waveform.addItemList({ "Sine", "Saw", "Pulse", "Triangle" }, 1);
    voiceMode.addItemList({ "Mono", "Poly" }, 1);
    lfoDestination.addItemList({ "Off", "Pitch", "Cutoff", "PWM", "Amp" }, 1);
    dualFilterMode.addItemList({ "Off", "Serial", "Parallel" }, 1);
    waveformLabel.setText("Waveform", juce::dontSendNotification);
    voiceModeLabel.setText("Voice", juce::dontSendNotification);
    lfoDestinationLabel.setText("LFO Dest", juce::dontSendNotification);
    dualFilterModeLabel.setText("Filter Mode", juce::dontSendNotification);
    waveformLabel.attachToComponent(&waveform, false);
    voiceModeLabel.attachToComponent(&voiceMode, false);
    lfoDestinationLabel.attachToComponent(&lfoDestination, false);
    dualFilterModeLabel.attachToComponent(&dualFilterMode, false);
    addAndMakeVisible(waveform);
    addAndMakeVisible(voiceMode);
    addAndMakeVisible(lfoDestination);
    addAndMakeVisible(dualFilterMode);
    addAndMakeVisible(waveformLabel);
    addAndMakeVisible(voiceModeLabel);
    addAndMakeVisible(lfoDestinationLabel);
    addAndMakeVisible(dualFilterModeLabel);

    addKnob(osc1Tune, osc1TuneLabel, "Osc 1 Tune");
    addKnob(osc2Tune, osc2TuneLabel, "Osc 2 Tune");
    addKnob(osc3Tune, osc3TuneLabel, "Osc 3 Tune");
    addKnob(osc1Level, osc1LevelLabel, "Osc 1 Level");
    addKnob(osc2Level, osc2LevelLabel, "Osc 2 Level");
    addKnob(osc3Level, osc3LevelLabel, "Osc 3 Level");
    addKnob(noiseLevel, noiseLevelLabel, "Noise");
    addKnob(pulseWidth, pulseWidthLabel, "Pulse Width");
    addKnob(mixerDrive, mixerDriveLabel, "Drive");
    addKnob(analogDrift, analogDriftLabel, "Drift");
    addKnob(driftRate, driftRateLabel, "Drift Rate");
    addKnob(filterCutoff, filterCutoffLabel, "Cutoff");
    addKnob(filterResonance, filterResonanceLabel, "Resonance");
    addKnob(filterEnvAmount, filterEnvAmountLabel, "Filter Env");
    addKnob(filterDrive, filterDriveLabel, "Filter Drive");
    addKnob(filterAttack, filterAttackLabel, "F Attack");
    addKnob(filterDecay, filterDecayLabel, "F Decay");
    addKnob(filterSustain, filterSustainLabel, "F Sustain");
    addKnob(filterRelease, filterReleaseLabel, "F Release");
    addKnob(ampAttack, ampAttackLabel, "A Attack");
    addKnob(ampDecay, ampDecayLabel, "A Decay");
    addKnob(ampSustain, ampSustainLabel, "A Sustain");
    addKnob(ampRelease, ampReleaseLabel, "A Release");
    addKnob(lfoRate, lfoRateLabel, "LFO Rate");
    addKnob(lfoDepth, lfoDepthLabel, "LFO Depth");
    addKnob(glideTime, glideTimeLabel, "Glide");
    addKnob(chorusMix, chorusMixLabel, "Chorus");
    addKnob(delayMix, delayMixLabel, "Delay");
    addKnob(reverbMix, reverbMixLabel, "Reverb");
    addKnob(outputGain, outputGainLabel, "Output");
}

void SynthAudioProcessorEditor::addKnob(juce::Slider& slider, juce::Label& label, const juce::String& text)
{
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 72, 20);
    slider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xffd4a24c));
    slider.setColour(juce::Slider::thumbColourId, juce::Colour(0xfff0d090));
    label.setText(text, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(slider);
    addAndMakeVisible(label);
}

void SynthAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff151515));
    g.setColour(juce::Colour(0xffeeeeee));
    g.setFont(juce::FontOptions(28.0f, juce::Font::bold));
    g.drawText("Synth", 24, 16, getWidth() - 48, 36, juce::Justification::centredLeft);

    g.setColour(juce::Colour(0xffd4a24c));
    g.setFont(juce::FontOptions(15.0f, juce::Font::bold));
    g.drawText("OSCILLATORS", 24, 72, 220, 24, juce::Justification::centredLeft);
    g.drawText("MIXER", 360, 72, 160, 24, juce::Justification::centredLeft);
    g.drawText("FILTER", 560, 72, 180, 24, juce::Justification::centredLeft);
    g.drawText("ENVELOPES", 24, 292, 220, 24, juce::Justification::centredLeft);
    g.drawText("MODULATION", 800, 72, 180, 24, juce::Justification::centredLeft);
    g.drawText("OUTPUT / FX", 800, 292, 180, 24, juce::Justification::centredLeft);
}

void SynthAudioProcessorEditor::resized()
{
    waveform.setBounds(24, 108, 136, 28);
    waveformLabel.setBounds(24, 84, 136, 20);
    voiceMode.setBounds(176, 108, 120, 28);
    voiceModeLabel.setBounds(176, 84, 120, 20);
    lfoDestination.setBounds(800, 108, 120, 28);
    lfoDestinationLabel.setBounds(800, 84, 120, 20);
    dualFilterMode.setBounds(624, 108, 120, 28);
    dualFilterModeLabel.setBounds(624, 84, 120, 20);

    constexpr auto knob = 86;
    auto place = [knob](juce::Slider& slider, juce::Label& label, int x, int y)
    {
        label.setBounds(x, y, knob, 20);
        slider.setBounds(x, y + 22, knob, knob);
    };

    place(osc1Tune, osc1TuneLabel, 24, 150);
    place(osc2Tune, osc2TuneLabel, 120, 150);
    place(osc3Tune, osc3TuneLabel, 216, 150);
    place(osc1Level, osc1LevelLabel, 360, 100);
    place(osc2Level, osc2LevelLabel, 456, 100);
    place(osc3Level, osc3LevelLabel, 360, 212);
    place(noiseLevel, noiseLevelLabel, 456, 212);
    place(pulseWidth, pulseWidthLabel, 216, 38);
    place(mixerDrive, mixerDriveLabel, 552, 212);
    place(analogDrift, analogDriftLabel, 120, 38);
    place(driftRate, driftRateLabel, 312, 38);

    filterCutoffLabel.setBounds(574, 96, 132, 20);
    filterCutoff.setBounds(548, 118, 184, 184);
    place(filterResonance, filterResonanceLabel, 742, 100);
    place(filterEnvAmount, filterEnvAmountLabel, 648, 212);
    place(filterDrive, filterDriveLabel, 742, 212);

    place(lfoRate, lfoRateLabel, 800, 150);
    place(lfoDepth, lfoDepthLabel, 896, 150);
    place(glideTime, glideTimeLabel, 992, 150);

    place(filterAttack, filterAttackLabel, 24, 322);
    place(filterDecay, filterDecayLabel, 120, 322);
    place(filterSustain, filterSustainLabel, 216, 322);
    place(filterRelease, filterReleaseLabel, 312, 322);
    place(ampAttack, ampAttackLabel, 432, 322);
    place(ampDecay, ampDecayLabel, 528, 322);
    place(ampSustain, ampSustainLabel, 624, 322);
    place(ampRelease, ampReleaseLabel, 720, 322);
    place(chorusMix, chorusMixLabel, 800, 322);
    place(delayMix, delayMixLabel, 896, 322);
    place(reverbMix, reverbMixLabel, 992, 322);
    place(outputGain, outputGainLabel, 992, 454);
}
