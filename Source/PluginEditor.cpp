#include "PluginEditor.h"

namespace
{
constexpr int editorWidth = 1280;
constexpr int editorHeight = 720;
constexpr int woodWidth = 38;

const auto panel = juce::Colour(0xff151512);
const auto panelDark = juce::Colour(0xff0f0f0d);
const auto panelLight = juce::Colour(0xff25231e);
const auto cream = juce::Colour(0xfff0d7a0);
const auto mutedCream = juce::Colour(0xffbda982);
const auto amber = juce::Colour(0xffffb23d);
const auto amberDim = juce::Colour(0xff78511e);
const auto outline = juce::Colour(0xff8d6a3a);
const auto shadow = juce::Colour(0xaa000000);
const auto textBox = juce::Colour(0xff171a18);

class RetroLookAndFeel final : public juce::LookAndFeel_V4
{
public:
    RetroLookAndFeel()
    {
        setColour(juce::Label::textColourId, cream);
        setColour(juce::Label::backgroundColourId, juce::Colours::transparentBlack);
        setColour(juce::Slider::textBoxTextColourId, cream);
        setColour(juce::Slider::textBoxBackgroundColourId, textBox);
        setColour(juce::Slider::textBoxOutlineColourId, outline.withAlpha(0.55f));
        setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xff1b1b18));
        setColour(juce::ComboBox::textColourId, cream);
        setColour(juce::ComboBox::outlineColourId, outline);
        setColour(juce::ComboBox::arrowColourId, amber);
        setColour(juce::PopupMenu::backgroundColourId, juce::Colour(0xff181815));
        setColour(juce::PopupMenu::textColourId, cream);
        setColour(juce::PopupMenu::highlightedBackgroundColourId, amberDim);
        setColour(juce::PopupMenu::highlightedTextColourId, cream);
    }

    juce::Font getLabelFont(juce::Label&) override
    {
        return juce::FontOptions(12.0f, juce::Font::bold);
    }

    void drawComboBox(juce::Graphics& g,
                      int width,
                      int height,
                      bool,
                      int,
                      int,
                      int,
                      int,
                      juce::ComboBox&) override
    {
        auto bounds = juce::Rectangle<float>(0.5f, 0.5f, static_cast<float>(width) - 1.0f, static_cast<float>(height) - 1.0f);
        g.setGradientFill(juce::ColourGradient(juce::Colour(0xff26241f), 0.0f, 0.0f,
                                               juce::Colour(0xff11110f), 0.0f, static_cast<float>(height), false));
        g.fillRoundedRectangle(bounds, 3.0f);
        g.setColour(outline.withAlpha(0.85f));
        g.drawRoundedRectangle(bounds, 3.0f, 1.0f);

        juce::Path arrow;
        const auto cx = static_cast<float>(width - 18);
        const auto cy = static_cast<float>(height / 2);
        arrow.addTriangle(cx - 5.0f, cy - 2.0f, cx + 5.0f, cy - 2.0f, cx, cy + 5.0f);
        g.setColour(amber);
        g.fillPath(arrow);
    }

    void positionComboBoxText(juce::ComboBox& box, juce::Label& label) override
    {
        label.setBounds(9, 1, box.getWidth() - 28, box.getHeight() - 2);
        label.setFont(juce::FontOptions(13.0f, juce::Font::bold));
        label.setJustificationType(juce::Justification::centredLeft);
    }

    void drawRotarySlider(juce::Graphics& g,
                          int x,
                          int y,
                          int width,
                          int height,
                          float sliderPos,
                          float rotaryStartAngle,
                          float rotaryEndAngle,
                          juce::Slider& slider) override
    {
        auto bounds = juce::Rectangle<float>(static_cast<float>(x), static_cast<float>(y),
                                             static_cast<float>(width), static_cast<float>(height)).reduced(8.0f);
        const auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.5f;
        const auto centre = bounds.getCentre();
        const auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
        const auto arcRadius = radius + 5.0f;

        g.setColour(shadow);
        g.fillEllipse(bounds.translated(3.0f, 5.0f));

        juce::Path inactiveArc;
        inactiveArc.addCentredArc(centre.x, centre.y, arcRadius, arcRadius, 0.0f,
                                  rotaryStartAngle, rotaryEndAngle, true);
        g.setColour(juce::Colour(0xff263039));
        g.strokePath(inactiveArc, juce::PathStrokeType(5.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

        juce::Path activeArc;
        activeArc.addCentredArc(centre.x, centre.y, arcRadius, arcRadius, 0.0f,
                                rotaryStartAngle, angle, true);
        g.setColour(amber);
        g.strokePath(activeArc, juce::PathStrokeType(5.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

        for (auto i = 0; i <= 18; ++i)
        {
            const auto tickAngle = rotaryStartAngle + (rotaryEndAngle - rotaryStartAngle) * static_cast<float>(i) / 18.0f;
            const auto inner = juce::Point<float>(centre.x + std::cos(tickAngle) * (arcRadius + 5.0f),
                                                  centre.y + std::sin(tickAngle) * (arcRadius + 5.0f));
            const auto outer = juce::Point<float>(centre.x + std::cos(tickAngle) * (arcRadius + (i % 3 == 0 ? 12.0f : 9.0f)),
                                                  centre.y + std::sin(tickAngle) * (arcRadius + (i % 3 == 0 ? 12.0f : 9.0f)));
            g.setColour((i % 3 == 0 ? cream : mutedCream).withAlpha(0.8f));
            g.drawLine({ inner, outer }, i % 3 == 0 ? 1.2f : 0.8f);
        }

        juce::ColourGradient knobGradient(juce::Colour(0xff34332d), centre.x - radius, centre.y - radius,
                                          juce::Colour(0xff070707), centre.x + radius, centre.y + radius, false);
        knobGradient.addColour(0.45, juce::Colour(0xff171713));
        g.setGradientFill(knobGradient);
        g.fillEllipse(bounds);
        g.setColour(juce::Colour(0xff070707));
        g.drawEllipse(bounds, 2.0f);
        g.setColour(juce::Colour(0xff545044).withAlpha(0.55f));
        g.drawEllipse(bounds.reduced(4.0f), 1.0f);

        const auto pointerLength = radius * 0.72f;
        juce::Path pointer;
        pointer.addRoundedRectangle(-2.0f, -pointerLength, 4.0f, pointerLength * 0.75f, 2.0f);
        pointer.applyTransform(juce::AffineTransform::rotation(angle).translated(centre.x, centre.y));
        g.setColour(cream);
        g.fillPath(pointer);

        if (slider.getWidth() > 130 || slider.getName() == "Cutoff")
        {
            g.setColour(juce::Colour(0x33ffffff));
            g.fillEllipse(bounds.reduced(radius * 0.58f).translated(-radius * 0.2f, -radius * 0.22f));
        }
    }

    void drawLinearSlider(juce::Graphics& g,
                          int x,
                          int y,
                          int width,
                          int height,
                          float sliderPos,
                          float,
                          float,
                          const juce::Slider::SliderStyle,
                          juce::Slider&) override
    {
        auto track = juce::Rectangle<float>(static_cast<float>(x + width / 2 - 3), static_cast<float>(y + 8),
                                            6.0f, static_cast<float>(height - 16));
        g.setColour(juce::Colour(0xff050505));
        g.fillRoundedRectangle(track, 2.0f);
        g.setColour(outline.withAlpha(0.55f));
        g.drawRoundedRectangle(track, 2.0f, 1.0f);

        for (auto i = 0; i <= 8; ++i)
        {
            const auto ty = track.getY() + track.getHeight() * static_cast<float>(i) / 8.0f;
            const auto longTick = i % 2 == 0;
            g.setColour((longTick ? cream : mutedCream).withAlpha(0.65f));
            g.drawLine(track.getCentreX() - (longTick ? 18.0f : 12.0f), ty,
                       track.getCentreX() + (longTick ? 18.0f : 12.0f), ty, 0.9f);
        }

        auto thumb = juce::Rectangle<float>(static_cast<float>(x + 8), sliderPos - 7.0f,
                                            static_cast<float>(width - 16), 14.0f);
        g.setColour(shadow);
        g.fillRoundedRectangle(thumb.translated(2.0f, 2.0f), 2.0f);
        g.setGradientFill(juce::ColourGradient(juce::Colour(0xff403a2f), thumb.getX(), thumb.getY(),
                                               juce::Colour(0xff11100e), thumb.getX(), thumb.getBottom(), false));
        g.fillRoundedRectangle(thumb, 2.0f);
        g.setColour(outline);
        g.drawRoundedRectangle(thumb, 2.0f, 1.0f);
    }

    void drawLabel(juce::Graphics& g, juce::Label& label) override
    {
        g.setColour(label.findColour(juce::Label::textColourId));
        g.setFont(label.getFont());
        g.drawFittedText(label.getText().toUpperCase(), label.getLocalBounds(), label.getJustificationType(), 1);
    }

    juce::Font getTextButtonFont(juce::TextButton&, int) override
    {
        return juce::FontOptions(13.0f, juce::Font::bold);
    }
};

void drawLed(juce::Graphics& g, juce::Point<float> centre)
{
    g.setColour(juce::Colour(0xff160700));
    g.fillEllipse(centre.x - 5.0f, centre.y - 5.0f, 10.0f, 10.0f);
    g.setColour(juce::Colour(0xffff5d1b));
    g.fillEllipse(centre.x - 3.5f, centre.y - 3.5f, 7.0f, 7.0f);
    g.setColour(juce::Colour(0x88ffb24d));
    g.drawEllipse(centre.x - 6.0f, centre.y - 6.0f, 12.0f, 12.0f, 1.0f);
}
}

SynthAudioProcessorEditor::SynthAudioProcessorEditor(SynthAudioProcessor& p)
    : AudioProcessorEditor(&p),
      processor(p),
      retroLookAndFeel(std::make_unique<RetroLookAndFeel>()),
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
    setSize(editorWidth, editorHeight);
    setLookAndFeel(retroLookAndFeel.get());

    waveform.addItemList({ "Sine", "Saw", "Pulse", "Triangle" }, 1);
    voiceMode.addItemList({ "Mono", "Poly" }, 1);
    lfoDestination.addItemList({ "Off", "Pitch", "Cutoff", "PWM", "Amp" }, 1);
    dualFilterMode.addItemList({ "Off", "Serial", "Parallel" }, 1);

    addCombo(waveform, waveformLabel, "Wave");
    addCombo(voiceMode, voiceModeLabel, "Voice");
    addCombo(lfoDestination, lfoDestinationLabel, "LFO Dest");
    addCombo(dualFilterMode, dualFilterModeLabel, "Mode");

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
    addKnob(filterEnvAmount, filterEnvAmountLabel, "Env Amount");
    addKnob(filterDrive, filterDriveLabel, "Filter Drive");
    addEnvelopeSlider(filterAttack, filterAttackLabel, "F A");
    addEnvelopeSlider(filterDecay, filterDecayLabel, "F D");
    addEnvelopeSlider(filterSustain, filterSustainLabel, "F S");
    addEnvelopeSlider(filterRelease, filterReleaseLabel, "F R");
    addEnvelopeSlider(ampAttack, ampAttackLabel, "A A");
    addEnvelopeSlider(ampDecay, ampDecayLabel, "A D");
    addEnvelopeSlider(ampSustain, ampSustainLabel, "A S");
    addEnvelopeSlider(ampRelease, ampReleaseLabel, "A R");
    addKnob(lfoRate, lfoRateLabel, "LFO Rate");
    addKnob(lfoDepth, lfoDepthLabel, "LFO Depth");
    addKnob(glideTime, glideTimeLabel, "Glide");
    addKnob(chorusMix, chorusMixLabel, "Chorus");
    addKnob(delayMix, delayMixLabel, "Delay");
    addKnob(reverbMix, reverbMixLabel, "Reverb");
    addKnob(outputGain, outputGainLabel, "Output");

    filterCutoff.setName("Cutoff");
    filterCutoff.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 84, 18);
}

