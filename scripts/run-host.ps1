$ErrorActionPreference = "Stop"

$root = Split-Path -Parent $PSScriptRoot
$host = Join-Path $root "build\external\JUCE\extras\AudioPluginHost\AudioPluginHost_artefacts\Release\AudioPluginHost.exe"

if (-not (Test-Path -LiteralPath $host)) {
    throw "AudioPluginHost was not found. Run scripts\build.ps1 first."
}

Start-Process -FilePath $host -WorkingDirectory (Split-Path -Parent $host)
