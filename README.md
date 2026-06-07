# Synth

Voyager-inspired analog synth VST built with JUCE and CMake.

## Build

```powershell
.\scripts\build.ps1
```

The build creates:

* `build\Synth_artefacts\Release\VST3\Synth.vst3`
* `build\external\JUCE\extras\AudioPluginHost\AudioPluginHost_artefacts\Release\AudioPluginHost.exe`

## Test In Host

```powershell
.\scripts\run-host.ps1
```

In AudioPluginHost, scan or add the built VST3 path:

```text
build\Synth_artefacts\Release\VST3\Synth.vst3
```

For development, prefer the build-folder VST3 above instead of the system VST3 copy. A running host can lock the system plugin file and prevent rebuilds.

## Debug Log

The plugin writes parameter-change and render snapshots to:

```text
%LOCALAPPDATA%\Synth\debug.log
```

After moving a GUI control in the host, inspect the latest entries with:

```powershell
.\scripts\show-debug-log.ps1
```

Project direction lives in:

* `docs\architecture.md`
* `design.md`
* `agents.md`
