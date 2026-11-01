# OpenIDEA Qt Rewrite

This directory contains the C++ desktop host for the existing React workbench.

The host uses Qt WebEngine for the React UI and Qt WebChannel for C++ IPC. The C++ backend is being ported incrementally; currently, only `app_info` is implemented, so project and editor operations are not yet functional in this host.

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Windows build for testing

The Windows build requires Qt 6.8.3 with the MSVC 2022 64-bit kit and WebEngine, plus Node.js, pnpm, CMake, and Visual Studio 2022 C++ tools.

1. Update `QT_ROOT` in `build-windows.bat` or `build-windows.ps1` if Qt is installed elsewhere.
2. Run one of the scripts:

```bat
build-windows.bat
```

or

```powershell
pwsh ./build-windows.ps1
```

The script builds React, copies its production assets to `package/webui/`, and generates the Qt runtime files next to `package/openidea_qt_rewrite.exe`.

## Theme behavior

The app reads GTK-related environment variables such as `GTK_THEME`, `GDK_THEME`, `XDG_CURRENT_DESKTOP`, and `COLOR_SCHEME`, then applies a matching dark or light palette to the Qt app so it adapts sensibly to GTK-based desktop themes.
