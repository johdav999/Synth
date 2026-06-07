# Development Prompts

Run these prompts in order. After each prompt, build the VST3 and run the smoke test before moving on.

1. Create a clean JUCE/CMake VST3 synth scaffold with standalone and host-test targets.
2. Implement a monophonic voice path with MIDI input, pitch bend, amp output, and state save/restore.
3. Add three oscillator sources with shared waveform, independent tuning, levels, pulse width, and random note phase.
4. Add a noise source and mixer drive stage with soft saturation before the filter.
5. Add a 24 dB ladder-style low-pass filter with cutoff, resonance, envelope amount, and musical smoothing.
6. Add dedicated filter ADSR and amp ADSR envelopes with logarithmic time ranges.
7. Add one LFO with rate, depth, destination, and per-voice phase.
8. Add glide/portamento for monophonic performance and keep pitch bend responsive.
9. Add a basic preset system using AudioProcessor programs and parameter recall.
10. Build a simple single-page GUI exposing the MVP controls.
11. Add polyphony with a mono/poly mode switch and voice-safe preparation.
12. Add a small modulation matrix with two source/destination/amount slots.
13. Add velocity and aftertouch modulation paths for cutoff, amplitude, and matrix sources.
14. Add simple effects: chorus, delay, and reverb with conservative mix controls.
15. Add advanced analog drift and dual-filter modes while preserving CPU-safe defaults.

## GUI Follow-up

Use [GUI Implementation Prompt](gui-implementation-prompt.md) to replace the current MVP editor with the retro hardware-style GUI described in `design.md`.

## Next 5 Improvement Prompts

Run these prompts after the retro GUI pass. Keep each change inside the existing JUCE `SynthAudioProcessor` / `SynthAudioProcessorEditor` architecture described in `docs/architecture.md`, and preserve the single-page hardware workflow described in `design.md`.

1. Implement functional preset controls. Replace decorative top-bar preset actions with real previous/next/load/save buttons, keep the displayed program name in sync with the processor, save current program metadata in plugin state, and support user preset XML files that store all APVTS parameter values.
2. Add per-oscillator waveform and octave controls. Introduce automatable parameters for oscillator 1/2/3 waveform and octave, migrate DSP rendering away from the shared waveform control, expose the controls in the oscillator section, and preserve compatibility with existing state where possible.
3. Polish the GUI layout for readable hardware workflow. Reduce label overlap, make the top preset display dynamic, align oscillator rows, keep the filter as the visual anchor, and remove or convert decorative controls that conflict with active controls.
4. Expose the two-slot modulation matrix. Add compact source/destination/amount controls for Mod 1 and Mod 2, wire them to the existing APVTS parameters, keep the panel immediate and hardware-like, and verify modulation still affects rendered audio.
5. Improve analog sound quality. Replace the most alias-prone oscillator edges with a lightweight PolyBLEP correction, keep drift and random phase behavior, add conservative output safety gain/clip protection, and expand smoke tests to catch silence, non-finite samples, and basic high-note rendering.
