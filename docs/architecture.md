# Architecture: Analog-Modelled Subtractive Synth VST

## 1. Project Goal

The goal is to build a VST instrument inspired by classic analog subtractive synthesizers, especially the Moog Voyager style of architecture and sound character.

The instrument should not be a direct clone. It should be a legally distinct, original synthesizer with a similar design philosophy:

- Three analog-style oscillators
- Mixer saturation
- Ladder-style low-pass filter
- Filter and amp envelopes
- LFO/modulation system
- Monophonic performance focus
- Glide/portamento
- Warm, unstable, nonlinear analog behaviour

The main design priority is analog feel rather than feature quantity.

---

## 2. High-Level Signal Flow

```text
MIDI Input
    ↓
Voice Manager
    ↓
Voice Engine
    ↓
Oscillator Section
    ↓
Mixer / Drive Stage
    ↓
Ladder Filter
    ↓
VCA / Amp Envelope
    ↓
Output Saturation
    ↓
Plugin Output
```

For the MVP, the synth should be monophonic. Polyphony can be added later by extending the voice manager.

---

## 3. Core Software Layers

```text
Plugin Layer
 ├── VST3 wrapper
 ├── Parameter system
 ├── Preset system
 ├── MIDI input handling
 ├── Automation support
 └── GUI

Synth Engine
 ├── Voice manager
 ├── Voice object
 │    ├── Oscillator 1
 │    ├── Oscillator 2
 │    ├── Oscillator 3
 │    ├── Noise generator
 │    ├── Mixer / saturation
 │    ├── Ladder filter
 │    ├── Filter envelope
 │    ├── Amp envelope
 │    ├── LFO
 │    └── Modulation routing
 ├── Global modulation
 └── Output stage

DSP Utilities
 ├── Anti-aliased waveform generation
 ├── Parameter smoothing
 ├── Random drift generators
 ├── Saturation functions
 ├── Envelope curve utilities
 └── Oversampling utilities
```

---

## 4. Recommended Technology Stack

### Primary Recommendation

Use **C++ with JUCE**.

Reasons:

- Mature VST3 plugin support
- Cross-platform support
- Good audio/MIDI abstractions
- Built-in parameter automation system
- GUI framework included
- Common industry choice for commercial plugins

### Alternative Options

- **iPlug2**: lighter C++ plugin framework
- **Rust + nih-plug**: modern Rust-based plugin development
- **CLAP support**: optional future plugin format

---

## 5. Main Components

## 5.1 Plugin Processor

Responsible for communication with the DAW.

Responsibilities:

- Receive MIDI events
- Process audio blocks
- Expose plugin parameters
- Handle automation
- Load/save plugin state
- Route data to the synth engine

Typical class:

```text
SynthAudioProcessor
```

Important methods:

```text
prepareToPlay(sampleRate, blockSize)
processBlock(audioBuffer, midiBuffer)
releaseResources()
getStateInformation()
setStateInformation()
```

---

## 5.2 Parameter System

All user-facing controls should be represented as automatable parameters.

Examples:

```text
osc1_waveform
osc1_tune
osc1_fine
osc1_level
osc2_waveform
osc2_tune
osc2_fine
osc2_level
osc3_waveform
osc3_tune
osc3_fine
osc3_level
noise_level
mixer_drive
filter_cutoff
filter_resonance
filter_drive
filter_keytrack
filter_env_amount
filter_attack
filter_decay
filter_sustain
filter_release
amp_attack
amp_decay
amp_sustain
amp_release
lfo_rate
lfo_amount
glide_time
master_volume
```

Use parameter smoothing for all parameters that affect audio directly.

Important smoothing targets:

- Filter cutoff
- Resonance
- Oscillator pitch
- Oscillator level
- Mixer drive
- Output volume
- Modulation depth

---

## 5.3 Voice Manager

The voice manager handles MIDI note logic.

For MVP monophonic mode, it should support:

- Last-note priority
- Legato mode
- Retrigger mode
- Glide/portamento
- Note stack for held notes

Later, it can be extended to support polyphony.

Responsibilities:

```text
noteOn(note, velocity)
noteOff(note)
renderNextBlock(buffer)
setPitchBend(value)
setModWheel(value)
```

---

## 5.4 Voice Engine

The voice object contains the sound-generating parts of the synth.

One voice contains:

```text
Oscillator 1
Oscillator 2
Oscillator 3
Noise generator
Mixer
Ladder filter
Filter envelope
Amp envelope
LFO
Modulation calculation
Output stage
```

For monophonic MVP, only one active voice is required.

---

## 6. Oscillator Architecture

## 6.1 Oscillator Features

Each oscillator should support:

- Saw
- Square / pulse
- Triangle
- Sine-like waveform
- Octave selection
- Fine tuning
- Pulse-width control
- Pulse-width modulation
- Key tracking on/off
- Optional oscillator sync
- Optional frequency modulation

