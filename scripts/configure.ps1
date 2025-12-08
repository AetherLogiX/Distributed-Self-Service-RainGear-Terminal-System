param(
    [string]$BuildDir = "build/qt-release",
    [string]$BuildType = "Release"
)

$ErrorActionPreference = "Stop"

# PowerShell 5 compatibility: avoid trailing commas in param defaults.

$here     = Split-Path -Parent $MyInvocation.MyCommand.Path
$repoRoot = Resolve-Path (Join-Path $here "..")

# Load Qt env (session scoped)
. (Join-Path $here "qt-env.ps1")

$buildPath = Join-Path $repoRoot $BuildDir
if (-not (Test-Path $buildPath)) {
    New-Item -ItemType Directory -Path $buildPath | Out-Null
}

Push-Location $buildPath

# Use qt-cmake to pick up correct Qt toolchain
qt-cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=$BuildType $repoRoot

Pop-Location