SynthAudioProcessorEditor::~SynthAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

void SynthAudioProcessorEditor::addKnob(juce::Slider& slider, juce::Label& label, const juce::String& text)
{
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 66, 18);
    slider.setNumDecimalPlacesToDisplay(2);
    slider.setPopupDisplayEnabled(true, false, this);
    slider.setColour(juce::Slider::textBoxTextColourId, cream);
    slider.setColour(juce::Slider::textBoxBackgroundColourId, textBox);
    slider.setColour(juce::Slider::textBoxOutlineColourId, outline.withAlpha(0.55f));

    label.setText(text, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    label.setColour(juce::Label::textColourId, cream);
    addAndMakeVisible(slider);
    addAndMakeVisible(label);
}

void SynthAudioProcessorEditor::addEnvelopeSlider(juce::Slider& slider, juce::Label& label, const juce::String& text)
{
    slider.setSliderStyle(juce::Slider::LinearVertical);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 18);
    slider.setNumDecimalPlacesToDisplay(2);
    slider.setPopupDisplayEnabled(true, false, this);
    slider.setColour(juce::Slider::textBoxTextColourId, cream);
    slider.setColour(juce::Slider::textBoxBackgroundColourId, textBox);
    slider.setColour(juce::Slider::textBoxOutlineColourId, outline.withAlpha(0.55f));

    label.setText(text, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    label.setColour(juce::Label::textColourId, cream);
    addAndMakeVisible(slider);
    addAndMakeVisible(label);
}

