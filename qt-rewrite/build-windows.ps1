$ErrorActionPreference = "Stop"

$QtRoot = "C:\Qt\5.15.2\mingw81_64"
$SourceDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$BuildDir = Join-Path $SourceDir "build-win"
$PackageDir = Join-Path $SourceDir "package"

if (-not (Test-Path (Join-Path $QtRoot "bin\qmake.exe"))) {
    Write-Error "Qt not found at $QtRoot. Install Qt 5.x with MinGW 8.1 64-bit and update this script."
    exit 1
}

$env:PATH = "$QtRoot\bin;$env:PATH"

cmake -S $SourceDir -B $BuildDir -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH="$QtRoot" -DCMAKE_BUILD_TYPE=Release
cmake --build $BuildDir --config Release

New-Item -ItemType Directory -Force -Path $PackageDir | Out-Null
$Executable = Join-Path $BuildDir "openidea_qt_rewrite.exe"
$PackagedExecutable = Join-Path $PackageDir "openidea_qt_rewrite.exe"
Copy-Item -Force $Executable $PackagedExecutable
& (Join-Path $QtRoot "bin\windeployqt.exe") --compiler-runtime $PackagedExecutable

Write-Host ""
Write-Host "Windows executable is ready:"
Write-Host $PackagedExecutable
