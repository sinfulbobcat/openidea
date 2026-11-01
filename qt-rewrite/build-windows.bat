@echo off
setlocal

set "QT_ROOT=C:\Qt\6.8.3\msvc2022_64"
set "SOURCE_DIR=%~dp0"
set "BUILD_DIR=%SOURCE_DIR%build-win"
set "PACKAGE_DIR=%SOURCE_DIR%package"
set "REPO_DIR=%SOURCE_DIR%.."

if not exist "%QT_ROOT%\bin\windeployqt.exe" (
    echo Qt not found at %QT_ROOT%.
    echo Install Qt 6.8.3 with MSVC 2022 64-bit and WebEngine, then update this script.
    exit /b 1
)

set "PATH=%QT_ROOT%\bin;%PATH%"

pushd "%REPO_DIR%"
pnpm install --frozen-lockfile
if errorlevel 1 exit /b 1
pnpm build
if errorlevel 1 exit /b 1
popd

cmake -S "%SOURCE_DIR%" -B "%BUILD_DIR%" -G "Visual Studio 17 2022" -A x64 -DCMAKE_PREFIX_PATH="%QT_ROOT%"
if errorlevel 1 exit /b 1
cmake --build "%BUILD_DIR%" --config Release
if errorlevel 1 exit /b 1

if not exist "%PACKAGE_DIR%" mkdir "%PACKAGE_DIR%"
if not exist "%PACKAGE_DIR%\webui" mkdir "%PACKAGE_DIR%\webui"
copy /Y "%BUILD_DIR%\Release\openidea_qt_rewrite.exe" "%PACKAGE_DIR%\openidea_qt_rewrite.exe"
if errorlevel 1 exit /b 1
robocopy "%REPO_DIR%\dist" "%PACKAGE_DIR%\webui" /E
if errorlevel 8 exit /b 1
"%QT_ROOT%\bin\windeployqt.exe" --compiler-runtime "%PACKAGE_DIR%\openidea_qt_rewrite.exe"
if errorlevel 1 exit /b 1

echo.
echo Windows executable is ready:
echo %PACKAGE_DIR%\openidea_qt_rewrite.exe