void SynthAudioProcessorEditor::addCombo(juce::ComboBox& combo, juce::Label& label, const juce::String& text)
{
    label.setText(text, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centredLeft);
    label.setColour(juce::Label::textColourId, cream);
    addAndMakeVisible(combo);
    addAndMakeVisible(label);
}

void SynthAudioProcessorEditor::drawSection(juce::Graphics& g, juce::Rectangle<int> bounds, const juce::String& title)
{
    auto b = bounds.toFloat();
    g.setColour(shadow);
    g.fillRoundedRectangle(b.translated(0.0f, 2.0f), 5.0f);

    g.setGradientFill(juce::ColourGradient(panelLight, b.getX(), b.getY(),
                                           panelDark, b.getX(), b.getBottom(), false));
    g.fillRoundedRectangle(b, 5.0f);
    g.setColour(outline.withAlpha(0.8f));
    g.drawRoundedRectangle(b, 5.0f, 1.0f);
    g.setColour(juce::Colour(0x55000000));
    g.drawRoundedRectangle(b.reduced(3.0f), 3.0f, 1.0f);

    g.setColour(amber);
    g.setFont(juce::FontOptions(17.0f, juce::Font::bold));
    g.drawText(title, bounds.removeFromTop(32), juce::Justification::centred);
}

