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
      pianoKeyboard(processor.getKeyboardState(), juce::MidiKeyboardComponent::horizontalKeyboard),
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

    pianoKeyboard.setAvailableRange(24, 96);
    pianoKeyboard.setLowestVisibleKey(36);
    pianoKeyboard.setKeyWidth(20.0f);
    pianoKeyboard.setScrollButtonsVisible(true);
    pianoKeyboard.setVelocity(0.85f, true);
    pianoKeyboard.setMidiChannel(1);
    pianoKeyboard.setWantsKeyboardFocus(true);
    pianoKeyboard.setMouseClickGrabsKeyboardFocus(true);
    pianoKeyboard.setColour(juce::MidiKeyboardComponent::whiteNoteColourId, juce::Colour(0xffe7d8bd));
    pianoKeyboard.setColour(juce::MidiKeyboardComponent::blackNoteColourId, juce::Colour(0xff080806));
    pianoKeyboard.setColour(juce::MidiKeyboardComponent::keySeparatorLineColourId, juce::Colour(0xff2e2518));
    pianoKeyboard.setColour(juce::MidiKeyboardComponent::mouseOverKeyOverlayColourId, amber.withAlpha(0.18f));
    pianoKeyboard.setColour(juce::MidiKeyboardComponent::keyDownOverlayColourId, amber.withAlpha(0.42f));
    pianoKeyboard.setColour(juce::MidiKeyboardComponent::textLabelColourId, juce::Colour(0xff3c2a14));
    pianoKeyboard.setColour(juce::MidiKeyboardComponent::shadowColourId, juce::Colour(0xcc000000));
    addAndMakeVisible(pianoKeyboard);

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
    refocusPianoKeyboard();
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
    slider.setWantsKeyboardFocus(false);
    slider.setMouseClickGrabsKeyboardFocus(false);
    slider.onDragEnd = [this] { refocusPianoKeyboard(); };
    slider.onValueChange = [this] { refocusPianoKeyboard(); };
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
    slider.setWantsKeyboardFocus(false);
    slider.setMouseClickGrabsKeyboardFocus(false);
    slider.onDragEnd = [this] { refocusPianoKeyboard(); };
    slider.onValueChange = [this] { refocusPianoKeyboard(); };
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
    combo.setWantsKeyboardFocus(false);
    combo.setMouseClickGrabsKeyboardFocus(false);
    combo.onChange = [this] { refocusPianoKeyboard(); };
    addAndMakeVisible(combo);
    addAndMakeVisible(label);
}

