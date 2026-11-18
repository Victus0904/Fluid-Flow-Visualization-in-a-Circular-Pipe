# Fluid Flow Visualization in a Circular Pipe

[![CI](https://github.com/Victus0904/Fluid-Flow-Visualization-in-a-Circular-Pipe/actions/workflows/ci.yml/badge.svg)](https://github.com/Victus0904/Fluid-Flow-Visualization-in-a-Circular-Pipe/actions/workflows/ci.yml)
![C++17](https://img.shields.io/badge/C%2B%2B-17-00599C)
![SFML](https://img.shields.io/badge/SFML-2.6%20%7C%203.x-8CC445)
[![License: MIT](https://img.shields.io/badge/license-MIT-green)](LICENSE)

A real-time C++/SFML visualization of **fully developed laminar flow** in a circular
pipe. Hundreds of tracer particles move with the local axial velocity of the
Hagen–Poiseuille profile, so the parabolic velocity field shows up directly as the
particles shear past each other.

▶️ **[Watch the demo](docs/demo.mp4)**

## The physics

For steady, incompressible, fully developed laminar flow in a pipe of radius *R*,
the axial velocity depends only on the radial position *r*:

$$u(r) = u_{max}\left(1 - \frac{r^2}{R^2}\right), \qquad u_{max} = 2\,\bar{u}$$

Velocity is zero at the wall (no-slip) and highest on the centreline, where it is
twice the mean velocity $\bar{u}$.

## Features

- **Speed-coloured tracers:** blue near the wall, red on the centreline
- **Live velocity-profile plot** at the inlet, scaled with the current flow rate
- **Interactive controls** for the mean velocity, pause and re-seed
- **Frame-rate independent** motion, with clamped time steps so dragging the window doesn't make particles jump
- **Portable:** builds against SFML 2.6 *and* SFML 3 from the same source

| Key | Action |
|---|---|
| `↑` / `↓` | Increase / decrease mean velocity |
| `Space` | Pause / resume |
| `R` | Re-seed particles |
| `Esc` | Quit |

## Building

Requirements: a C++17 compiler, CMake ≥ 3.16 and [SFML](https://www.sfml-dev.org/download/) 2.6 or 3.x.

```bash
# Ubuntu / Debian
sudo apt-get install libsfml-dev cmake g++
# macOS
brew install sfml cmake

git clone https://github.com/Victus0904/Fluid-Flow-Visualization-in-a-Circular-Pipe.git
cd Fluid-Flow-Visualization-in-a-Circular-Pipe
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
./build/fluid_flow
```

**Windows (MinGW):** download the SFML build that matches your compiler, then point CMake at it:

```powershell
cmake -S . -B build -G "MinGW Makefiles" -DSFML_DIR="C:/SFML/lib/cmake/SFML"
cmake --build build
# copy the SFML DLLs from C:/SFML/bin next to build/fluid_flow.exe, then run it
```

Without CMake:

```bash
g++ -std=c++17 main.cpp -o fluid_flow -lsfml-graphics -lsfml-window -lsfml-system
```

## Project structure

```text
.
├── main.cpp          # Simulation and rendering
├── CMakeLists.txt    # Cross-platform build (SFML 2.6 or 3)
└── docs/demo.mp4     # Screen recording
```

## License

[MIT](LICENSE)
