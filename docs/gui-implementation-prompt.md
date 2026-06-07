# GUI Implementation Prompt

Use this prompt to replace the current MVP GUI with a polished retro analog synthesizer interface.

## Prompt

Implement a new custom JUCE GUI for this VST synth using the existing `SynthAudioProcessorEditor` as the functional baseline.

The current GUI is a flat MVP layout with:

* Black background
* Plain JUCE rotary sliders
* Yellow/orange knob arcs
* White labels
* Raw numeric text boxes
* Section labels for `OSCILLATORS`, `MIXER`, `FILTER`, `ENVELOPES`, `MODULATION`, and `OUTPUT / FX`
* A large central `Cutoff` knob
* Comboboxes for `Waveform`, `Voice`, `Filter Mode`, and `LFO Dest`

Keep the same parameters and attachments working, but replace the visual treatment with a pixel-accurate retro hardware-style design.

## Visual Target

Create a 1970s analog hardware-inspired VST panel while staying legally distinct from any real instrument.

The GUI should feel like a physical synth front panel:

* Dark charcoal or near-black metal faceplate
* Warm walnut or dark wood side cheeks on left and right
* Cream/off-white labels and tick marks
* Amber/gold accent color for active arcs, indicators, and section headings
* Black knurled knobs with cream indicator lines
* Large central `Cutoff` knob as the visual anchor
* Small amber LEDs for active/status controls where useful
* Subtle bevels, shadows, panel dividers, and screw heads
* Tactile toggle styling for binary/mode controls
* Compact preset/status strip across the top

Avoid:

* Any real brand logo, model name, or exact panel copy
* Modern web gradients
* Floating card UI
* Cartoon styling
* Excessive fake wear, scratches, or dirt
* Hiding core synthesis controls behind tabs

## Layout Requirements

Preserve a single-page workflow.

Use the current GUI screenshot as the baseline for control coverage and rough positioning, but improve spacing and alignment.

Required sections:

* `OSCILLATORS`
* `MIXER`
* `FILTER`
* `ENVELOPES`
* `MODULATION`
* `OUTPUT / FX`

Required visible controls:

* Waveform
* Voice mode
* Osc 1 Tune
* Osc 2 Tune
* Osc 3 Tune
* Osc 1 Level
* Osc 2 Level
* Osc 3 Level
* Noise
* Pulse Width
* Mixer Drive
* Analog Drift
* Drift Rate
* Cutoff
* Resonance
* Filter Mode
* Filter Env
* Filter Drive
* Filter ADSR
* Amp ADSR
* LFO destination
* LFO Rate
* LFO Depth
* Glide
* Chorus
* Delay
* Reverb
* Output

## Implementation Guidance

Refactor the editor code in `Source/PluginEditor.h` and `Source/PluginEditor.cpp`.

Add custom LookAndFeel classes instead of relying on default JUCE styling:

* `RetroLookAndFeel`
* custom rotary knob drawing
* custom combobox drawing
* custom label/textbox colors

Use custom painting for:

* faceplate
* wood side cheeks
* section dividers
* screw heads
* section headers
* top preset/status strip

Make the GUI fixed-size for now, but keep coordinates organized through helper functions or layout structs so future resizing is possible.

Recommended dimensions:

```text
1120 x 620
```

The current GUI has cramped numeric boxes. Improve it by:

* making value boxes smaller and darker
* using concise numeric precision
* keeping labels aligned above knobs
* increasing spacing around the large cutoff knob
* avoiding overlaps between `Cutoff`, `Drive`, and `Filter Env`

## Functional Requirements

Do not break any parameter IDs or attachments.

All existing controls must still update the underlying `AudioProcessorValueTreeState` parameters.

The smoke test must still pass:

```powershell
.\scripts\build.ps1
```

The 15-prompt check must still pass:

```powershell
.\scripts\run-prompt-checks.ps1
```

After opening the plugin in AudioPluginHost, moving a control should produce debug log entries:

```powershell
.\scripts\show-debug-log.ps1
```

Expected log lines:

```text
parameterChanged FilterCutoff=...
parameterChanged OutputGain=...
```

## Acceptance Criteria

The finished GUI should:

* Look clearly inspired by premium retro analog hardware
* Retain all current controls
* Make the cutoff knob visually dominant
* Use wood side panels and dark metal faceplate styling
* Be readable at normal plugin size
* Avoid overlap and cramped text
* Build successfully as VST3
* Pass the smoke test
* Show parameter-change logs when controls move

Do not implement DSP changes as part of this prompt unless required to fix a GUI binding bug.
