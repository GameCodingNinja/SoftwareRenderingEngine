# Native Linux Window & Framebuffer — Implementation Plan

## Overview

Replace SDL window/surface rendering with native Linux display server code.
Wayland is tried first; X11 is the fallback. Window management and framebuffer
(pixel buffer) are separated into their own class hierarchies so each concern
is independently extensible.

SDL remains for: audio, gamepad, timer, and the event queue. The native window
classes translate their keyboard/mouse/window events into `SDL_Event` via
`SDL_PushEvent()` so existing game code is unaffected.

---

## Architecture

```
    ╭──────────────╮           ╭───────────────╮
    │   IWindow    │           │  IFrameBuffer │
    │──────────────│           │───────────────│
    │ Create()     │           │ GetPixels()   │
    │ Destroy()    │           │ GetWidth()    │
    │ Show()       │           │ GetHeight()   │
    │ SetTitle()   │           │ Clear()       │
    │ SetFullScreen│           │ Flip()        │
    │ PollEvents() │           ╰───────┬───────╯
    │ GetFrameBuf()│                   │
    ╰──────┬───────╯       ┌───────────┴───────────┐
           │               │                       │
   ┌───────┴───────┐   ╭──────────────╮   ╭──────────────╮
   │               │   │CWaylandFrame │   │ CX11Frame    │
╭──────────╮ ╭─────────╮│   Buffer     │   │   Buffer     │
│CWayland  │ │ CX11    ││ (wl_shm)    │   │ (XImage)     │
│ Window   │ │ Window  │╰──────────────╯   ╰──────────────╯
│(creates &│ │(creates&│
│ owns its │ │ owns its│
│ framebuf)│ │ framebuf)│
╰──────────╯ ╰─────────╯

    Factory: CreateNativeWindow() → tries Wayland, falls back to X11
             Returns unique_ptr<IWindow>
```

### New Files

| File | Description |
|------|-------------|
| `library/system/iwindow.h` | Pure virtual window interface |
| `library/system/iframebuffer.h` | Pure virtual framebuffer interface |
| `library/system/x11window.h` | X11 window header |
| `library/system/x11window.cpp` | X11 window implementation |
| `library/system/x11framebuffer.h` | X11 framebuffer header |
| `library/system/x11framebuffer.cpp` | X11 framebuffer implementation |
| `library/system/waylandwindow.h` | Wayland window header |
| `library/system/waylandwindow.cpp` | Wayland window implementation |
| `library/system/waylandframebuffer.h` | Wayland framebuffer header |
| `library/system/waylandframebuffer.cpp` | Wayland framebuffer implementation |
| `library/system/windowfactory.h` | Factory function header |
| `library/system/windowfactory.cpp` | Factory: try Wayland → X11 |

### Modified Files

| File | Change |
|------|--------|
| `library/softwareRender/renderdefs.h` | Replace `SDL_Surface*` with `CSurfaceData*` struct |
| `library/softwareRender/softwareRender.h` | Replace `SDL_Surface*` with `CSurfaceData` member |
| `library/softwareRender/softwareRender.cpp` | Use `CSurfaceData` for pixels/dimensions |
| `library/system/device.h` | Replace `SDL_Window*` with `std::unique_ptr<IWindow>` |
| `library/system/device.cpp` | Create IWindow via factory, delegate Show/FullScreen/etc. |
| `library/system/basegame.h` | Replace `SDL_Window*` with `IWindow*`/`IFrameBuffer*` |
| `library/system/basegame.cpp` | Use IWindow/IFrameBuffer for flip/clear/events, init SDL without video |
| `library/utilities/statcounter.h` | Replace `SDL_Window*` param with `IWindow*` |
| `library/utilities/statcounter.cpp` | Use `IWindow::SetTitle()` |
| `library/CMakeLists.txt` | Add new source files, link X11/Wayland libs |
| `gameTemplate/CMakeLists.txt` | Add X11 and Wayland find_package |

---

## Acceptance Criteria

1. **Wayland path works**: On a Wayland session, the engine creates a native
   Wayland window with a `wl_shm`-backed pixel buffer, renders triangles to
   it, and displays them correctly.

2. **X11 fallback works**: When Wayland is unavailable (e.g., `WAYLAND_DISPLAY`
   unset, or running in a pure X11 session), the engine falls back to X11
   and renders identically.

3. **Separation of concerns**: Window management (create, show, title, events)
   is handled by `IWindow` implementations. Pixel buffer management (pixels,
   clear, flip) is handled by `IFrameBuffer` implementations. Each window
   class creates and owns its corresponding framebuffer.

