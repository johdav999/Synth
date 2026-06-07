$ErrorActionPreference = "Stop"

$root = Split-Path -Parent $PSScriptRoot
$hostPath = Join-Path $root "build\external\JUCE\extras\AudioPluginHost\AudioPluginHost_artefacts\Release\AudioPluginHost.exe"

if (-not (Test-Path -LiteralPath $hostPath)) {
    throw "AudioPluginHost was not found. Run scripts\build.ps1 first."
}

Start-Process -FilePath $hostPath -WorkingDirectory (Split-Path -Parent $hostPath)
