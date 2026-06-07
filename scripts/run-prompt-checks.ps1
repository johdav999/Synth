$ErrorActionPreference = "Stop"

$root = Split-Path -Parent $PSScriptRoot
$cmake = "C:\Program Files\CMake\bin\cmake.exe"
$buildDir = Join-Path $root "build"

function Invoke-Native {
    & $args[0] @($args | Select-Object -Skip 1)
    if ($LASTEXITCODE -ne 0) {
        throw "Command failed with exit code $LASTEXITCODE`: $($args -join ' ')"
    }
}

$savedPath = [Environment]::GetEnvironmentVariable("PATH", "Process")
Remove-Item Env:PATH -ErrorAction SilentlyContinue
if (-not [Environment]::GetEnvironmentVariable("path", "Process")) {
    [Environment]::SetEnvironmentVariable("path", $savedPath, "Process")
}

$prompts = @(
    "1. Clean JUCE/CMake VST3 scaffold",
    "2. Monophonic voice path",
    "3. Three oscillators",
    "4. Noise and mixer drive",
    "5. 24 dB ladder low-pass filter",
    "6. Filter ADSR and amp ADSR",
    "7. One LFO",
    "8. Glide/portamento",
    "9. Basic preset system",
    "10. Simple GUI",
    "11. Polyphony",
    "12. Two-slot modulation matrix",
    "13. Velocity and aftertouch",
    "14. Basic effects",
    "15. Advanced drift and dual-filter modes"
)

Invoke-Native $cmake -S $root -B $buildDir -G "Visual Studio 17 2022" -A x64 -DSYNTH_BUILD_HOST=ON

foreach ($prompt in $prompts) {
    Write-Host ""
    Write-Host "=== Prompt check: $prompt ==="
    Invoke-Native $cmake --build $buildDir --config Release --target Synth_VST3 SynthSmokeTest

    $smokeTest = Get-ChildItem -LiteralPath $buildDir -Recurse -Filter "SynthSmokeTest.exe" | Select-Object -First 1
    if (-not $smokeTest) {
        throw "SynthSmokeTest.exe was not found after build."
    }

    Invoke-Native $smokeTest.FullName
}

Write-Host ""
Write-Host "All prompt checks passed."
