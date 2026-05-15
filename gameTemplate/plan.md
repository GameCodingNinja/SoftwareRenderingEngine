# Native Window & Framebuffer — Implementation Plan

## Overview

Replace SDL window/surface rendering with native platform code. On Linux,
Wayland is tried first with X11 as the fallback. On Windows, the Win32 API
is used directly. Window management and framebuffer (pixel buffer) are
separated into their own class hierarchies so each concern is independently
extensible.

SDL is being fully removed from the project. The native window classes
use a custom event system to replace SDL events.

---

## Architecture

```
    ╭──────────────╮   ╭───────────────╮   ╭───────────────╮
    │   IWindow    │   │  IFrameBuffer │   │  CEventQueue  │
    │──────────────│   │───────────────│   │───────────────│
    │ Create()     │   │ GetPixels()   │   │ PollEvent()   │
    │ Destroy()    │   │ GetWidth()    │   │ PushEvent()   │
    │ Show()       │   │ GetHeight()   │   │ Clear()       │
    │ SetTitle()   │   │ Clear()       │   ╰───────────────╯
    │ SetFullScreen│   │ Flip()        │        ▲
    │ PollEvents() │   ╰───────┬───────╯        │
    │ GetFrameBuf()│           │           ╭─────────╮
    ╰──────┬───────╯   ┌───────┼───────┐   │ CEvent  │
           │           │       │       │   │─────────│
   ┌───────┼───────┐   │       │       │   │ type    │
   │       │       │   │       │       │   │ code    │
╭──────╮╭──────╮╭──────╮│       │       │   │ x, y    │
│CWayl-││CX11 ││CWin- ││       │       │   ╰─────────╯
│and   ││Window││dows  ││       │       │
│Window││      ││Window││       │       │
╰──────╯╰──────╯╰──────╯│       │       │
                   ╭───────────╮╭────────╮╭──────────╮
                   │CWayland   ││CX11    ││CWindows  │
                   │FrameBuffer││FrameBuf││FrameBuf  │
                   │(wl_shm)   ││(XImage)││(DIB/GDI) │
                   ╰───────────╯╰────────╯╰──────────╯

    IWindow::PollEvents() translates native events → CEvent
    and pushes them into CEventQueue.

    Factory: CreateNativeWindow()
      Linux:   tries Wayland → falls back to X11
      Windows: creates CWindowsWindow
      Returns unique_ptr<IWindow>
```

### New Files

| File | Platform | Description |
|------|----------|-------------|
| `library/system/iwindow.h` | All | Pure virtual window interface |
| `library/system/iframebuffer.h` | All | Pure virtual framebuffer interface |
| `library/system/event.h` | All | `CEvent` struct and `EEventType` enum |
| `library/system/eventqueue.h` | All | `CEventQueue` singleton — thread-safe event queue |
| `library/system/eventqueue.cpp` | All | `CEventQueue` implementation |
| `library/system/x11window.h` | Linux | X11 window header |
| `library/system/x11window.cpp` | Linux | X11 window implementation |
| `library/system/x11framebuffer.h` | Linux | X11 framebuffer header |
| `library/system/x11framebuffer.cpp` | Linux | X11 framebuffer implementation |
| `library/system/waylandwindow.h` | Linux | Wayland window header |
| `library/system/waylandwindow.cpp` | Linux | Wayland window implementation |
| `library/system/waylandframebuffer.h` | Linux | Wayland framebuffer header |
| `library/system/waylandframebuffer.cpp` | Linux | Wayland framebuffer implementation |
| `library/system/windowswindow.h` | Windows | Win32 window header |
| `library/system/windowswindow.cpp` | Windows | Win32 window implementation |
| `library/system/windowsframebuffer.h` | Windows | Win32 framebuffer header |
| `library/system/windowsframebuffer.cpp` | Windows | Win32 framebuffer implementation |
| `library/system/windowfactory.h` | All | Factory function header |
| `library/system/windowfactory.cpp` | All | Factory: platform detection → backend |

### Modified Files