4. **Event translation**: Keyboard (key down/up), mouse (button, motion),
   and window close events from the native window are translated to
   `SDL_Event` and dispatched via `SDL_PushEvent()`. Existing game menu
   navigation, quit handling, and input all work unchanged.

5. **Framebuffer operations**: `Clear()` zeros the pixel buffer. `Flip()`
   displays the current buffer contents in the window.

6. **Window operations**: `Show()`/`SetTitle()`/`SetFullScreen()` work on
   both backends.

7. **No SDL video dependency**: `SDL_Init()` no longer includes
   `SDL_INIT_VIDEO`. SDL is only used for audio, gamepad, timer, and the
   event queue.

8. **No rendering regressions**: The software-rendered output (textured
   triangles, menus, sprites) looks identical to before the change.

9. **Clean build**: Zero errors, zero new warnings.

10. **Existing game code unchanged**: No modifications to game state files,
    smart GUI files, or menu handling code. The interface change is confined
    to the library layer.

---

## Phases

### Phase 1: Interfaces & Surface Data Struct

**Goal**: Define the `IWindow` and `IFrameBuffer` interfaces and the
`CSurfaceData` struct that replaces `SDL_Surface*` in the render pipeline.

**Tasks**:
- Create `library/system/iwindow.h` with pure virtual interface:
  ```cpp
  class IWindow
  {
  public:
      virtual ~IWindow() = default;

      // Create the window
      virtual void Create(int width, int height, const char* title = "") = 0;

      // Destroy the window
      virtual void Destroy() = 0;

      // Show or hide the window
      virtual void Show(bool visible) = 0;

      // Set the window title
      virtual void SetTitle(const std::string& title) = 0;

      // Set full screen mode
      virtual void SetFullScreen(bool fullscreen) = 0;

      // Poll and translate native events to SDL events
      virtual void PollEvents() = 0;

      // Get the framebuffer owned by this window
      virtual IFrameBuffer* GetFrameBuffer() = 0;
  };
  ```
- Create `library/system/iframebuffer.h` with pure virtual interface:
  ```cpp
  class IFrameBuffer
  {
  public:
      virtual ~IFrameBuffer() = default;

      // Get the raw pixel buffer (ARGB/XRGB 32-bit)
      virtual uint32_t* GetPixels() = 0;

      // Get framebuffer dimensions
      virtual int GetWidth() const = 0;
      virtual int GetHeight() const = 0;

      // Clear the pixel buffer to zero (black)
      virtual void Clear() = 0;

      // Display the pixel buffer contents in the window
      virtual void Flip() = 0;
  };
  ```
- Create `CSurfaceData` struct in `library/softwareRender/renderdefs.h`:
  ```cpp
  struct CSurfaceData
  {
      uint32_t* pixels = nullptr;
      int w = 0;
      int h = 0;
  };
  ```
- Update `CRender2d` in `renderdefs.h` to use `CSurfaceData*` instead of
  `SDL_Surface*`

**Deliverable**: Header files compile. No functional changes yet.

**Review checkpoint**: User reviews the interface and struct design before
proceeding.

---

### Phase 2: X11 Backend

**Goal**: Implement the X11 window and framebuffer classes.

**Tasks — CX11FrameBuffer**:
- Create `library/system/x11framebuffer.h` and `x11framebuffer.cpp`
- Constructor takes X11 `Display*`, `Window`, `GC`, width, height
- Allocates `uint32_t[w*h]` pixel buffer
- Creates `XImage` wrapping the pixel buffer (depth 24/32, ZPixmap)
- `GetPixels()`: returns the pixel buffer pointer
- `GetWidth()`/`GetHeight()`: return dimensions
- `Clear()`: `memset(pixels, 0, w * h * 4)`
- `Flip()`: calls `XPutImage` to blit the XImage to the window
- Destructor: frees XImage and pixel buffer

**Tasks — CX11Window**:
- Create `library/system/x11window.h` and `x11window.cpp`
- `Create()`:
  - `XOpenDisplay(NULL)` to connect
  - `XCreateWindow` with 32-bit depth visual
  - `XCreateGC` for the graphics context
  - Set `WM_DELETE_WINDOW` protocol for clean close
  - Set `KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask |
    PointerMotionMask | StructureNotifyMask` event mask
  - Instantiate `CX11FrameBuffer` with display/window/gc/dimensions
