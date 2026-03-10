# Rawdger

A real-time audio recorder with two frontend implementations: one using JUCE, one using Qt.

Shared DSP and audio logic lives in `core/`, which both apps link against.

## Project structure

```
rawdger/
├── core/           Shared C++ library (no framework dependencies)
├── juce-app/       JUCE implementation
└── qt-app/         Qt implementation
```

## Prerequisites

- **CMake** 3.24 or later
- **C++17** compiler
- **Qt 6** (only needed if building the Qt app)
- **Git** (JUCE is fetched automatically via CMake FetchContent)

### Installing CMake

- **macOS:** `brew install cmake`
- **Windows:** Download from https://cmake.org/download/ or `winget install Kitware.CMake`
- **Linux:** `sudo apt install cmake` (Ubuntu/Debian) or `sudo dnf install cmake` (Fedora)

### Installing Qt 6

Qt is required only if you want to build `rawdger-qt`. You can skip this and build only the JUCE app (see below).

- **macOS:** `brew install qt`
- **Windows:** Download the Qt online installer from https://www.qt.io/download-qt-installer-oss and install Qt 6 with the MSVC or MinGW kit. The default install path is `C:\Qt`.
- **Linux (Ubuntu/Debian):** `sudo apt install qt6-base-dev`
- **Linux (Fedora):** `sudo dnf install qt6-qtbase-devel`
- **Linux (Arch):** `sudo pacman -S qt6-base`

## Building

### JUCE app only (no Qt needed)

```bash
cmake -B build -DRAWDGER_BUILD_QT=OFF
cmake --build build
```

### Qt app only (no JUCE fetch)

```bash
# macOS
cmake -B build -DRAWDGER_BUILD_JUCE=OFF -DCMAKE_PREFIX_PATH=$(brew --prefix qt)

# Windows (adjust path to match your Qt install)
cmake -B build -DRAWDGER_BUILD_JUCE=OFF -DCMAKE_PREFIX_PATH=C:/Qt/6.8.0/msvc2022_64

# Linux
cmake -B build -DRAWDGER_BUILD_JUCE=OFF
```

Then build:

```bash
cmake --build build
```

### Both apps

```bash
# macOS
cmake -B build -DCMAKE_PREFIX_PATH=$(brew --prefix qt)

# Windows
cmake -B build -DCMAKE_PREFIX_PATH=C:/Qt/6.8.0/msvc2022_64

# Linux
cmake -B build
```

Then build:

```bash
cmake --build build
```

### Build a specific target

```bash
cmake --build build --target rawdger-juce
cmake --build build --target rawdger-qt
```