void SynthAudioProcessorEditor::refocusPianoKeyboard()
{
    if (pianoKeyboard.isShowing())
        pianoKeyboard.grabKeyboardFocus();
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

    auto top = face.removeFromTop(72).reduced(10, 8);
    g.setGradientFill(juce::ColourGradient(juce::Colour(0xff25241f), static_cast<float>(top.getX()), static_cast<float>(top.getY()),
                                           juce::Colour(0xff0e0e0c), static_cast<float>(top.getX()), static_cast<float>(top.getBottom()), false));
    g.fillRoundedRectangle(top.toFloat(), 4.0f);
    g.setColour(outline.withAlpha(0.45f));
    g.drawRoundedRectangle(top.toFloat(), 4.0f, 1.0f);

    auto drawHardwareButton = [&g](juce::Rectangle<int> button, const juce::String& text)
    {
        g.setGradientFill(juce::ColourGradient(juce::Colour(0xff2b2924), static_cast<float>(button.getX()), static_cast<float>(button.getY()),
                                               juce::Colour(0xff0c0c0b), static_cast<float>(button.getX()), static_cast<float>(button.getBottom()), false));
        g.fillRoundedRectangle(button.toFloat(), 3.0f);
        g.setColour(juce::Colour(0xff050505));
        g.drawRoundedRectangle(button.toFloat(), 3.0f, 1.5f);
        g.setColour(outline.withAlpha(0.45f));
        g.drawRoundedRectangle(button.reduced(2).toFloat(), 2.0f, 1.0f);
        g.setColour(text == "8" ? amber : cream);
        g.setFont(juce::FontOptions(14.0f, juce::Font::bold));
        g.drawText(text, button, juce::Justification::centred);
    };

    auto drawToggle = [&g](int x, int y, bool active, const juce::String& label)
    {
        drawLed(g, { static_cast<float>(x), static_cast<float>(y + 6) });
        auto toggle = juce::Rectangle<int>(x - 10, y + 18, 20, 26);
        g.setColour(juce::Colour(0xff050505));
        g.fillRoundedRectangle(toggle.toFloat(), 5.0f);
        g.setGradientFill(juce::ColourGradient(active ? juce::Colour(0xff3d3529) : juce::Colour(0xff141412),
                                               static_cast<float>(toggle.getX()), static_cast<float>(toggle.getY()),
                                               juce::Colour(0xff070707), static_cast<float>(toggle.getX()), static_cast<float>(toggle.getBottom()), false));
        g.fillRoundedRectangle(toggle.reduced(3).toFloat(), 4.0f);
        g.setColour(cream);
        g.setFont(juce::FontOptions(11.0f, juce::Font::bold));
        g.drawText(label, x - 34, y + 48, 68, 16, juce::Justification::centred);
    };

    g.setColour(mutedCream);
    g.setFont(juce::FontOptions(25.0f, juce::Font::plain));
    g.drawText("SYNTH", top.withTrimmedLeft(52).removeFromLeft(145), juce::Justification::centredLeft);

    g.setColour(mutedCream.withAlpha(0.9f));
    for (auto i = 0; i < 3; ++i)
        g.fillRect(top.getX() + 34, top.getY() + 13 + i * 8, 20, 3);

    drawHardwareButton({ top.getX() + 205, top.getY() + 8, 34, 36 }, "<");
    drawHardwareButton({ top.getX() + 242, top.getY() + 8, 34, 36 }, ">");

    auto presetDisplay = juce::Rectangle<int>(top.getX() + 286, top.getY() + 7, 320, 38);
    g.setColour(juce::Colour(0xff080807));
    g.fillRoundedRectangle(presetDisplay.toFloat(), 3.0f);
    g.setColour(outline.withAlpha(0.25f));
    g.drawRoundedRectangle(presetDisplay.toFloat(), 3.0f, 1.0f);
    g.setColour(amber);
    g.setFont(juce::FontOptions(18.0f, juce::Font::bold));
    g.drawText("001   CLASSIC LEAD", presetDisplay.reduced(14, 0), juce::Justification::centredLeft);

    drawHardwareButton({ top.getX() + 620, top.getY() + 8, 34, 36 }, "<");
    drawHardwareButton({ top.getX() + 657, top.getY() + 8, 34, 36 }, ">");
    drawHardwareButton({ top.getX() + 696, top.getY() + 8, 42, 36 }, "SAVE");
    drawHardwareButton({ top.getX() + 778, top.getY() + 8, 72, 36 }, "A / B");
    drawHardwareButton({ top.getX() + 856, top.getY() + 8, 72, 36 }, "COPY");
    drawHardwareButton({ top.getX() + 934, top.getY() + 8, 72, 36 }, "UNDO");
    drawHardwareButton({ top.getX() + 1012, top.getY() + 8, 72, 36 }, "REDO");
    drawHardwareButton({ top.getX() + 1090, top.getY() + 8, 44, 36 }, "*");

    auto meter = juce::Rectangle<int>(top.getRight() - 192, top.getY() + 10, 116, 32);
    g.setColour(juce::Colour(0xff090909));
    g.fillRoundedRectangle(meter.toFloat(), 3.0f);
    for (auto x = 0; x < 11; ++x)
        for (auto y = 0; y < 4; ++y)
        {
            const auto active = x < 5 && y < 3;
            g.setColour(active ? amber : juce::Colour(0xff1f241e));
            g.fillRect(meter.getX() + 9 + x * 9, meter.getY() + 7 + y * 5, 7, 3);
        }

    auto master = juce::Rectangle<int>(top.getRight() - 64, top.getY() + 4, 48, 48);
    g.setColour(cream);
    g.setFont(juce::FontOptions(8.0f, juce::Font::bold));
    g.drawText("MASTER", master.withY(master.getY() - 5), juce::Justification::centredTop);
    juce::Slider fauxMaster;
    fauxMaster.setName("Master");
    retroLookAndFeel->drawRotarySlider(g, master.getX(), master.getY() + 2, master.getWidth(), master.getHeight(), 0.72f,
                                       juce::MathConstants<float>::pi * 1.2f, juce::MathConstants<float>::pi * 2.8f, fauxMaster);

    drawSection(g, { 50, 92, 430, 330 }, "OSCILLATORS");
    drawSection(g, { 492, 92, 230, 330 }, "MIXER");
    drawSection(g, { 734, 92, 300, 330 }, "FILTER");
    drawSection(g, { 1046, 92, 194, 330 }, "ENVELOPES");
    drawSection(g, { 50, 424, 430, 170 }, "LFO");
    drawSection(g, { 492, 424, 300, 170 }, "PERFORMANCE");
    drawSection(g, { 804, 424, 240, 170 }, "OUTPUT / FX");
    drawSection(g, { 1056, 424, 184, 170 }, "VOICES");
    drawSection(g, { 50, 606, 1190, 106 }, "PIANO ROLL");

    g.setColour(outline.withAlpha(0.6f));
    g.drawLine(72.0f, 198.0f, 458.0f, 198.0f, 1.0f);
    g.drawLine(72.0f, 304.0f, 458.0f, 304.0f, 1.0f);
    g.drawLine(608.0f, 132.0f, 608.0f, 390.0f, 1.0f);
    g.drawLine(1048.0f, 254.0f, 1238.0f, 254.0f, 1.0f);
    g.drawLine(532.0f, 444.0f, 532.0f, 574.0f, 1.0f);
    g.drawLine(832.0f, 444.0f, 832.0f, 574.0f, 1.0f);

    for (auto y : { 157, 263, 369 })
    {
        auto badge = juce::Rectangle<int>(82, y, 28, 32);
        g.setColour(juce::Colour(0xff1a1711));
        g.fillRoundedRectangle(badge.toFloat(), 3.0f);
        g.setColour(outline);
        g.drawRoundedRectangle(badge.toFloat(), 3.0f, 1.0f);
        g.setColour(cream);
        g.setFont(juce::FontOptions(18.0f, juce::Font::bold));
        g.drawText(juce::String((y - 51) / 106), badge, juce::Justification::centred);
        drawLed(g, { 98.0f, static_cast<float>(y + 42) });
    }

    g.setColour(cream.withAlpha(0.75f));
    g.setFont(juce::FontOptions(11.0f, juce::Font::bold));
    for (auto y : { 165, 271, 377 })
    {
        g.drawText("TUNE", 128, y - 28, 70, 16, juce::Justification::centred);
        g.drawText("WAVE", 276, y - 28, 70, 16, juce::Justification::centred);
        g.drawText("OCTAVE", 382, y - 28, 72, 16, juce::Justification::centred);
        g.drawText("-2        +2", 382, y + 62, 74, 16, juce::Justification::centred);
    }

    g.setColour(cream.withAlpha(0.9f));
    g.setFont(juce::FontOptions(11.0f, juce::Font::bold));
    g.drawText("LP", 770, 152, 24, 16, juce::Justification::centred);
    g.drawText("HP", 810, 152, 24, 16, juce::Justification::centred);
    g.drawText("12 dB", 982, 152, 42, 16, juce::Justification::centred);
    g.drawText("24 dB", 982, 174, 42, 16, juce::Justification::centred);
    drawToggle(792, 168, true, "MODE");
    drawToggle(1004, 168, true, "SLOPE");

    drawToggle(598, 332, true, "DRIVE");
    drawToggle(580, 506, true, "VEL>AMP");
    drawToggle(648, 506, true, "VEL>FILTER");
    drawToggle(716, 506, false, "AFTERTOUCH");

    auto drawEnvGraph = [&g](juce::Rectangle<int> envGraph)
    {
        g.setColour(juce::Colour(0xff11110f));
        g.fillRoundedRectangle(envGraph.toFloat(), 3.0f);
        g.setColour(outline.withAlpha(0.35f));
        for (auto i = 1; i < 8; ++i)
            g.drawVerticalLine(envGraph.getX() + i * envGraph.getWidth() / 8, static_cast<float>(envGraph.getY()), static_cast<float>(envGraph.getBottom()));
        for (auto i = 1; i < 4; ++i)
            g.drawHorizontalLine(envGraph.getY() + i * envGraph.getHeight() / 4, static_cast<float>(envGraph.getX()), static_cast<float>(envGraph.getRight()));
        juce::Path envPath;
        envPath.startNewSubPath(static_cast<float>(envGraph.getX() + 8), static_cast<float>(envGraph.getBottom() - 16));
        envPath.lineTo(static_cast<float>(envGraph.getX() + 54), static_cast<float>(envGraph.getY() + 18));
        envPath.lineTo(static_cast<float>(envGraph.getX() + 98), static_cast<float>(envGraph.getY() + 18));
        envPath.lineTo(static_cast<float>(envGraph.getRight() - 10), static_cast<float>(envGraph.getBottom() - 16));
        g.setColour(amber);
        g.strokePath(envPath, juce::PathStrokeType(1.7f));
    };

    auto envGraph = juce::Rectangle<int>(615, 492, 144, 70);
    drawEnvGraph(envGraph);

    drawHardwareButton({ 1080, 504, 34, 40 }, "1");
    drawHardwareButton({ 1118, 504, 34, 40 }, "2");
    drawHardwareButton({ 1156, 504, 34, 40 }, "4");
    drawHardwareButton({ 1194, 504, 34, 40 }, "8");

    drawScrew(g, 52, 86);
    drawScrew(g, getWidth() - 52, 86);
    drawScrew(g, 52, getHeight() - 28);
    drawScrew(g, getWidth() - 52, getHeight() - 28);

    drawLed(g, { 67.0f, 456.0f });
    drawLed(g, { 586.0f, 456.0f });
    drawLed(g, { 1212.0f, 456.0f });
    drawLed(g, { 67.0f, 632.0f });

    g.setColour(mutedCream);
    g.setFont(juce::FontOptions(11.0f, juce::Font::bold));
    g.drawText("MOUSE PLAYABLE", 92, 623, 160, 16, juce::Justification::centredLeft);
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

    waveformLabel.setBounds(268, 244, 82, 18);
    waveform.setBounds(268, 264, 118, 26);
    place(osc1Tune, osc1TuneLabel, 122, 134, 86, 94);
    place(pulseWidth, pulseWidthLabel, 268, 134, 86, 94);
    place(analogDrift, analogDriftLabel, 386, 134, 70, 82);
    place(osc2Tune, osc2TuneLabel, 122, 240, 86, 94);
    place(driftRate, driftRateLabel, 386, 240, 70, 82);
    place(osc3Tune, osc3TuneLabel, 122, 342, 86, 82);

    place(osc1Level, osc1LevelLabel, 516, 128, 76, 82);
    place(osc2Level, osc2LevelLabel, 516, 222, 76, 82);
    place(osc3Level, osc3LevelLabel, 516, 316, 76, 82);
    place(noiseLevel, noiseLevelLabel, 626, 128, 76, 82);
    place(mixerDrive, mixerDriveLabel, 626, 274, 76, 92);

    dualFilterModeLabel.setBounds(760, 136, 90, 18);
    dualFilterMode.setBounds(760, 156, 132, 26);
    placeBig(filterCutoff, filterCutoffLabel, 794, 190, 166, 166);
    place(filterResonance, filterResonanceLabel, 760, 308, 78, 90);
    place(filterEnvAmount, filterEnvAmountLabel, 936, 308, 78, 90);
    place(filterDrive, filterDriveLabel, 916, 136, 78, 90);

    placeEnv(filterAttack, filterAttackLabel, 1064, 142);
    placeEnv(filterDecay, filterDecayLabel, 1108, 142);
    placeEnv(filterSustain, filterSustainLabel, 1152, 142);
    placeEnv(filterRelease, filterReleaseLabel, 1196, 142);
    placeEnv(ampAttack, ampAttackLabel, 1064, 274);
    placeEnv(ampDecay, ampDecayLabel, 1108, 274);
    placeEnv(ampSustain, ampSustainLabel, 1152, 274);
    placeEnv(ampRelease, ampReleaseLabel, 1196, 274);

    lfoDestinationLabel.setBounds(74, 462, 92, 18);
    lfoDestination.setBounds(74, 482, 122, 26);
    place(lfoRate, lfoRateLabel, 224, 462, 82, 94);
    place(lfoDepth, lfoDepthLabel, 320, 462, 82, 94);
    place(glideTime, glideTimeLabel, 414, 462, 70, 86);

    voiceModeLabel.setBounds(512, 462, 78, 18);
    voiceMode.setBounds(512, 482, 118, 26);
    place(chorusMix, chorusMixLabel, 670, 462, 76, 90);

    place(delayMix, delayMixLabel, 850, 462, 76, 90);
    place(reverbMix, reverbMixLabel, 932, 462, 76, 90);
    place(outputGain, outputGainLabel, 970, 462, 78, 96);

    pianoKeyboard.setBounds(72, 640, 1146, 58);
}