| File | Change |
|------|--------|
| `library/softwareRender/renderdefs.h` | Replace `SDL_Surface*` with `CSurfaceData*` struct |
| `library/softwareRender/softwareRender.h` | Replace `SDL_Surface*` with `CSurfaceData` member |
| `library/softwareRender/softwareRender.cpp` | Use `CSurfaceData` for pixels/dimensions, remove `#include <SDL.h>` |
| `library/system/device.h` | Replace `SDL_Window*` with `std::unique_ptr<IWindow>`, remove `#include <SDL.h>` |
| `library/system/device.cpp` | Create IWindow via factory, delegate Show/FullScreen/etc., remove SDL |
| `library/system/basegame.h` | Replace `SDL_Window*`/`SDL_Event` with `IWindow*`/`IFrameBuffer*`/`CEvent`, remove `#include <SDL.h>` |
| `library/system/basegame.cpp` | Use IWindow/IFrameBuffer/CEventQueue for flip/clear/events, remove SDL |
| `library/utilities/statcounter.h` | Replace `SDL_Window*` param with `IWindow*` |
| `library/utilities/statcounter.cpp` | Use `IWindow::SetTitle()`, remove `#include <SDL.h>` |
| `library/utilities/genfunc.h/cpp` | Replace `DispatchEvent` SDL impl with `CEventQueue::PushEvent` |
| `gameTemplate/source/game/game.h/cpp` | Replace `SDL_Event` with `CEvent`, `SDL_QUIT` with `EEventType::QUIT` |
| `gameTemplate/source/state/igamestate.h` | Replace `SDL_Event` with `CEvent` in `HandleEvent` |
| `gameTemplate/source/state/commonstate.h/cpp` | Replace `SDL_Event` with `CEvent`, remove `#include <SDL.h>` |
| `gameTemplate/source/state/titlescreenstate.h/cpp` | Replace `SDL_Event` with `CEvent`, remove `#include <SDL.h>` |
| `library/CMakeLists.txt` | Add new source files, conditional platform libs |
| `gameTemplate/CMakeLists.txt` | Remove SDL, platform-conditional find_package and linking |

---

## Acceptance Criteria

1. **Wayland path works**: On a Wayland session, the engine creates a native
   Wayland window with a `wl_shm`-backed pixel buffer, renders triangles to
   it, and displays them correctly.

2. **X11 fallback works**: When Wayland is unavailable (e.g., `WAYLAND_DISPLAY`
   unset, or running in a pure X11 session), the engine falls back to X11
   and renders identically.

3. **Windows path works**: On Windows, the engine creates a Win32 window with
   a DIB section pixel buffer and renders correctly using `StretchDIBits`
   or `BitBlt`.

4. **Separation of concerns**: Window management (create, show, title, events)
   is handled by `IWindow` implementations. Pixel buffer management (pixels,
   clear, flip) is handled by `IFrameBuffer` implementations. Each window
   class creates and owns its corresponding framebuffer.

5. **Event translation**: Keyboard (key down/up), mouse (button, motion),
   and window close events from the native window are translated to the
   engine's event system. Window close produces a quit event that the
   game loop handles to exit cleanly.

6. **Framebuffer operations**: `Clear()` zeros the pixel buffer. `Flip()`
   displays the current buffer contents in the window.

7. **Window operations**: `Show()`/`SetTitle()`/`SetFullScreen()` work on
   all backends.

8. **No SDL dependency**: SDL is fully removed from the project. No SDL
   includes, no SDL linking, no SDL initialization.

9. **No rendering regressions**: The software-rendered output (textured
   triangles, sprites) looks identical to before the change.

10. **Clean build**: Zero errors, zero new warnings on both Linux and Windows.

11. **Existing game code unchanged**: No modifications to game state files
    or game state code. The interface change is confined to the library
    layer.

---

## Phases

### Phase 1: Interfaces, Event System & Surface Data Struct