void SynthAudioProcessorEditor::drawScrew(juce::Graphics& g, int x, int y)
{
    auto b = juce::Rectangle<float>(static_cast<float>(x - 6), static_cast<float>(y - 6), 12.0f, 12.0f);
    g.setColour(juce::Colour(0xff050505));
    g.fillEllipse(b);
    g.setGradientFill(juce::ColourGradient(juce::Colour(0xff504a3f), b.getX(), b.getY(),
                                           juce::Colour(0xff050505), b.getRight(), b.getBottom(), false));
    g.fillEllipse(b.reduced(2.0f));
    g.setColour(juce::Colour(0xff080808));
    g.drawLine(b.getX() + 3.0f, b.getCentreY(), b.getRight() - 3.0f, b.getCentreY(), 1.2f);
}

void SynthAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff070707));

    auto drawWood = [&g](juce::Rectangle<int> area)
    {
        auto b = area.toFloat();
        juce::ColourGradient grad(juce::Colour(0xff7b4a28), b.getX(), b.getY(),
                                  juce::Colour(0xff2a1308), b.getRight(), b.getBottom(), false);
        grad.addColour(0.35, juce::Colour(0xff9a6338));
        grad.addColour(0.75, juce::Colour(0xff3e1f10));
        g.setGradientFill(grad);
        g.fillRoundedRectangle(b, 9.0f);
        g.setColour(juce::Colour(0x55210d04));
        for (auto i = 0; i < 9; ++i)
        {
            const auto x = b.getX() + 5.0f + static_cast<float>((i * 7) % juce::jmax(1, area.getWidth() - 10));
            g.drawLine(x, b.getY() + 10.0f, x + std::sin(static_cast<float>(i)) * 5.0f, b.getBottom() - 10.0f, 1.0f);
        }
        g.setColour(juce::Colour(0xff140805));
        g.drawRoundedRectangle(b.reduced(1.0f), 8.0f, 2.0f);
    };

    drawWood({ 0, 0, woodWidth, getHeight() });
    drawWood({ getWidth() - woodWidth, 0, woodWidth, getHeight() });

    auto face = juce::Rectangle<int>(woodWidth, 8, getWidth() - woodWidth * 2, getHeight() - 16);
    g.setGradientFill(juce::ColourGradient(juce::Colour(0xff20201c), static_cast<float>(face.getX()), static_cast<float>(face.getY()),
                                           juce::Colour(0xff0b0b0a), static_cast<float>(face.getX()), static_cast<float>(face.getBottom()), false));
    g.fillRoundedRectangle(face.toFloat(), 7.0f);
    g.setColour(juce::Colour(0xff050505));
    g.drawRoundedRectangle(face.toFloat(), 7.0f, 2.0f);

    auto top = face.removeFromTop(70).reduced(10, 8);
    g.setGradientFill(juce::ColourGradient(juce::Colour(0xff25241f), static_cast<float>(top.getX()), static_cast<float>(top.getY()),
                                           juce::Colour(0xff0e0e0c), static_cast<float>(top.getX()), static_cast<float>(top.getBottom()), false));
    g.fillRoundedRectangle(top.toFloat(), 4.0f);
    g.setColour(outline.withAlpha(0.45f));
    g.drawRoundedRectangle(top.toFloat(), 4.0f, 1.0f);

    g.setColour(mutedCream);
    g.setFont(juce::FontOptions(25.0f, juce::Font::plain));
    g.drawText("SYNTH", top.withTrimmedLeft(52).removeFromLeft(160), juce::Justification::centredLeft);
    g.setColour(amber);
    g.setFont(juce::FontOptions(16.0f, juce::Font::bold));
    g.drawText("001   CLASSIC LEAD", top.withTrimmedLeft(290).removeFromLeft(320), juce::Justification::centredLeft);

    for (auto i = 0; i < 3; ++i)
    {
        auto button = juce::Rectangle<int>(top.getX() + 215 + i * 34, top.getY() + 8, 28, 32);
        g.setGradientFill(juce::ColourGradient(panelLight, static_cast<float>(button.getX()), static_cast<float>(button.getY()),
                                               panelDark, static_cast<float>(button.getX()), static_cast<float>(button.getBottom()), false));
        g.fillRoundedRectangle(button.toFloat(), 3.0f);
        g.setColour(outline.withAlpha(0.45f));
        g.drawRoundedRectangle(button.toFloat(), 3.0f, 1.0f);
    }

    auto meter = juce::Rectangle<int>(top.getRight() - 205, top.getY() + 12, 126, 30);
    g.setColour(juce::Colour(0xff090909));
    g.fillRoundedRectangle(meter.toFloat(), 3.0f);
    for (auto x = 0; x < 10; ++x)
        for (auto y = 0; y < 4; ++y)
        {
            const auto active = x < 4 && y < 3;
            g.setColour(active ? amber : juce::Colour(0xff1f241e));
            g.fillRect(meter.getX() + 10 + x * 10, meter.getY() + 7 + y * 5, 8, 3);
        }

    drawSection(g, { 50, 92, 380, 286 }, "OSCILLATORS");
    drawSection(g, { 442, 92, 214, 286 }, "MIXER");
    drawSection(g, { 668, 92, 300, 286 }, "FILTER");
    drawSection(g, { 980, 92, 260, 286 }, "ENVELOPES");
    drawSection(g, { 50, 392, 520, 150 }, "MODULATION");
    drawSection(g, { 584, 392, 300, 150 }, "PERFORMANCE");
    drawSection(g, { 898, 392, 342, 150 }, "OUTPUT / FX");

    drawScrew(g, 52, 86);
    drawScrew(g, getWidth() - 52, 86);
    drawScrew(g, 52, getHeight() - 28);
    drawScrew(g, getWidth() - 52, getHeight() - 28);

    drawLed(g, { 67.0f, 418.0f });
    drawLed(g, { 586.0f, 418.0f });
    drawLed(g, { 1212.0f, 418.0f });
}

