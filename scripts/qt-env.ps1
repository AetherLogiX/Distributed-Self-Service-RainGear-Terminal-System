$ErrorActionPreference = "Stop"

# Qt 6.9.3 + MinGW (installed at E:\Qt)
$qtRoot   = "E:\Qt\6.9.3\mingw_64"
$qtBin    = Join-Path $qtRoot "bin"

# Qt-provided MinGW toolchain (comes with the Qt install)
$qtToolchainBin = "E:\Qt\Tools\mingw1310_64\bin"

if (-not (Test-Path $qtBin)) {
    Write-Error "Qt bin not found at $qtBin. Please adjust the path."
    exit 1
}

if (-not (Test-Path $qtToolchainBin)) {
    Write-Error "MinGW toolchain not found at $qtToolchainBin. Please adjust the path."
    exit 1
}

# Export helper env vars (session scope)
$env:QT_ROOT        = $qtRoot
$env:QT_BIN         = $qtBin
$env:QT_MINGW_BIN   = $qtToolchainBin
$env:PATH           = "$qtBin;$qtToolchainBin;$env:PATH"

Write-Host "Qt PATH configured:" -ForegroundColor Green
Write-Host "  QT_BIN       = $qtBin"
Write-Host "  QT_MINGW_BIN = $qtToolchainBin"

# Quick sanity check
qmake -v