- `Destroy()`: destroy framebuffer, free GC, destroy window, close display
- `Show()`: `XMapWindow` / `XUnmapWindow`
- `SetTitle()`: `XStoreName`
- `SetFullScreen()`: send `_NET_WM_STATE_FULLSCREEN` via `XSendEvent`
- `PollEvents()`: while `XPending` > 0, `XNextEvent`, translate to SDL:
  - `KeyPress`/`KeyRelease` → `SDL_KEYDOWN`/`SDL_KEYUP` (use `XLookupKeysym`)
  - `ButtonPress`/`ButtonRelease` → `SDL_MOUSEBUTTONDOWN`/`SDL_MOUSEBUTTONUP`
  - `MotionNotify` → `SDL_MOUSEMOTION`
  - `ClientMessage(WM_DELETE_WINDOW)` → `SDL_QUIT`
- `GetFrameBuffer()`: returns pointer to owned `CX11FrameBuffer`

**Deliverable**: X11 backend compiles and can be instantiated standalone.

**Review checkpoint**: User reviews X11 implementation before proceeding.

---

### Phase 3: Wayland Backend

**Goal**: Implement the Wayland window and framebuffer classes.

**Tasks — CWaylandFrameBuffer**:
- Create `library/system/waylandframebuffer.h` and `waylandframebuffer.cpp`
- Constructor takes Wayland `wl_surface*`, width, height
- Creates shared memory buffer:
  - `shm_open` + `ftruncate` to create a shared memory file
  - `mmap` to map it into process memory
  - `wl_shm_pool_create` → `wl_shm_pool_create_buffer` (format `WL_SHM_FORMAT_XRGB8888`)
- `GetPixels()`: returns the mmap'd pixel pointer (cast to `uint32_t*`)
- `GetWidth()`/`GetHeight()`: return dimensions
- `Clear()`: `memset(pixels, 0, w * h * 4)`
- `Flip()`:
  - `wl_surface_attach(surface, buffer, 0, 0)`
  - `wl_surface_damage_buffer(surface, 0, 0, w, h)`
  - `wl_surface_commit(surface)`
- Destructor: destroy wl_buffer, wl_shm_pool, munmap, close shm fd

**Tasks — CWaylandWindow**:
- Create `library/system/waylandwindow.h` and `waylandwindow.cpp`
- `Create()`:
  - `wl_display_connect(NULL)` to connect
  - Bind globals via `wl_registry`: `wl_compositor`, `wl_shm`, `wl_seat`,
    `xdg_wm_base`
  - Create `wl_surface` via `wl_compositor_create_surface`
  - Create `xdg_surface` + `xdg_toplevel` for window decoration
  - Set `xdg_toplevel` title
  - Handle `xdg_toplevel::close` → `SDL_QUIT`
  - Handle `xdg_surface::configure` → ack configure
  - Bind `wl_keyboard` and `wl_pointer` from `wl_seat` for input
  - Instantiate `CWaylandFrameBuffer` with wl_surface/dimensions
- `Destroy()`: destroy toplevel, xdg_surface, wl_surface, seat listeners,
  framebuffer, disconnect display
- `Show()`: commit surface (Wayland shows on first commit), or create/destroy
  surface to hide
- `SetTitle()`: `xdg_toplevel_set_title`
- `SetFullScreen()`: `xdg_toplevel_set_fullscreen` / `xdg_toplevel_unset_fullscreen`
- `PollEvents()`: `wl_display_dispatch_pending()`, translate events:
  - `wl_keyboard::key` → `SDL_KEYDOWN`/`SDL_KEYUP`
  - `wl_pointer::button` → `SDL_MOUSEBUTTONDOWN`/`SDL_MOUSEBUTTONUP`
  - `wl_pointer::motion` → `SDL_MOUSEMOTION`
  - `xdg_toplevel::close` → `SDL_QUIT`
- `GetFrameBuffer()`: returns pointer to owned `CWaylandFrameBuffer`

**Deliverable**: Wayland backend compiles and can be instantiated standalone.

**Review checkpoint**: User reviews Wayland implementation before proceeding.

---

### Phase 4: Factory & Integration

**Goal**: Wire the backends into the engine, replacing SDL window/surface code.

**Tasks — Factory**:
- Create `library/system/windowfactory.h` and `windowfactory.cpp`:
  ```cpp
  std::unique_ptr<IWindow> CreateNativeWindow();
  ```
  - Try `wl_display_connect(NULL)`:
    - If succeeds → disconnect, return `std::make_unique<CWaylandWindow>()`
    - If fails → try `XOpenDisplay(NULL)`:
      - If succeeds → close, return `std::make_unique<CX11Window>()`
      - If fails → throw exception

