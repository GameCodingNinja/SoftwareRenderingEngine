# Software Rendering Engine - Game Template

## Project Overview
This is a software rendering game engine written in C++26. It performs all rendering in software (no GPU APIs). The shared engine library lives at `../library/`.

## Reference Engines
Consult these sibling engines for design and implementation patterns (but use best judgment):
- `~/Development/SDL2-Vulkan-Game-Engine/gameTemplate/`
- `~/Development/javajcript-webgl-game-engine/gameTemplate/`
- `~/Development/sdl2-opengl-game-engine/gameTemplate/`
- `~/Development/Legacy/TheEarlyYears/Funware Projects/Library/tri3D/`

## Architecture
- **`source/game/`** — Main game loop, entry point (`main.cpp`), game class, and soft shader header.
- **`source/state/`** — Game state machine: `IGameState` interface, `CommonState`, `StartupState`, `TitleScreenState`.
- **`data/`** — Runtime assets: `objects/`, `settings/`, `textures/`.
- **`../library/`** — Shared engine library (2D, 3D, software renderer, sprite, system, utilities, managers, object data, SOIL).

## Build
- CMake ≥ 3.25, C++26, Linux (X11 + Wayland).
- Debug: `mkdir build && cd build && cmake -DCMAKE_BUILD_TYPE=Debug .. && make`
- Release: `mkdir release && cd release && cmake -DCMAKE_BUILD_TYPE=Release .. && make`

## Performance Rules (MUST follow)
These rules apply to all hot pixel loops and rasterizer code:
1. **Use bit shifts instead of division** for power-of-two operations.
2. **Minimize branching and memory access** in inner rasterizer loops.
3. **Use threading and SSE intrinsics (SIMD)** for performance-critical paths.
4. Prefer `__m128`/`__m128i` SSE types and `_mm_*` intrinsics in pixel-processing code.
5. Keep inner loops tight — avoid function calls, virtual dispatch, and heap allocation inside hot paths.

## Code Style
- C++26 standard, no exceptions to this.
- Use `pthread` for threading (per CMake flags).
- Header/source pairs: `foo.h` / `foo.cpp`.
