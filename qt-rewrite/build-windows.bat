@echo off
setlocal

set "QT_ROOT=C:\Qt\5.15.2\mingw81_64"
set "SOURCE_DIR=%~dp0"
set "BUILD_DIR=%SOURCE_DIR%build-win"
set "PACKAGE_DIR=%SOURCE_DIR%package"

if not exist "%QT_ROOT%\bin\qmake.exe" (
    echo Qt not found at %QT_ROOT%.
    echo Install Qt 5.x with MinGW 8.1 64-bit and update this script.
    exit /b 1
)

set "PATH=%QT_ROOT%\bin;%PATH%"

cmake -S "%SOURCE_DIR%" -B "%BUILD_DIR%" -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH="%QT_ROOT%" -DCMAKE_BUILD_TYPE=Release
if errorlevel 1 exit /b 1
cmake --build "%BUILD_DIR%" --config Release
if errorlevel 1 exit /b 1

if not exist "%PACKAGE_DIR%" mkdir "%PACKAGE_DIR%"
copy /Y "%BUILD_DIR%\openidea_qt_rewrite.exe" "%PACKAGE_DIR%\openidea_qt_rewrite.exe"
if errorlevel 1 exit /b 1
"%QT_ROOT%\bin\windeployqt.exe" --compiler-runtime "%PACKAGE_DIR%\openidea_qt_rewrite.exe"
if errorlevel 1 exit /b 1

echo.
echo Windows executable is ready:
echo %PACKAGE_DIR%\openidea_qt_rewrite.exe
