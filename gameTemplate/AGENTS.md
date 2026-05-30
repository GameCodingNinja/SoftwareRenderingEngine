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

## Lighting
- **Shading models:** Gouraud (per-vertex) and Phong (per-pixel), selected via compile-time define in `../library/common/lightdefs.h`. Only one can be active — they are mutually exclusive.
- **Light types:** Ambient, Directional, Point, Spot. Defined in `../library/common/light.h`. Spot lights use `m_innerCone`/`m_outerCone` (cosines of half-angles) for smooth cone falloff.
- **Normal transformation:** Normals are transformed into world space using only the model matrix (no view/projection) so lighting is correct regardless of camera position/rotation.
- **Gouraud:** Lighting is computed per-vertex in `render3D()`, the resulting color is perspective-divided and interpolated across the triangle via `CTriangleSlope`. The rasterizer recovers the color per-pixel using affine subdivision (16-pixel runs) with 8.16 fixed-point precision.
- **Phong:** The world-space normal (divided by W) is interpolated per-pixel. At each pixel the normal is recovered via perspective divide, normalized, and full lighting is computed via `computeVertexLighting()`. Point lights are not yet wired up for Phong (zero position is passed).
- **`CVertex` layout must not change** between shading models. Phong stores extra per-triangle data on `CRender3d` (not in `CVertex`) to keep the vertex buffer layout stable.
- **Render interface:** `CSprite::render(const CCamera &)` passes the sprite's model matrix and camera to the visual component. `CVisualComponent3d` builds the MVP matrix and world matrix internally.
- **Light loading:** `CLightMgr` inherits from `CManagerBase` and follows the same `LoadListTable`/`LoadGroup` pattern as object data. XML files use `<lightList>` as the root element. Data lives in `data/objects/lightDataList/`.
- **Light binding:** `CSprite::setLights()` stores a pointer on `CVisualComponent3d`. The visual component passes it to `render3D()` which reads it via `getLights()`. If null, renderer defaults are used. **The light vector must outlive the sprite** — use `const auto &` (not `auto`) when capturing from `CLightMgr::get()` to avoid dangling pointers.

## Fixed-Function Pipeline
Ignore the fixed function rendering pipeline for development. It's just there for speed comparisons.

## Code Style
- C++26 standard, no exceptions to this.
- Use `pthread` for threading (per CMake flags).
- Header/source pairs: `foo.h` / `foo.cpp`.
