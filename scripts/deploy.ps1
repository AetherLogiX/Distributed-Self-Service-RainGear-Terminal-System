# 部署脚本：复制所有必需的DLL和插件到build输出目录
$ErrorActionPreference = "Stop"

$buildDir = Join-Path $PSScriptRoot "..\build\qt-release"
$qtDir = "E:\Qt\6.9.3\mingw_64"
$mysqlLib = "D:\phpstudy_pro\Extensions\MySQL8.0.12\lib\libmysql.dll"

Write-Host "=== RainHub Client 部署脚本 ===" -ForegroundColor Green

# 1. 复制Qt插件到plugins/sqldrivers
$pluginsDir = Join-Path $buildDir "plugins\sqldrivers"
if (!(Test-Path $pluginsDir)) {
    New-Item -ItemType Directory -Path $pluginsDir -Force | Out-Null
}
Write-Host "复制SQL驱动插件..." -ForegroundColor Yellow
Copy-Item -Force (Join-Path $qtDir "plugins\sqldrivers\qsqlmysql.dll") $pluginsDir
Write-Host "  ✓ qsqlmysql.dll" -ForegroundColor Green

# 2. 复制libmysql.dll到build目录
Write-Host "复制MySQL客户端库..." -ForegroundColor Yellow
if (Test-Path $mysqlLib) {
    Copy-Item -Force $mysqlLib $buildDir
    Write-Host "  ✓ libmysql.dll" -ForegroundColor Green
} else {
    Write-Host "  ✗ 未找到 libmysql.dll at $mysqlLib" -ForegroundColor Red
}

# 3. 复制libmysql.dll到plugins/sqldrivers（某些情况下需要）
Copy-Item -Force $mysqlLib $pluginsDir -ErrorAction SilentlyContinue

# 4. 复制Qt运行时DLL（如果需要）
Write-Host "检查Qt运行时DLL..." -ForegroundColor Yellow
$qtDlls = @("Qt6Core.dll", "Qt6Gui.dll", "Qt6Widgets.dll", "Qt6Sql.dll", "Qt6Network.dll", "Qt6Svg.dll")
foreach ($dll in $qtDlls) {
    $src = Join-Path $qtDir "bin\$dll"
    if (Test-Path $src) {
        Copy-Item -Force $src $buildDir -ErrorAction SilentlyContinue
    }
}

# 5. 复制MinGW运行时DLL
Write-Host "复制MinGW运行时..." -ForegroundColor Yellow
$mingwBin = "E:\Qt\Tools\mingw1310_64\bin"
$mingwDlls = @("libgcc_s_seh-1.dll", "libstdc++-6.dll", "libwinpthread-1.dll")
foreach ($dll in $mingwDlls) {
    $src = Join-Path $mingwBin $dll
    if (Test-Path $src) {
        Copy-Item -Force $src $buildDir -ErrorAction SilentlyContinue
        Write-Host "  ✓ $dll" -ForegroundColor Green
    }
}

Write-Host "`n=== 部署完成 ===" -ForegroundColor Green
Write-Host "可执行文件: $buildDir\rainhub_client.exe" -ForegroundColor Cyan

