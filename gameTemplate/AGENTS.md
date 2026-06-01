# Software Rendering Engine - Game Template

## Project Overview
This is a software rendering game engine written in C++26. It performs all rendering in software (no GPU APIs). The shared engine library lives at `../library/`.

## Reference Engines
Consult these sibling engines for design and implementation patterns (but use best judgment):
- `~/Development/SDL2-Vulkan-Game-Engine/gameTemplate/`
- `~/Development/javajcript-webgl-game-engine/gameTemplate/`
- `~/Development/sdl2-opengl-game-engine/gameTemplate/`
- `~/Development/Legacy/TheEarlyYears/Funware Projects/Library/tri3D/`

**Note:** The Vulkan engine and the JavaScript/WebGL engine are basically the same architecture and patterns, just in different languages/APIs. Treat them as one reference design.

## Node & Strategy Architecture
- **CNode** is a composition-based scene graph node (no subclasses). It holds a `std::variant` payload (`CObject`, `CSprite`, or `CUIControl`) and a vector of child nodes. The node system is meant to be used within the strategy system. `UI_CONTROL` is for in-game UI controls attached to sprites (e.g., health bars on ships), not for the menu system.
- **Strategy** is the layer that combines multiple objects, sprites, and in-game UI controls into higher-level constructs. It defines render-time details: position, rotation, scale, scripts. The **object data** layer below it defines creation details: texture, mesh, color, shader, etc. This separation enables reusability — the same object data can be instantiated multiple times with different transforms and behaviors.
- **Menu system** stays separate from the node/strategy system — menus are more straightforward and use the same GUI architecture as the other engines.

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

## Window Resize & Projection
- **Windows are resizable** (stretch, pull, maximize) on all platforms (X11, Wayland, Windows).
- **Resize events** (`EVENT_WINDOW_RESIZE`) are pushed by the window but all resize work (framebuffer resize, settings update, projection rebuild) is deferred to `CDevice::HandleResolutionChange()`, called from `CGame::HandleEvent()` — never during rendering.
- **3D perspective:** Option 2 — framebuffer matches window size, vertical FOV is fixed, horizontal FOV adjusts via aspect ratio in `perspectiveFovRH()`. Objects maintain correct proportions; wider windows show more of the world.
- **2D orthographic:** Framebuffer matches window size but rendering uses an **aspect-locked display rect** centered in the framebuffer. `CSoftwareRender` computes `m_halfDisplay` and `m_displayOffset` from the native aspect ratio (`CSettings::getNativeAspectRatio()`). `render2D` and `renderFixedFunction2D` use these instead of `m_halfScreen`. Extra framebuffer space stays black from the clear.
- **Native aspect ratio** (`m_nativeAspectRatio` in `CSettings`) is set once from the startup resolution and never changes on resize.
- **Projection matrix builders** (`perspectiveFovRH`, `perspectiveFovLH`, `orthographicRH`, `orthographicLH`) call `initilizeMatrix()` first and explicitly set `m_33 = 0` for perspective. They are self-contained — no dependency on prior matrix state.
- **Camera references:** Game states must hold `CCamera &` references to managed cameras, not copies. Copies become stale when `CCameraMgr::rebuildProjectionMatrix()` is called on resize.

## Sound System
- **Location:** `../library/sound/` — entirely custom code, no SDL or SDL_mixer dependency.
- **Design plan:** `../library/sound_plan.md` — phased implementation plan with full architecture details.
- **Architecture:** `CMixEngine` singleton owns a platform audio device (`IAudioDevice`), runs a dedicated audio thread, and mixes through 4 buses (Music, Ambient, Voice, SFX) with per-bus volume/enable and master volume.
- **Audio device backends:** ALSA (Linux), WASAPI (Windows). `audiodevicefactory.cpp` selects via `#ifdef`, same pattern as `windowfactory.cpp`. On Linux, the ALSA backend tries devices in order: `pipewire` → `pulse` → `default` → `plughw:0,0` (PipeWire/PulseAudio systems lack a working `"default"` device).
- **Audio thread:** Spawned by `IAudioDevice::open()`. Runs a wait→mix→write loop. Uses `snd_pcm_wait` with 100ms timeout (ALSA) or event-driven `WaitForSingleObject` (WASAPI) for clean shutdown. Thread priority is elevated (`SCHED_FIFO` on Linux, MMCSS `"Pro Audio"` on Windows).
- **Mixing:** All mixing is done in F32 internally. The audio device converts to native format (S16LE for ALSA, F32 with AUTOCONVERTPCM for WASAPI). Inner mix loops use SSE intrinsics (`_mm_mul_ps`, `_mm_add_ps`, `_mm_min_ps`, `_mm_max_ps`) for volume-weighted mixing and output clamping.
- **Codecs:** WAV (built-in), OGG (stb_vorbis), MP3 (minimp3), FLAC (dr_flac) — all header-only, bundled. `NCodecFactory::load()` auto-detects format by magic bytes.
- **Manager layer:** `CSoundMgr` inherits `CManagerBase`, mirrors `SDL3-Vulkan-Game-Engine/library/sound/` organization. XML-driven group loading, sound IDs, playlist IDs. `CSound` wraps PCM/stream data, `CPlayList` provides random/sequential playback with anti-repeat shuffle.
- **Shutdown order:** `CMixEngine::shutdown()` must clear play state under the mutex **before** closing the device/joining the audio thread. Sound data (`SWavData`) pointed to by the mix engine may already be freed by the time the singleton destructor runs during `atexit`. Always null the pointer first.
- **Lifetime rule:** Any `SWavData` passed to `CMixEngine::playSound()` must outlive playback — the engine stores a raw pointer. In Phase 4, `CSoundMgr` owns all sound data so game states don't manage lifetimes.
- **Volume hierarchy:** `final = sound_vol × bus_vol × master_vol × bus_enabled`. Each bus can be independently disabled from settings.
- **Looping:** `loopCount`: `0` = play once, `N` = repeat N times, `-1` = infinite until stopped.
- **Game integration:** `CSoundMgr::Instance().init()` is called in `CGame::CGame()` constructor. `CSoundMgr::Instance().cleanup()` is called in `CGame::~CGame()` destructor. `LoadListTable` and `loadGroup` are called in `CStartUpState::Load()`. Sound data lives in `data/sound/`.

## Fixed-Function Pipeline
Ignore the fixed function rendering pipeline for development. It's just there for speed comparisons.

## Code Style
- C++26 standard, no exceptions to this.
- Use `pthread` for threading (per CMake flags).
- Header/source pairs: `foo.h` / `foo.cpp`.
