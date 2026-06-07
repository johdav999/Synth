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

# The Codex app shell can inherit both PATH and path. MSBuild treats those as
# duplicate environment keys, so keep only one for child build tools.
$savedPath = [Environment]::GetEnvironmentVariable("PATH", "Process")
Remove-Item Env:PATH -ErrorAction SilentlyContinue
if (-not [Environment]::GetEnvironmentVariable("path", "Process")) {
    [Environment]::SetEnvironmentVariable("path", $savedPath, "Process")
}

Invoke-Native $cmake -S $root -B $buildDir -G "Visual Studio 17 2022" -A x64 -DSYNTH_BUILD_HOST=ON
Invoke-Native $cmake --build $buildDir --config Release --target Synth_VST3 AudioPluginHost SynthSmokeTest

$smokeTest = Get-ChildItem -LiteralPath $buildDir -Recurse -Filter "SynthSmokeTest.exe" | Select-Object -First 1
if (-not $smokeTest) {
    throw "SynthSmokeTest.exe was not found after build."
}

Invoke-Native $smokeTest.FullName

Write-Host ""
Write-Host "Built VST3:"
Write-Host "  $buildDir\Synth_artefacts\Release\VST3\Synth.vst3"
Write-Host ""
Write-Host "Built host:"
Write-Host "  $buildDir\external\JUCE\extras\AudioPluginHost\AudioPluginHost_artefacts\Release\AudioPluginHost.exe"