## 6.2 Anti-Aliasing

Oscillators must be anti-aliased.

Recommended methods:

- PolyBLEP
- MinBLEP
- Band-limited wavetable oscillators

Avoid naive saw and square oscillators because they produce strong aliasing at higher pitches.

## 6.3 Analog Oscillator Behaviour

To create analog feel, each oscillator should include:

- Slight pitch drift
- Independent oscillator instability
- Random phase on note start
- Small waveform shape imperfections
- Tiny tuning offsets

Example drift model:

```text
finalPitch = basePitch
           + staticDetune
           + slowRandomDrift
           + temperatureDrift
           + modulation
```

The drift should be slow and subtle. It should not sound like vibrato.

---

## 7. Noise Generator

The noise generator should provide:

- White noise
- Pink noise, optional
- Noise level control

Noise is mixed before the filter.

Very low-level background noise may also be added after the VCA to emulate analog noise floor.

---

## 8. Mixer and Drive Stage

The mixer is a critical part of the analog character.

Signal flow:

```text
Oscillator 1
Oscillator 2
Oscillator 3
Noise
    ↓
Level mixing
    ↓
Nonlinear saturation
    ↓
Filter input
```

The mixer should not be perfectly clean. It should introduce mild saturation before the filter.

This is important because the filter should react differently depending on the input level.

Recommended saturation functions:

```text
tanh(x)
soft clipper
asymmetric saturation
waveshaper with drive compensation
```

The saturation should be gain-staged carefully so the synth can sound clean at low levels and aggressive when driven.

---

## 9. Ladder Filter

The ladder filter is the most important sound-shaping component.

## 9.1 Filter Requirements

The filter should behave like a transistor-ladder-inspired low-pass filter.

Core features:

- 24 dB/oct low-pass response
- Resonance
- Self-oscillation at high resonance
- Drive/saturation
- Keyboard tracking
- Envelope modulation
- Smooth cutoff modulation

## 9.2 Recommended Filter Model

Use a zero-delay feedback ladder filter model if possible.

Reasons:

- Better resonance behaviour
- More realistic cutoff modulation
- More convincing self-oscillation
- Better nonlinear interaction

## 9.3 Nonlinear Behaviour

The filter should include nonlinearities:

- Saturation inside ladder stages
- Resonance saturation
- Input-level-dependent response
- Bass reduction at high resonance
- Slight cutoff instability

This interaction is central to the analog feel.

---

## 10. Envelopes

The synth should include two primary ADSR envelopes:

```text
Filter Envelope
Amp Envelope
```

## 10.1 Envelope Behaviour

The envelopes should use exponential analog-style curves rather than simple linear ramps.

Important features:

- Attack
- Decay
- Sustain
- Release
- Legato behaviour
- Retrigger behaviour
- Slight timing variation, optional

## 10.2 Analog Feel

Analog-style envelopes should have:

- Rounded attack and decay shapes
- Slight variation between notes
- Smooth transitions
- No zipper noise

---

## 11. LFO and Modulation

## 11.1 LFO Features

The MVP should include one LFO.

Waveforms:

- Sine
- Triangle
- Square
- Saw
- Sample and hold

Destinations:

- Oscillator pitch
- Filter cutoff
- Pulse width
- Amp level

## 11.2 Modulation System

Initial MVP can use fixed modulation routes.

Later version can add a modulation matrix.

Possible modulation sources:

```text
LFO
Filter envelope
Amp envelope
Velocity
Aftertouch
Mod wheel
Key tracking
Pitch bend
Random drift
```

Possible destinations:

```text
Oscillator pitch
Oscillator pulse width
Oscillator level
Filter cutoff
Filter resonance
Mixer drive
Amp level
Pan
```

---

## 12. Glide and Monophonic Behaviour

Because this synth is inspired by classic monophonic analog synths, glide and note priority are important.

Required features:

- Glide time
- Legato glide
- Always glide mode
- Last-note priority
- Optional low-note/high-note priority later

Glide should smooth pitch changes between notes rather than instantly jumping to the new note frequency.

---

## 13. Output Stage

The output stage should include:

- VCA controlled by amp envelope
- Master volume
- Soft clipping
- Optional analog-style output saturation
- Optional noise floor

The output should avoid harsh digital clipping.

A final safety limiter can be added internally, but it should not be part of the core sound unless intentionally designed that way.

---

## 14. Analog Feel Design Principles

Analog feel is created by many small behaviours working together.

Important ingredients:

```text
Oscillator drift
Random oscillator phase
Mixer saturation
Filter saturation
Nonlinear resonance
Envelope curve realism
Parameter smoothing
Voice-to-voice variation
Low-level noise
Small waveform imperfections
```

The most important areas are:

1. Ladder filter quality
2. Oscillator behaviour
3. Mixer and filter drive
4. Parameter smoothing
5. Envelope curves

Avoid making every imperfection too obvious. The goal is subtle instability, not broken tuning.

