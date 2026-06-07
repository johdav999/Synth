# Agent Guide

This project is a Voyager-inspired analog synthesizer VST. Before making architectural, DSP, UI, or product-direction changes, read these documents:

* [Architecture](docs/architecture.md)
* [Design Specification](design.md)

## Working Priorities

Use `docs/architecture.md` to understand the intended software structure, signal flow, DSP layers, and MVP scope.

Use `design.md` to understand the sound goals, workflow principles, user interface direction, analog modeling strategy, preset philosophy, and success criteria.

When the two documents overlap, preserve both intent and implementation direction:

* Sound quality comes first.
* The core synth workflow should stay immediate and hardware-like.
* Avoid unnecessary feature sprawl.
* Keep the MVP focused on the subtractive analog voice architecture.
* Prefer musical parameter behavior over purely technical ranges.

## Change Guidelines

When adding or modifying code:

* Follow the architecture document's layer boundaries.
* Keep DSP behavior aligned with the design document's sound character goals.
* Preserve the single-page core synthesis workflow for UI work.
* Treat analog imperfections as subtle musical behavior, not obvious instability.
* Add tests or focused verification for DSP, parameter, preset, and MIDI behavior when touched.

If a requested change conflicts with either document, call out the conflict before implementing it.
