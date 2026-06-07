# Voyager-Inspired Analog Synth VST

## Design Specification

Version: 1.0

---

# Vision

Create a premium virtual analog synthesizer inspired by the sound, workflow, and musicality of classic Moog instruments, particularly the Voyager.

The goal is not circuit cloning but rather to capture:

* Warm analog tone
* Immediate hands-on workflow
* Rich bass response
* Musical filter behavior
* Expressive performance controls
* Vintage imperfections

The synthesizer should feel alive and organic while remaining modern, stable, and CPU efficient.

---

# Design Principles

## Sound First

Every design decision should prioritize sound quality over visual appearance.

If a DSP feature improves realism, it takes priority over GUI improvements.

---

## Fast Workflow

Users should be able to create useful sounds within seconds.

Important controls should never be hidden behind menus.

The user should always be able to see:

* Oscillator settings
* Mixer settings
* Filter settings
* Envelopes

simultaneously.

---

## Hardware Mindset

The instrument should feel like a physical synthesizer.

Avoid:

* Endless pages
* Deep nested menus
* Hundreds of modulation slots

Prefer:

* Dedicated controls
* Direct manipulation
* Immediate feedback

---

## Musical Behavior

Parameters should behave musically.

Examples:

* Resonance should remain useful throughout its range.
* Envelope times should be logarithmic.
* Detune should be optimized for musical intervals.
* Saturation should enhance rather than destroy signals.

---

# Target Users

## Primary

Synthesizer enthusiasts

* Progressive rock
* Electronic music
* Synthwave
* Ambient
* Film scoring

## Secondary

Producers seeking:

* Bass sounds
* Leads
* Pads
* Analog textures

## Tertiary

Sound designers interested in vintage analog character.

---

# Sound Character Goals

## Bass

Must feel:

* Deep
* Powerful
* Round
* Stable

A single oscillator patch should already sound substantial.

---

## Leads

Must feel:

* Expressive
* Warm
* Present
* Dynamic

Pitch wheel and modulation wheel should play a major role.

---

## Pads

Must feel:

* Wide
* Organic
* Moving

Micro variations should create subtle movement even without modulation.

---

## Filter

The filter is the centerpiece.

Desired characteristics:

* Smooth sweeps
* Strong low-end
* Musical resonance
* Natural self-oscillation
* Saturation under drive

---

# User Interface

## Layout Philosophy

Single-page design.

No tabbed interface for core synthesis.

Suggested sections:

---

## OSCILLATORS | MIXER | FILTER | ENVELOPES

## MODULATION | PERFORMANCE | OUTPUT

---

## GUI Visual Style

The GUI should use a retro analog hardware-inspired style.

The visual direction should feel warm, tactile, and premium, with clear inspiration from 1970s analog synthesizer panels while remaining legally distinct from any specific real instrument.

Preferred visual elements:

* Dark charcoal or near-black metal faceplate
* Warm walnut or dark wood side panels
* Large black knurled knobs with light indicator lines
* Cream or off-white panel text
* Amber LEDs or small warm status lights
* Toggle switches for binary performance options
* Slider-style controls for envelope stages where useful
* Subtle panel dividers and grouped hardware sections
* Slightly worn or satin material finish, not glossy plastic

The large filter cutoff knob should act as the main visual anchor.

The interface should feel like a real instrument photographed or rendered from the front, not like a flat generic software skin.

Avoid:

* Logos or brand marks referencing real synthesizer companies
* Exact panel layouts from existing hardware
* Decorative gradients that make the UI feel modern-web rather than hardware-like
* Overly skeuomorphic dirt, scratches, or fake aging that harms readability
* Hidden controls that undermine the single-page workflow

Use the generated retro analog GUI concept image as the style reference for future GUI tasks: wood side cheeks, dark metal panel, amber highlights, large tactile controls, and clearly grouped sections.

---

# Oscillator Section

Per oscillator:

* Waveform
* Octave
* Fine tune
* Level
* Pulse width
* PWM amount

Visual waveform display optional.

---

# Mixer Section

Controls:

* Osc 1 level
* Osc 2 level
* Osc 3 level
* Noise level
* External input
* Mixer drive

The mixer should visually communicate gain staging.

---

# Filter Section

Controls:

* Cutoff
* Resonance
* Keyboard tracking
* Envelope amount
* Filter drive

Large cutoff knob should be the visual centerpiece.

---

# Envelope Section

Two dedicated ADSR envelopes:

* Filter Envelope
* Amp Envelope

Controls should be visible at all times.

Graphical envelope display optional.

---

# Modulation Section

Sources:

* LFO
* Mod Wheel
* Velocity
* Aftertouch
* Envelopes

Destinations:

* Pitch
* Cutoff
* Resonance
* PWM
* Amplitude

Keep modulation intentionally limited.

---

# Performance Controls

Include:

* Pitch Bend
* Mod Wheel
* Glide
* Legato
* Mono/Poly modes
* Unison

These should be quickly accessible.

---

# Analog Modeling Strategy

## Oscillator Drift

Every voice should contain:

* Slow tuning drift
* Independent oscillator movement
* Slight voice variation

Drift must remain subtle.

The user should feel it more than hear it.

---

## Random Phase

Oscillators should not restart from identical phase positions.

Each note should begin slightly differently.

---

## Component Tolerances

Each voice receives small random offsets:

* Filter cutoff
* Oscillator tuning
* Envelope speed
* Saturation level

These offsets remain constant for the voice.

---

## Saturation

Saturation exists in three locations:

1. Mixer
2. Filter
3. Output stage

All stages should be subtle.

The goal is warmth rather than distortion.

---

# Preset Philosophy

Presets should demonstrate:

* Basses
* Leads
* Pads
* Sequences
* Effects
* Vintage sounds

Avoid overwhelming users with thousands of presets.

Target:

100-200 high-quality presets.

---

# Performance Targets

## Startup

Less than 2 seconds.

## Preset Loading

Instant.

## MIDI Response

Perceptually immediate.

## CPU Usage

Target:

Less than 5% CPU for a typical patch on a modern desktop CPU.

---

# Future Features

Potential Version 2 additions:

* Dual filter mode
* Polyphonic aftertouch
* MPE support
* Sequencer
* Arpeggiator
* Effects rack
* Preset browser
* Vintage calibration modes

---

# Success Criteria

A successful instrument should:

* Inspire musicians to play.
* Sound convincing without effects.
* Produce powerful basses and leads.
* Feel alive and organic.
* Encourage experimentation.
* Remain simple enough to learn in one session.

When a musician closes their eyes, the instrument should feel closer to a hardware synthesizer than to a software plugin.