---

## 15. Oversampling

Oversampling should be considered for nonlinear parts of the synth.

Recommended oversampled sections:

- Mixer saturation
- Ladder filter
- Output saturation

Possible oversampling rates:

```text
2x for normal quality
4x for high quality
8x for render/offline quality
```

The plugin should expose a quality mode later:

```text
Eco
Standard
High
Offline Render
```

---

## 16. GUI Architecture

The GUI should be separated from the DSP engine.

The GUI should only:

- Display parameter values
- Send parameter changes
- Show modulation status
- Load/save presets

The GUI should not contain DSP logic.

Suggested UI sections:

```text
Oscillators
Mixer
Filter
Envelopes
LFO / Modulation
Glide / Performance
Output
Preset browser
```

The first GUI version can be simple. Sound engine quality is more important than visual polish early in the project.

---

## 17. Preset System

Presets should store all parameter values.

Preset metadata:

```text
Name
Author
Category
Description
Version
```

Suggested categories:

```text
Bass
Lead
Pad
FX
Percussion
Drone
Sequence
Experimental
```

Presets should be forward-compatible when new parameters are added.

---

## 18. MVP Scope

The first working version should include:

- VST3 plugin
- Monophonic voice engine
- Three anti-aliased oscillators
- Noise generator
- Mixer with drive
- Ladder-style low-pass filter
- Filter ADSR
- Amp ADSR
- One LFO
- Glide/portamento
- Pitch bend
- Mod wheel to vibrato or filter
- Master volume
- Basic preset save/load
- Simple GUI

Do not start with advanced features before the core tone is good.

---

## 19. Post-MVP Features

After the MVP, add:

- Polyphony
- Modulation matrix
- More filter modes
- Dual-filter mode
- Oscillator sync
- Audio-rate modulation
- Effects section
- Arpeggiator
- Step sequencer
- MPE support
- Advanced preset browser
- Skin/theme system
- CLAP/AU support

---

## 20. Suggested Development Order

## Phase 1: Headless DSP Prototype

Build the synth engine without GUI.

Tasks:

- MIDI note handling
- One oscillator
- Amp envelope
- Audio rendering
- Basic test output

## Phase 2: Oscillator Section

Tasks:

- Add three oscillators
- Add waveform selection
- Add tuning controls
- Add anti-aliasing
- Add drift and random phase

## Phase 3: Mixer and Filter

Tasks:

- Add oscillator mixer
- Add mixer drive
- Add ladder filter
- Add resonance
- Add filter envelope
- Add parameter smoothing

## Phase 4: VST Integration

Tasks:

- Wrap engine in JUCE plugin
- Add parameters
- Add automation
- Add MIDI input from DAW
- Add state save/load

## Phase 5: GUI

Tasks:

- Build simple editor
- Connect controls to parameters
- Add preset management
- Add visual feedback

## Phase 6: Analog Feel Polish

Tasks:

- Tune drift behaviour
- Tune saturation
- Tune filter resonance
- Tune envelope curves
- Add voice/component variation
- Add quality/oversampling modes

---

## 21. Testing Strategy

## 21.1 DSP Tests

Test:

- Oscillator tuning accuracy
- Envelope timing
- Filter cutoff response
- Resonance behaviour
- MIDI note handling
- Parameter smoothing
- No denormal CPU issues
- No clicks during parameter changes

## 21.2 Audio Tests

Listen for:

- Aliasing at high notes
- Harsh resonance
- Clicks on note start/end
- Zipper noise
- Unstable gain staging
- Excessive drift
- Weak bass response

## 21.3 Plugin Tests

Test in several DAWs:

- Cubase
- Ableton Live
- Logic Pro, if AU is added
- Reaper
- FL Studio

Test:

- Loading/unloading
- Automation
- Preset recall
- MIDI input
- Offline rendering
- Different sample rates
- Different buffer sizes

---

## 22. Performance Considerations

Important optimizations:

- Avoid memory allocation in the audio thread
- Avoid locks in the audio thread
- Precompute tables where useful
- Use SIMD later if needed
- Handle denormals
- Use efficient oversampling only where needed
- Keep GUI and DSP communication lock-free

The audio thread must remain deterministic and real-time safe.

---

## 23. Legal and Product Positioning Notes

The synth should be described as:

```text
Analog-modelled subtractive synthesizer inspired by classic monophonic analog synth architecture.
```

Avoid:

- Using protected brand names in the product name
- Copying exact visual panel design
- Claiming it is a Moog Voyager clone
- Claiming circuit-level accuracy without verified modelling

Safe positioning:

```text
A warm, analog-inspired, three-oscillator subtractive synthesizer with ladder-style filtering and expressive monophonic performance controls.
```

---

## 24. Key Design Principle

The core sound should be developed before the final GUI.

A good development rule:

```text
First make it sound alive.
Then make it easy to use.
Then make it beautiful.
```