**Goal**: Define the `IWindow` and `IFrameBuffer` interfaces, the custom
event system that replaces `SDL_Event`, and the `CSurfaceData` struct that
replaces `SDL_Surface*` in the render pipeline.

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

      // Poll native events and push them to the event queue
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

      // Get the raw pixel buffer (XRGB 32-bit)
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
- Create `library/system/event.h` — custom event types and structures,
  modeled after SDL's union-based design but simplified to only what the
  engine needs:
  ```cpp
  // Event type enum (inspired by SDL_EventType, using hex ranges for grouping)
  enum EEventType : uint32_t
  {
      EVENT_NONE          = 0,

      // Application events (0x100 range)
      EVENT_QUIT          = 0x100,

      // Keyboard events (0x300 range)
      EVENT_KEY_DOWN      = 0x300,
      EVENT_KEY_UP        = 0x301,

      // Mouse events (0x400 range)
      EVENT_MOUSE_MOTION      = 0x400,
      EVENT_MOUSE_BUTTON_DOWN = 0x401,
      EVENT_MOUSE_BUTTON_UP   = 0x402,

      // User events (0x8000 range)
      EVENT_USER          = 0x8000,
  };

  // Key event data (modeled after SDL_KeyboardEvent)
  struct CKeyEvent
  {
      EEventType type;        // EVENT_KEY_DOWN or EVENT_KEY_UP
      int keyCode;            // Platform-independent key code
      bool repeat;            // true if this is a key repeat
  };

  // Mouse motion event data (modeled after SDL_MouseMotionEvent)
  struct CMouseMotionEvent
  {
      EEventType type;        // EVENT_MOUSE_MOTION
      int x;                  // X coordinate, relative to window
      int y;                  // Y coordinate, relative to window
      int xrel;               // Relative motion in X
      int yrel;               // Relative motion in Y
  };

  // Mouse button event data (modeled after SDL_MouseButtonEvent)
  struct CMouseButtonEvent
  {
      EEventType type;        // EVENT_MOUSE_BUTTON_DOWN or EVENT_MOUSE_BUTTON_UP
      uint8_t button;         // Button index (1=left, 2=middle, 3=right)
      int x;                  // X coordinate, relative to window
      int y;                  // Y coordinate, relative to window
  };

  // User-defined event data (modeled after SDL_UserEvent)
  struct CUserEvent
  {
      EEventType type;        // EVENT_USER or higher
      int code;               // User-defined event code
      void* data1;            // User-defined data pointer
      void* data2;            // User-defined data pointer
  };

  // Union of all event types (modeled after SDL_Event union)
  union CEvent
  {
      EEventType type;                // Event type, shared with all events
      CKeyEvent key;                  // Keyboard event data
      CMouseMotionEvent motion;       // Mouse motion event data
      CMouseButtonEvent button;       // Mouse button event data
      CUserEvent user;                // User-defined event data
  };
  ```
