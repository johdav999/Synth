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
