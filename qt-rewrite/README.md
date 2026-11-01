# OpenIDEA Qt Rewrite

This directory is a Qt-based C++ rewrite scaffold for the OpenIDEA IDE.

It is intentionally separate from the existing Rust + Tauri + React codebase and represents a clean C++ desktop rewrite path with GTK-aware theming support.

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Windows build for testing

This Linux container cannot produce a real Windows `.exe` without a Windows Qt toolchain. If you want the app on Windows, build it on a Windows machine with Qt 5 installed.

1. Install Qt 5 with MinGW 8.1 64-bit.
2. Update the `QT_ROOT` path in `build-windows.bat` or `build-windows.ps1` if your install is elsewhere.
3. Run one of the scripts:

```bat
build-windows.bat
```

or

```powershell
pwsh ./build-windows.ps1
```

The script will generate a self-contained test folder at `package/`, including `openidea_qt_rewrite.exe` and the required Qt and MinGW runtime DLLs.

## Theme behavior

The app reads GTK-related environment variables such as `GTK_THEME`, `GDK_THEME`, `XDG_CURRENT_DESKTOP`, and `COLOR_SCHEME`, then applies a matching dark or light palette to the Qt app so it adapts sensibly to GTK-based desktop themes.