- Create `library/system/eventqueue.h` and `eventqueue.cpp` — thread-safe
  event queue singleton (modeled after SDL's internal event queue):
  ```cpp
  class CEventQueue
  {
  public:
      static CEventQueue& Instance()
      {
          static CEventQueue eventQueue;
          return eventQueue;
      }

      // Push an event onto the queue (thread-safe)
      void PushEvent(const CEvent& event);

      // Pop the next event from the queue
      // Returns true if an event was available, false if queue is empty
      bool PollEvent(CEvent& event);

      // Clear all pending events
      void Clear();

  private:
      CEventQueue() = default;

      std::queue<CEvent> m_eventQueue;
      std::mutex m_mutex;
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

**Deliverable**: Header files and event queue compile. No functional changes yet.

**Review checkpoint**: User reviews the interface, event, and struct design
before proceeding.

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
- `PollEvents()`: while `XPending` > 0, `XNextEvent`, translate to engine
  events:
  - `KeyPress`/`KeyRelease` → key down/up event
  - `ButtonPress`/`ButtonRelease` → mouse button event
  - `MotionNotify` → mouse motion event
  - `ClientMessage(WM_DELETE_WINDOW)` → quit event
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
  - Handle `xdg_toplevel::close` → quit event
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
  - `wl_keyboard::key` → key down/up event
  - `wl_pointer::button` → mouse button event
  - `wl_pointer::motion` → mouse motion event
  - `xdg_toplevel::close` → quit event
- `GetFrameBuffer()`: returns pointer to owned `CWaylandFrameBuffer`

**Deliverable**: Wayland backend compiles and can be instantiated standalone.

**Review checkpoint**: User reviews Wayland implementation before proceeding.

---

### Phase 4: Windows Backend

**Goal**: Implement the Win32 window and framebuffer classes.

**Tasks — CWindowsFrameBuffer**:
- Create `library/system/windowsframebuffer.h` and `windowsframebuffer.cpp`
- Guarded with `#ifdef _WIN32`
- Constructor takes `HWND`, `HDC`, width, height
- Creates a DIB section:
  - `CreateDIBSection` with `BITMAPINFOHEADER` (32-bit, BI_RGB,
    top-down by using negative height)
  - Pixel pointer comes from the DIB section output parameter
- `GetPixels()`: returns the DIB pixel pointer (cast to `uint32_t*`)
- `GetWidth()`/`GetHeight()`: return dimensions
- `Clear()`: `memset(pixels, 0, w * h * 4)`
- `Flip()`: `BitBlt` or `StretchDIBits` from the DIB to the window DC
- Destructor: `DeleteObject` the DIB section

**Tasks — CWindowsWindow**:
- Create `library/system/windowswindow.h` and `windowswindow.cpp`
- Guarded with `#ifdef _WIN32`
- `Create()`:
  - `RegisterClassEx` with a `WndProc` callback
  - `CreateWindowEx` with `WS_OVERLAPPEDWINDOW` style, centered position
  - `GetDC` to obtain the device context
  - Instantiate `CWindowsFrameBuffer` with HWND/HDC/dimensions
- `Destroy()`: destroy framebuffer, `ReleaseDC`, `DestroyWindow`,
  `UnregisterClass`
- `Show()`: `ShowWindow(SW_SHOW)` / `ShowWindow(SW_HIDE)`
- `SetTitle()`: `SetWindowText`
- `SetFullScreen()`: `ChangeDisplaySettings` + `SetWindowLongPtr` to
  toggle between windowed and fullscreen styles
- `PollEvents()`: `PeekMessage` loop, translate via `WndProc`:
  - `WM_KEYDOWN`/`WM_KEYUP` → key down/up event
  - `WM_LBUTTONDOWN`/`WM_RBUTTONDOWN`/`WM_LBUTTONUP`/`WM_RBUTTONUP`
    → mouse button event
  - `WM_MOUSEMOVE` → mouse motion event
  - `WM_CLOSE` / `WM_DESTROY` → quit event
- `GetFrameBuffer()`: returns pointer to owned `CWindowsFrameBuffer`

**Deliverable**: Windows backend compiles and can be instantiated standalone.

**Review checkpoint**: User reviews Windows implementation before proceeding.

---

### Phase 5: Factory, Event Wiring & Integration

**Goal**: Wire all backends into the engine, replacing SDL entirely.

**Tasks — Event system wiring**:
- Update `basegame.h`:
  - Replace `#include <SDL.h>` with `#include <system/event.h>`
  - `HandleEvent(const SDL_Event&)` → `HandleEvent(const CEvent&)`
  - Remove `SDL_Window* m_pWindow`
- Update `basegame.cpp`:
  - Remove `SDL_Init()`, `SDL_Quit()`, `SDL_DestroyWindow()`
  - `PollEvents()`: call `IWindow::PollEvents()` then
    `while(CEventQueue::Instance().PollEvent(event))` loop
- Update `game.h/cpp`:
  - `HandleEvent(const SDL_Event&)` → `HandleEvent(const CEvent&)`
  - `SDL_QUIT` → `EVENT_QUIT`
- Update `igamestate.h`:
  - Remove `union SDL_Event;` forward declaration
  - `HandleEvent(const SDL_Event&)` → `HandleEvent(const CEvent&)`
  - Add `#include <system/event.h>`
- Update `commonstate.h/cpp`:
  - Replace `SDL_Event` with `CEvent`, remove `#include <SDL.h>`
- Update `titlescreenstate.h/cpp`:
  - Replace `SDL_Event` with `CEvent`, remove `#include <SDL.h>`
- Update `genfunc.h/cpp`:
  - `DispatchEvent()`: build a `CEvent` with `type = EVENT_USER`,
    push via `CEventQueue::Instance().PushEvent()`
  - Remove `#include <SDL.h>` from genfunc.cpp

**Tasks — Factory**:
- Create `library/system/windowfactory.h` and `windowfactory.cpp`:
  ```cpp
  std::unique_ptr<IWindow> CreateNativeWindow();
  ```
  - On Linux (`#ifndef _WIN32`):
    - Try `wl_display_connect(NULL)`:
      - If succeeds → disconnect,
        `NGenFunc::PostDebugMsg("Windowing system: Wayland")`,
        return `std::make_unique<CWaylandWindow>()`
      - If fails → try `XOpenDisplay(NULL)`:
        - If succeeds → close,
          `NGenFunc::PostDebugMsg("Windowing system: X11")`,
          return `std::make_unique<CX11Window>()`
        - If fails → throw exception
  - On Windows (`#ifdef _WIN32`):
    - `NGenFunc::PostDebugMsg("Windowing system: Windows")`
    - Return `std::make_unique<CWindowsWindow>()`

**Tasks — CDevice updates**:
- Replace `SDL_Window* m_pWindow` with `std::unique_ptr<IWindow> m_upWindow`
- `Create()`:
  - Call `CreateNativeWindow()` to get `IWindow`
  - Call `m_upWindow->Create(w, h, title)`
  - Remove all SDL calls
  - Call `CSoftwareRender::Instance().SetSurface(m_upWindow->GetFrameBuffer())`
- `ShowWindow()` → `m_upWindow->Show(visible)`
- `SetFullScreen()` → `m_upWindow->SetFullScreen(fullscreen)`
- Remove `GetWindow()`
- Add `IWindow* GetNativeWindow()` accessor
- Add `IFrameBuffer* GetFrameBuffer()` convenience accessor
- Remove `#include <SDL.h>`

**Tasks — CBaseGame updates**:
- Replace `SDL_Window* m_pWindow` with `IWindow*` and `IFrameBuffer*`
  cached from device
- Remove `SDL_Init()`, `SDL_Quit()`, `SDL_DestroyWindow()`
- `PollEvents()`: call `IWindow::PollEvents()` then process engine
  event queue
- Use `IFrameBuffer::Clear()` and `IFrameBuffer::Flip()`
- Remove `#include <SDL.h>`

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
- Remove `GetSurface()`, `Flip()`, `Clear()` (now on IFrameBuffer)
- `Render()`: use `m_surfaceData.w/h`, pass `&m_surfaceData` to `CRender2d`
- Remove `#include <SDL.h>`

**Tasks — CRender2d / RenderTri updates**:
- `CRender2d` constructor takes `CSRTexture*, CSurfaceData*` instead of
  `CSRTexture*, SDL_Surface*`
- `RenderTri()`: access `pRender->m_pSurface->w`, `->h`, `->pixels`
  (now CSurfaceData fields — same names, same types, rasterizer unchanged)

**Tasks — CStatCounter updates**:
- Replace `SDL_Window*` parameter with `IWindow*`
- Use `pWindow->SetTitle(statStr)` instead of `SDL_SetWindowTitle`
- Remove `#include <SDL.h>`

**Tasks — Build system updates**:
- `library/CMakeLists.txt`: add new .cpp files to `add_library`,
  conditionally compile platform-specific files
- `gameTemplate/CMakeLists.txt`:
  - Remove SDL find_package and linking
  - On Linux: `find_package(X11 REQUIRED)`,
    `pkg_search_module(WAYLAND_CLIENT REQUIRED wayland-client)`,
    link `${X11_LIBRARIES}`, `${WAYLAND_CLIENT_LIBRARIES}`
  - On Windows: link `user32`, `gdi32`
  - Add wayland-protocols for xdg-shell header generation (Linux only)

**Deliverable**: Full build succeeds on Linux, game runs with native window.
No SDL references remain (except xmlParser.cpp which is untouched).

**Review checkpoint**: User reviews integration before testing phase.

---

### Phase 6: Testing & Polish

**Goal**: Verify everything works correctly on all backends.

**Tasks**:
- Run automated tests (see test plan below)
- Run manual tests (see test plan below)
- Fix any issues found
- Remove any leftover SDL references (dead includes, commented code)
- Clean up includes
- Verify no new warnings

**Deliverable**: All tests pass, game runs identically to before.

**Review checkpoint**: User does final review.

---

## Test Plan

### Automated Tests

| # | Test | Method | Expected Result |
|---|------|--------|-----------------|
| A1 | Factory selects Wayland on Wayland session | Run on Wayland, log backend type | `CWaylandWindow` created |
| A2 | Factory falls back to X11 on X11 session | Run with `WAYLAND_DISPLAY` unset, `DISPLAY` set | `CX11Window` created |
| A3 | Factory creates Windows window on Windows | Run on Windows, log backend type | `CWindowsWindow` created |
| A4 | Factory fails gracefully (Linux) | Run with both env vars unset | Throws exception, doesn't crash |
| A5 | IFrameBuffer pixel write | Create FB, write known pattern, verify buffer | Pixels match expected pattern |
| A6 | Clear zeroes buffer | Create FB, write pixels, Clear(), verify all zero | All pixels are 0 |
| A7 | IWindow/IFrameBuffer separation | Verify GetFrameBuffer() returns valid IFrameBuffer* | Non-null, correct dimensions |
| A8 | Build clean (Linux) | `cmake --build . 2>&1 \| grep error` | Zero errors |
| A9 | Build clean (Windows) | Build with MSVC/MinGW | Zero errors |
| A10 | No new warnings | `cmake --build . 2>&1 \| grep warning` | No new warnings vs. baseline |
| A11 | No SDL references | Grep for SDL_ in source (excluding xmlParser) | Zero matches |

### Manual Tests

| # | Test | Platform | Steps | Expected Result |
|---|------|----------|-------|-----------------|
| M1 | Window appears | All | Launch game | Window appears with correct size |
| M2 | Window title | All | Launch game, wait for FPS counter | Title bar shows FPS stats |
| M3 | Rendering correct | All | Navigate to title screen | Sprites/text render correctly |
| M4 | Keyboard input | All | Press keys while game is running | Key events are received by game state |
| M5 | Mouse input | All | Move mouse, click in window | Mouse events are received by game state |
| M6 | Window close | All | Click the X button | Game exits cleanly |
| M7 | Full screen | All | Enable full screen in settings | Window goes full screen |
| M8 | X11 fallback | Linux | `unset WAYLAND_DISPLAY && ./gametemplate` | Game runs via X11/XWayland |
| M9 | Performance | All | Run game, check FPS counter | FPS comparable to SDL version |
| M10 | Clean exit | All | Exit game, check console for errors | No segfaults, no errors |
| M11 | Quit event | All | Close window via X button or trigger quit | Game loop terminates, resources freed |

---

## Open Questions

1. **Keysym mapping depth**: For event translation, how complete should the
   native → engine keysym mapping be? Full keyboard coverage (all keys
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

7. **Key code values**: The `CEvent` union is defined in Phase 1 with
   `CKeyEvent::keyCode`. Should we define our own key code enum (like
   SDL_Keycode), or reuse platform values (X11 keysyms on Linux,
   virtual key codes on Windows) and just document the mapping?

8. **Audio replacement**: SDL_mixer is still used for audio. Should audio
   replacement be a separate future phase, or should it be addressed as
   part of this plan? Options: ALSA, PulseAudio, PipeWire on Linux;
   WASAPI or XAudio2 on Windows.

9. **xmlParser.cpp**: This third-party file still includes `<SDL.h>` but
   we are leaving it untouched. After SDL is fully removed, this file
   will fail to compile. Should we remove just the SDL include from it
   (minimal change), or treat it as truly untouchable?
