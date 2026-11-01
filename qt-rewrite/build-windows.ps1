$ErrorActionPreference = "Stop"

$QtRoot = "C:\Qt\6.8.3\msvc2022_64"
$SourceDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$RepoDir = Split-Path -Parent $SourceDir
$BuildDir = Join-Path $SourceDir "build-win"
$PackageDir = Join-Path $SourceDir "package"
$WebUiDir = Join-Path $PackageDir "webui"

if (-not (Test-Path (Join-Path $QtRoot "bin\windeployqt.exe"))) {
    Write-Error "Qt not found at $QtRoot. Install Qt 6.8.3 with MSVC 2022 64-bit and WebEngine, then update this script."
    exit 1
}

$env:PATH = "$QtRoot\bin;$env:PATH"

Push-Location $RepoDir
pnpm install --frozen-lockfile
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
pnpm build
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
Pop-Location

cmake -S $SourceDir -B $BuildDir -G "Visual Studio 17 2022" -A x64 -DCMAKE_PREFIX_PATH="$QtRoot"
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
cmake --build $BuildDir --config Release
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

New-Item -ItemType Directory -Force -Path $PackageDir | Out-Null
$Executable = Join-Path $BuildDir "Release\openidea_qt_rewrite.exe"
$PackagedExecutable = Join-Path $PackageDir "openidea_qt_rewrite.exe"
New-Item -ItemType Directory -Force -Path $WebUiDir | Out-Null
Copy-Item -Force $Executable $PackagedExecutable
Copy-Item -Path (Join-Path $RepoDir "dist\*") -Destination $WebUiDir -Recurse -Force
& (Join-Path $QtRoot "bin\windeployqt.exe") --compiler-runtime $PackagedExecutable
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

Write-Host ""
Write-Host "Windows executable is ready:"
Write-Host $PackagedExecutable
