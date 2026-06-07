$ErrorActionPreference = "Stop"

$logPath = Join-Path $env:LOCALAPPDATA "Synth\debug.log"

if (-not (Test-Path -LiteralPath $logPath)) {
    throw "No Synth debug log found at $logPath. Launch the plugin, move a parameter, then run this again."
}

Get-Content -LiteralPath $logPath -Tail 120