**Tasks — CDevice updates**:
- Replace `SDL_Window* m_pWindow` with `std::unique_ptr<IWindow> m_upWindow`
- `Create()`:
  - Call `CreateNativeWindow()` to get `IWindow`
  - Call `m_upWindow->Create(w, h, title)`
  - Remove `SDL_CreateWindow` and `SDL_GetWindowSurface` calls
  - Remove `CSoftwareRender::Instance().CreateSurface(m_pWindow)` — instead,
    call `CSoftwareRender::Instance().SetSurface(m_upWindow->GetFrameBuffer())`
- `ShowWindow()` → `m_upWindow->Show(visible)`
- `SetFullScreen()` → `m_upWindow->SetFullScreen(fullscreen)`
- Remove `GetWindow()` and `GetContext()`
- Add `IWindow* GetNativeWindow()` accessor
- Add `IFrameBuffer* GetFrameBuffer()` convenience accessor

**Tasks — CBaseGame updates**:
- Replace `SDL_Window* m_pWindow` with `IWindow*` and `IFrameBuffer*` cached
  from device
- `Init()`:
  - Change `SDL_Init()` to exclude `SDL_INIT_VIDEO`
  - Use `IFrameBuffer::Clear()` and `IFrameBuffer::Flip()`
- `Render()`:
  - `IFrameBuffer::Clear()` instead of `CSoftwareRender::Instance().Clear()`
  - `IFrameBuffer::Flip()` instead of `CSoftwareRender::Instance().Flip(m_pWindow)`
- `PollEvents()`:
  - Call `IWindow::PollEvents()` before `SDL_PollEvent()` to translate
    native events into SDL events
- `DisplayErrorMsg()`: use `fprintf(stderr, ...)` instead of
  `SDL_ShowSimpleMessageBox` (since SDL_INIT_VIDEO is removed)

**Tasks — CSoftwareRender updates**:
- Remove `SDL_Surface* m_pSurface` member
- Add `CSurfaceData m_surfaceData` member
- Remove `CreateSurface(SDL_Window*)`, add `SetSurface(IFrameBuffer*)`:
  ```cpp
  void SetSurface(IFrameBuffer* pFB) {
      m_surfaceData.pixels = pFB->GetPixels();
      m_surfaceData.w = pFB->GetWidth();
      m_surfaceData.h = pFB->GetHeight();
      m_halfScreen.w = m_surfaceData.w / 2;
      m_halfScreen.h = m_surfaceData.h / 2;
  }
  ```
- Remove `GetSurface()`, `Flip()`, `Clear()` (now on IFrameBuffer/IWindow)
- `Render()`: use `m_surfaceData.w/h` for screen dimensions, pass
  `&m_surfaceData` to `CRender2d`

**Tasks — CRender2d / RenderTri updates**:
- `CRender2d` constructor takes `CSRTexture*, CSurfaceData*` instead of
  `CSRTexture*, SDL_Surface*`
- `RenderTri()`: access `pRender->m_pSurface->w`, `->h`, `->pixels`
  (now CSurfaceData fields instead of SDL_Surface fields — same names, same
  types, so most of the rasterizer code is unchanged)

**Tasks — CStatCounter updates**:
- Replace `SDL_Window*` parameter with `IWindow*`
- Use `pWindow->SetTitle(statStr)` instead of `SDL_SetWindowTitle`

**Tasks — Build system updates**:
- `library/CMakeLists.txt`: add new .cpp files to `add_library`
- `gameTemplate/CMakeLists.txt`:
  - `find_package(X11 REQUIRED)`
  - `pkg_search_module(WAYLAND_CLIENT REQUIRED wayland-client)`
  - Link `${X11_LIBRARIES}`, `${WAYLAND_CLIENT_LIBRARIES}`
  - Add wayland-protocols for xdg-shell header generation

**Deliverable**: Full build succeeds, game runs with native window.

**Review checkpoint**: User reviews integration before testing phase.

---

### Phase 5: Testing & Polish

**Goal**: Verify everything works correctly on both backends.

**Tasks**:
- Run automated tests (see test plan below)
- Run manual tests (see test plan below)
- Fix any issues found
- Remove any leftover SDL video references (dead includes, commented code)
- Clean up includes
- Verify no new warnings

**Deliverable**: All tests pass, game runs identically to before.

**Review checkpoint**: User does final review.

