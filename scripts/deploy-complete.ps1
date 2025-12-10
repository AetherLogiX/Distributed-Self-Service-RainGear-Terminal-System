# 完整部署脚本 - 一次性解决所有运行时依赖
$ErrorActionPreference = "Stop"

Write-Host "=== RainHub Client 完整部署 ===" -ForegroundColor Green

# 路径配置
$buildDir = "C:\Users\hdcqW\Desktop\IoT Self-Service Umbrella Terminal\build\qt-release"
$qtRoot = "E:\Qt\6.9.3\mingw_64"
$qtBin = "$qtRoot\bin"
$qtPlugins = "$qtRoot\plugins"
$mingwBin = "E:\Qt\Tools\mingw1310_64\bin"
$mysqlRoot = "D:\phpstudy_pro\Extensions\MySQL8.0.12"
$mysqlLib = "$mysqlRoot\lib\libmysql.dll"

Write-Host "构建目录: $buildDir" -ForegroundColor Cyan
Write-Host "Qt根目录: $qtRoot" -ForegroundColor Cyan
Write-Host "MySQL目录: $mysqlRoot" -ForegroundColor Cyan

# 1. 创建插件目录结构
$pluginDirs = @("plugins\sqldrivers", "plugins\platforms", "plugins\imageformats")
foreach ($dir in $pluginDirs) {
    $fullPath = Join-Path $buildDir $dir
    if (!(Test-Path $fullPath)) {
        New-Item -ItemType Directory -Path $fullPath -Force | Out-Null
        Write-Host "  ✓ 创建目录: $dir" -ForegroundColor Green
    }
}

# 2. 复制Qt核心DLL
Write-Host "`n复制Qt运行时库..." -ForegroundColor Yellow
$qtDlls = @(
    "Qt6Core.dll", "Qt6Gui.dll", "Qt6Widgets.dll", "Qt6Network.dll", 
    "Qt6Sql.dll", "Qt6Svg.dll"
)
foreach ($dll in $qtDlls) {
    $src = Join-Path $qtBin $dll
    if (Test-Path $src) {
        Copy-Item -Force $src $buildDir
        Write-Host "  ✓ $dll" -ForegroundColor Green
    } else {
        Write-Host "  ✗ 未找到: $dll" -ForegroundColor Red
    }
}

# 3. 复制MinGW运行时DLL
Write-Host "`n复制MinGW运行时..." -ForegroundColor Yellow
$mingwDlls = @(
    "libgcc_s_seh-1.dll", "libstdc++-6.dll", "libwinpthread-1.dll"
)
foreach ($dll in $mingwDlls) {
    $src = Join-Path $mingwBin $dll
    if (Test-Path $src) {
        Copy-Item -Force $src $buildDir
        Write-Host "  ✓ $dll" -ForegroundColor Green
    } else {
        Write-Host "  ✗ 未找到: $dll" -ForegroundColor Red
    }
}

# 4. 复制平台插件（必需）
Write-Host "`n复制Qt插件..." -ForegroundColor Yellow
$platformSrc = Join-Path $qtPlugins "platforms\qwindows.dll"
if (Test-Path $platformSrc) {
    Copy-Item -Force $platformSrc "$buildDir\plugins\platforms\"
    Write-Host "  ✓ qwindows.dll" -ForegroundColor Green
} else {
    Write-Host "  ✗ 未找到平台插件" -ForegroundColor Red
}

# 5. 复制图像格式插件
$imagePlugins = @("qico.dll", "qjpeg.dll", "qgif.dll", "qsvg.dll")
foreach ($plugin in $imagePlugins) {
    $src = Join-Path $qtPlugins "imageformats\$plugin"
    if (Test-Path $src) {
        Copy-Item -Force $src "$buildDir\plugins\imageformats\" -ErrorAction SilentlyContinue
        Write-Host "  ✓ $plugin" -ForegroundColor Green
    }
}

# 6. 复制MySQL驱动和客户端库
Write-Host "`n复制MySQL支持..." -ForegroundColor Yellow
$sqlDriverSrc = Join-Path $qtPlugins "sqldrivers\qsqlmysql.dll"
if (Test-Path $sqlDriverSrc) {
    Copy-Item -Force $sqlDriverSrc "$buildDir\plugins\sqldrivers\"
    Write-Host "  ✓ qsqlmysql.dll" -ForegroundColor Green
} else {
    Write-Host "  ✗ 未找到MySQL驱动插件" -ForegroundColor Red
}

if (Test-Path $mysqlLib) {
    Copy-Item -Force $mysqlLib $buildDir
    Copy-Item -Force $mysqlLib "$buildDir\plugins\sqldrivers\" -ErrorAction SilentlyContinue
    Write-Host "  ✓ libmysql.dll" -ForegroundColor Green
} else {
    Write-Host "  ✗ 未找到MySQL客户端库: $mysqlLib" -ForegroundColor Red
}

# 7. 复制其他可能需要的DLL
Write-Host "`n复制其他依赖..." -ForegroundColor Yellow
$otherDlls = @("opengl32sw.dll", "D3Dcompiler_47.dll")
foreach ($dll in $otherDlls) {
    $src = Join-Path $qtBin $dll
    if (Test-Path $src) {
        Copy-Item -Force $src $buildDir -ErrorAction SilentlyContinue
        Write-Host "  ✓ $dll" -ForegroundColor Green
    }
}

Write-Host "`n=== 部署完成 ===" -ForegroundColor Green
Write-Host "可执行文件: $buildDir\rainhub_client.exe" -ForegroundColor Cyan

# 8. 测试运行
Write-Host "`n启动客户端..." -ForegroundColor Yellow
Push-Location $buildDir
try {
    Start-Process -FilePath ".\rainhub_client.exe" -Wait -NoNewWindow
} catch {
    Write-Host "启动失败: $_" -ForegroundColor Red
} finally {
    Pop-Location
}
