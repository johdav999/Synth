#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_dsp/juce_dsp.h>

class SynthAudioProcessor final : public juce::AudioProcessor,
                                  private juce::AudioProcessorValueTreeState::Listener
{
public:
    SynthAudioProcessor();
    ~SynthAudioProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState& getParameters();
    juce::MidiKeyboardState& getKeyboardState();
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

private:
    class AnalogSound final : public juce::SynthesiserSound
    {
    public:
        bool appliesToNote(int) override { return true; }
        bool appliesToChannel(int) override { return true; }
    };

    class AnalogVoice final : public juce::SynthesiserVoice
    {
    public:
        explicit AnalogVoice(juce::AudioProcessorValueTreeState& state);

        bool canPlaySound(juce::SynthesiserSound* sound) override;
        void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound*, int) override;
        void stopNote(float velocity, bool allowTailOff) override;
        void pitchWheelMoved(int newPitchWheelValue) override;
        void controllerMoved(int controllerNumber, int newControllerValue) override;
        void aftertouchChanged(int newAftertouchValue) override;
        void channelPressureChanged(int newChannelPressureValue) override;
        void prepare(double sampleRate, int samplesPerBlock, int outputChannels);
        void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;

    private:
        class VoiceLadderFilter final : public juce::dsp::LadderFilter<float>
        {
        public:
            using juce::dsp::LadderFilter<float>::processSample;
        };

        void updateParameters();
        float modulationSource(int source, float lfo, float velocity, float aftertouch, float envelope) const;
        static float oscillatorSample(int waveform, double phase, double pulseWidth);

        juce::AudioProcessorValueTreeState& parameters;
        juce::ADSR ampEnvelope;
        juce::ADSR filterEnvelope;
        VoiceLadderFilter filter;
        VoiceLadderFilter filter2;
        juce::AudioBuffer<float> voiceBuffer;

        std::array<double, 3> phases {};
        std::array<double, 3> increments {};
        std::array<float, 3> driftOffsets {};

        double currentSampleRate = 44100.0;
        double baseFrequency = 440.0;
        double currentFrequency = 440.0;
        double targetFrequency = 440.0;
        double lfoPhase = 0.0;
        float noteVelocity = 0.0f;
        float aftertouch = 0.0f;
        int pitchWheelValue = 8192;
        juce::Random random;
    };

    void configureVoiceCount();
    void applyEffects(juce::AudioBuffer<float>& buffer);
    void loadProgram(int index);
    void parameterChanged(const juce::String& parameterID, float newValue) override;
    void debugLog(const juce::String& message);

    juce::Synthesiser synth;
    juce::MidiKeyboardState keyboardState;
    juce::AudioProcessorValueTreeState parameters;
    juce::StringArray parameterIds;
    std::unique_ptr<juce::FileLogger> logger;
    juce::dsp::Chorus<float> chorus;
    juce::Reverb reverb;
    juce::AudioBuffer<float> delayBuffer;
    int delayWritePosition = 0;
    int configuredVoiceCount = 0;
    int currentProgram = 0;
    double preparedSampleRate = 0.0;
    int preparedSamplesPerBlock = 0;
    int preparedOutputChannels = 0;
    int processLogCountdown = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SynthAudioProcessor)
};