void SynthAudioProcessorEditor::resized()
{
    auto place = [](juce::Slider& slider, juce::Label& label, int x, int y, int w = 72, int h = 86)
    {
        label.setBounds(x - 8, y, w + 16, 18);
        slider.setBounds(x, y + 18, w, h);
    };

    auto placeBig = [](juce::Slider& slider, juce::Label& label, int x, int y, int w, int h)
    {
        label.setBounds(x, y, w, 20);
        slider.setBounds(x, y + 18, w, h);
    };

    auto placeEnv = [](juce::Slider& slider, juce::Label& label, int x, int y)
    {
        label.setBounds(x, y, 46, 16);
        slider.setBounds(x - 2, y + 18, 50, 96);
    };

    waveformLabel.setBounds(72, 132, 80, 18);
    waveform.setBounds(72, 152, 132, 26);
    place(analogDrift, analogDriftLabel, 226, 122);
    place(pulseWidth, pulseWidthLabel, 314, 122);
    place(driftRate, driftRateLabel, 226, 232);
    place(osc1Tune, osc1TuneLabel, 78, 220);
    place(osc2Tune, osc2TuneLabel, 168, 220);
    place(osc3Tune, osc3TuneLabel, 316, 220);

    place(osc1Level, osc1LevelLabel, 464, 124);
    place(osc2Level, osc2LevelLabel, 464, 226);
    place(osc3Level, osc3LevelLabel, 552, 124);
    place(noiseLevel, noiseLevelLabel, 552, 226);
    place(mixerDrive, mixerDriveLabel, 506, 302);

    dualFilterModeLabel.setBounds(690, 132, 90, 18);
    dualFilterMode.setBounds(690, 152, 130, 26);
    placeBig(filterCutoff, filterCutoffLabel, 738, 182, 170, 170);
    place(filterResonance, filterResonanceLabel, 690, 286);
    place(filterEnvAmount, filterEnvAmountLabel, 874, 286);
    place(filterDrive, filterDriveLabel, 874, 132);

    placeEnv(filterAttack, filterAttackLabel, 1000, 142);
    placeEnv(filterDecay, filterDecayLabel, 1056, 142);
    placeEnv(filterSustain, filterSustainLabel, 1112, 142);
    placeEnv(filterRelease, filterReleaseLabel, 1168, 142);
    placeEnv(ampAttack, ampAttackLabel, 1000, 266);
    placeEnv(ampDecay, ampDecayLabel, 1056, 266);
    placeEnv(ampSustain, ampSustainLabel, 1112, 266);
    placeEnv(ampRelease, ampReleaseLabel, 1168, 266);

    lfoDestinationLabel.setBounds(74, 428, 92, 18);
    lfoDestination.setBounds(74, 448, 122, 26);
    place(lfoRate, lfoRateLabel, 218, 432);
    place(lfoDepth, lfoDepthLabel, 306, 432);
    place(glideTime, glideTimeLabel, 412, 432);

    voiceModeLabel.setBounds(612, 432, 70, 18);
    voiceMode.setBounds(612, 452, 118, 26);
    place(chorusMix, chorusMixLabel, 758, 432);

    place(delayMix, delayMixLabel, 930, 432);
    place(reverbMix, reverbMixLabel, 1028, 432);
    place(outputGain, outputGainLabel, 1132, 424, 78, 96);
}
