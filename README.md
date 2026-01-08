# Cosmic (Pacman-like) — C++ / CMake project

A small Pacman-style game implemented in C++ using SFML and CMake. The repository contains the application source under `GUI/` and `Logic/`, assets under `assets/`, and CMake configuration files for building on multiple platforms.

## Features

- Simple 2D game using SFML
- CMake-based cross-platform build
- Unit tests available under `Tests/`

## Requirements

- CMake 3.15+
- A C++17-capable compiler (g++, clang, or MSVC)
- Network connection

## Build methods

This project supports two primary workflows:

- CL/CI method — command-line builds suitable for local development and CI pipelines.
- Visual Studio (VS) method — for developers using Visual Studio on Windows.

Each method includes the essential commands; platform-specific dependency installation is noted where required.

### CL/CI method (command-line and CI)

Use this method for command-line development and for continuous integration pipelines (GitHub Actions, GitLab CI, etc.). The CMake commands are cross-platform; dependency installation is platform-specific.

> **Note:** If you run CI on `ubuntu-latest` or develop on Debian/Ubuntu, install the packages below. For other distros or platforms, install equivalent packages or provide prebuilt binaries (e.g., `SFML`) via your package manager or a dependency manager.
>
> Install dependencies (example for Debian/Ubuntu CI runner or developer machine):
```bash
sudo add-apt-repository universe
sudo apt-get update
sudo apt-get install -y \
  libx11-dev libxrandr-dev libxcursor-dev libxi-dev libudev-dev \
  libopenal-dev libflac-dev libvorbis-dev \
  libgl1-mesa-dev libglu1-mesa-dev \
  libfreetype6-dev libjpeg-dev libpng-dev \
  cmake g++
```

Configure, build and test (CLI):

```bash
# Optional: export BUILD_TYPE=Release
cmake -S . -B build -DCMAKE_BUILD_TYPE=${BUILD_TYPE:-Release}
cmake --build build --config ${BUILD_TYPE:-Release} -- -j$(nproc)
cd build
ctest -C ${BUILD_TYPE:-Release} --output-on-failure
```

Run the application (from project root):

```bash
./build/PacmanGame
```

### Visual Studio (VS) method

Use this method if you develop with Visual Studio on Windows. It uses the Visual Studio generator and preserves Visual Studio project/solution metadata in the `build/` directory.

1. Open "x64 Native Tools Command Prompt for VS" or Developer PowerShell for Visual Studio.
2. Configure and generate Visual Studio project files:

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release
```

3. Build the solution from the command line or from within Visual Studio:

```powershell
cmake --build build --config Release
```

4. Run tests (optional):

```powershell
cd build/tests
ctest -C Release
```

5. Run the built executable (for example `PacmanGame.exe`) from `build/` (usually in build/Bin) or the Visual Studio output directory.

## Project layout (important files)

- `CMakeLists.txt` — top-level CMake configuration
- `GUI/`, `Logic/` — project source code
- `assets/` — images and other runtime assets (ensure these are copied to the runtime working directory)
- `Tests/` — unit tests and test configuration
- `diagrams/` — UML diagrams realized in Visual Paradigm

## Classic soundtrack

The game now looks for a background track at `assets/audio/pacman_theme.ogg`. Because the original Namco recording is copyrighted, the repository does **not** ship the file. To enable the music:

1. Obtain a legally licensed copy of the Pac-Man intro/loop.
2. Convert it to `OGG`, `FLAC`, or `WAV` (OGG keeps the download small).
3. Save/rename the file to `assets/audio/pacman_theme.ogg` (or the equivalent path next to your executable if you deploy elsewhere).

At runtime the engine streams and loops this file. If the file is missing, the game logs a warning and continues silently.

### Windows audio dependencies

If you see `Failed to open the audio device` even though the soundtrack file is present, Windows cannot find OpenAL (the `openal32.dll` shipped with SFML). Re-run `cmake --build` so the post-build step copies SFML's `extlibs/bin/x64` (or `x86`) directory next to `PacmanGame.exe`, or copy `openal32.dll` manually from `build/_deps/sfml-src/extlibs/bin/<arch>/`. Without that DLL the SFML audio backend cannot initialize and the music stays muted.

## Diagrams Legend
- **Orange** represent structures
- **Blue** represent behaviors/classes
- **Green** represent interfaces
- **Magenta** represent enumerations