---

## Test Plan

### Automated Tests

| # | Test | Method | Expected Result |
|---|------|--------|-----------------|
| A1 | Factory selects Wayland on Wayland session | Run with `WAYLAND_DISPLAY` set, log backend type | `CWaylandWindow` created |
| A2 | Factory falls back to X11 | Run with `WAYLAND_DISPLAY` unset, `DISPLAY` set | `CX11Window` created |
| A3 | Factory fails gracefully | Run with both env vars unset | Throws exception, doesn't crash |
| A4 | IFrameBuffer pixel write | Create FB, write known pattern, verify buffer | Pixels match expected pattern |
| A5 | Clear zeroes buffer | Create FB, write pixels, Clear(), verify all zero | All pixels are 0 |
| A6 | IWindow/IFrameBuffer separation | Verify GetFrameBuffer() returns valid IFrameBuffer* | Non-null, correct dimensions |
| A7 | Build clean | `cmake --build . 2>&1 \| grep error` | Zero errors |
| A8 | No new warnings | `cmake --build . 2>&1 \| grep warning` | No new warnings vs. baseline |
| A9 | No SDL video init | Grep SDL_Init call, confirm no SDL_INIT_VIDEO | Confirmed |

### Manual Tests

| # | Test | Steps | Expected Result |
|---|------|-------|-----------------|
| M1 | Window appears | Launch game | Window appears with correct size from settings |
| M2 | Window title | Launch game, wait for FPS counter | Title bar shows FPS stats |
| M3 | Rendering correct | Navigate to title screen | Sprites/text render identically to SDL version |
| M4 | Keyboard input | Press arrow keys, Enter, Escape in menus | Menu navigation works |
| M5 | Mouse input | Click menu buttons, move mouse | Buttons highlight and activate |
| M6 | Window close | Click the X button | Game exits cleanly |
| M7 | Full screen | Enable full screen in settings | Window goes full screen |
| M8 | Gamepad | Connect gamepad, navigate menus | Gamepad input works (SDL handles this) |
| M9 | X11 fallback | `unset WAYLAND_DISPLAY && ./gametemplate` | Game runs via X11/XWayland |
| M10 | Performance | Run game, check FPS counter | FPS comparable to SDL version |
| M11 | Clean exit | Exit game, check console for errors | No segfaults, no protocol errors |

---

## Open Questions

1. **Keysym mapping depth**: For event translation, how complete should the
   X11/Wayland → SDL keysym mapping be? Full keyboard coverage (all keys
   including function keys, numpad, media keys)? Or just the keys the game
   currently uses (arrows, Enter, Escape, alphanumerics)?

2. **Double buffering**: Should the framebuffer implement double buffering
   (two pixel buffers, swap on Flip) for tear-free rendering? Or is single
   buffer acceptable since the current SDL path uses single-buffer
   `SDL_UpdateWindowSurface`?

3. **Window resize**: The current engine doesn't appear to support runtime
   window resizing (size is set at startup from settings.xml). Should the
   native window be created as non-resizable? Or should we handle resize
   events and reallocate the framebuffer?

4. **MIT-SHM for X11**: Should the X11 framebuffer use MIT Shared Memory
   extension (`XShmCreateImage`/`XShmPutImage`) for faster blitting, as
   SDL does? Or is regular `XCreateImage`/`XPutImage` sufficient?

5. **Wayland xdg-shell version**: Wayland requires `xdg_wm_base` (stable)
   or `wl_shell` (deprecated) for window management. Should we only
   support `xdg_wm_base` (modern compositors) or also fall back to
   `wl_shell` for older setups?

6. **Cursor visibility**: The current engine doesn't appear to hide the
   cursor. Should the native window implementations manage cursor
   visibility, or leave it as the system default?

7. **Error message dialogs**: `CBaseGame::DisplayErrorMsg()` currently uses
   `SDL_ShowSimpleMessageBox()`. With `SDL_INIT_VIDEO` removed, this won't
   work. Should we replace it with `fprintf(stderr, ...)`, or keep
   `SDL_INIT_VIDEO` just for this edge case?

8. **SDL_INIT_VIDEO removal scope**: Some SDL functions used in the
   codebase (like `SDL_GetNumVideoDisplays`, `SDL_GetDisplayMode` in
   `smartresolutionbtn.cpp`) require `SDL_INIT_VIDEO`. Should we keep
   `SDL_INIT_VIDEO` initialized but just not use SDL for the window/
   surface? Or replace those SDL display-query calls with native ones too?
