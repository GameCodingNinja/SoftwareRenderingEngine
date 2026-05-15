
/************************************************************************
*    FILE NAME:       waylandwindow.cpp
*
*    DESCRIPTION:     Wayland window implementation
************************************************************************/

#ifdef __linux__

// Physical component dependency
#include <system/waylandwindow.h>
#include <system/waylandframebuffer.h>

// Wayland lib dependencies
#include <wayland-client.h>
#include <system/xdg-shell-client-protocol.h>
#include <system/xdg-decoration-client-protocol.h>

// Standard lib dependencies
#include <cstring>
#include <poll.h>
#include <linux/input-event-codes.h>

// Game lib dependencies
#include <system/event.h>
#include <system/eventqueue.h>
#include <utilities/exceptionhandling.h>
#include <utilities/genfunc.h>


// ======================================================================
//  Wayland listener C callbacks — forward to the CWaylandWindow instance
// ======================================================================

// --- Registry ---
static void RegistryGlobal(void* data, struct wl_registry* registry,
    uint32_t name, const char* interface, uint32_t version)
{
    static_cast<CWaylandWindow*>(data)->OnRegistryGlobal(registry, name, interface, version);
}

static void RegistryGlobalRemove(void* /*data*/, struct wl_registry* /*registry*/, uint32_t /*name*/)
{
}

static const struct wl_registry_listener registryListener = {
    RegistryGlobal,
    RegistryGlobalRemove
};


// --- xdg_wm_base ---
static void WmBasePing(void* data, struct xdg_wm_base* wmBase, uint32_t serial)
{
    static_cast<CWaylandWindow*>(data)->OnWmBasePing(wmBase, serial);
}

static const struct xdg_wm_base_listener wmBaseListener = {
    WmBasePing
};


// --- xdg_surface ---
static void XdgSurfaceConfigure(void* data, struct xdg_surface* surface, uint32_t serial)
{
    static_cast<CWaylandWindow*>(data)->OnXdgSurfaceConfigure(surface, serial);
}

static const struct xdg_surface_listener xdgSurfaceListener = {
    XdgSurfaceConfigure
};


// --- xdg_toplevel ---
static void ToplevelConfigure(void* data, struct xdg_toplevel* toplevel,
    int32_t width, int32_t height, struct wl_array* states)
{
    static_cast<CWaylandWindow*>(data)->OnToplevelConfigure(toplevel, width, height, states);
}

static void ToplevelClose(void* data, struct xdg_toplevel* toplevel)
{
    static_cast<CWaylandWindow*>(data)->OnToplevelClose(toplevel);
}

static void ToplevelConfigureBounds(void* /*data*/, struct xdg_toplevel* /*toplevel*/,
    int32_t /*width*/, int32_t /*height*/)
{
}

static void ToplevelWmCapabilities(void* /*data*/, struct xdg_toplevel* /*toplevel*/,
    struct wl_array* /*capabilities*/)
{
}

static const struct xdg_toplevel_listener toplevelListener = {
    ToplevelConfigure,
    ToplevelClose,
    ToplevelConfigureBounds,
    ToplevelWmCapabilities
};


// --- wl_keyboard ---
static void KeyboardKeymap(void* /*data*/, struct wl_keyboard* /*kb*/,
    uint32_t /*format*/, int32_t /*fd*/, uint32_t /*size*/)
{
}

static void KeyboardEnter(void* /*data*/, struct wl_keyboard* /*kb*/,
    uint32_t /*serial*/, struct wl_surface* /*surface*/, struct wl_array* /*keys*/)
{
}

static void KeyboardLeave(void* /*data*/, struct wl_keyboard* /*kb*/,
    uint32_t /*serial*/, struct wl_surface* /*surface*/)
{
}

static void KeyboardKey(void* data, struct wl_keyboard* kb,
    uint32_t serial, uint32_t time, uint32_t key, uint32_t state)
{
    static_cast<CWaylandWindow*>(data)->OnKeyboardKey(kb, serial, time, key, state);
}

static void KeyboardModifiers(void* /*data*/, struct wl_keyboard* /*kb*/,
    uint32_t /*serial*/, uint32_t /*modsDepressed*/, uint32_t /*modsLatched*/,
    uint32_t /*modsLocked*/, uint32_t /*group*/)
{
}

static void KeyboardRepeatInfo(void* /*data*/, struct wl_keyboard* /*kb*/,
    int32_t /*rate*/, int32_t /*delay*/)
{
}

static const struct wl_keyboard_listener keyboardListener = {
    KeyboardKeymap,
    KeyboardEnter,
    KeyboardLeave,
    KeyboardKey,
    KeyboardModifiers,
    KeyboardRepeatInfo
};


// --- wl_pointer ---
static void PointerEnter(void* /*data*/, struct wl_pointer* /*pointer*/,
    uint32_t /*serial*/, struct wl_surface* /*surface*/,
    wl_fixed_t /*sx*/, wl_fixed_t /*sy*/)
{
}

static void PointerLeave(void* /*data*/, struct wl_pointer* /*pointer*/,
    uint32_t /*serial*/, struct wl_surface* /*surface*/)
{
}

static void PointerMotion(void* data, struct wl_pointer* pointer,
    uint32_t time, wl_fixed_t sx, wl_fixed_t sy)
{
    static_cast<CWaylandWindow*>(data)->OnPointerMotion(
        pointer, time, wl_fixed_to_int(sx), wl_fixed_to_int(sy));
}

static void PointerButton(void* data, struct wl_pointer* pointer,
    uint32_t serial, uint32_t time, uint32_t button, uint32_t state)
{
    static_cast<CWaylandWindow*>(data)->OnPointerButton(pointer, serial, time, button, state);
}

static void PointerAxis(void* /*data*/, struct wl_pointer* /*pointer*/,
    uint32_t /*time*/, uint32_t /*axis*/, wl_fixed_t /*value*/)
{
}

static void PointerFrame(void* /*data*/, struct wl_pointer* /*pointer*/)
{
}

static void PointerAxisSource(void* /*data*/, struct wl_pointer* /*pointer*/,
    uint32_t /*axis_source*/)
{
}

static void PointerAxisStop(void* /*data*/, struct wl_pointer* /*pointer*/,
    uint32_t /*time*/, uint32_t /*axis*/)
{
}

static void PointerAxisDiscrete(void* /*data*/, struct wl_pointer* /*pointer*/,
    uint32_t /*axis*/, int32_t /*discrete*/)
{
}

static void PointerAxisValue120(void* /*data*/, struct wl_pointer* /*pointer*/,
    uint32_t /*axis*/, int32_t /*value120*/)
{
}

static void PointerAxisRelativeDirection(void* /*data*/, struct wl_pointer* /*pointer*/,
    uint32_t /*axis*/, uint32_t /*direction*/)
{
}

static const struct wl_pointer_listener pointerListener = {
    PointerEnter,
    PointerLeave,
    PointerMotion,
    PointerButton,
    PointerAxis,
    PointerFrame,
    PointerAxisSource,
    PointerAxisStop,
    PointerAxisDiscrete,
    PointerAxisValue120,
    PointerAxisRelativeDirection
};


// --- wl_seat ---
static void SeatCapabilities(void* data, struct wl_seat* seat, uint32_t caps)
{
    auto* self = static_cast<CWaylandWindow*>(data);

    // This is called when seat capabilities change. We grab keyboard/pointer
    // from within the window class, so we access members via the public
    // listener callbacks below. For simplicity, we handle it directly here
    // by re-calling Create logic. But since we store the seat pointer and
    // bind in Create(), we handle it there instead.
    (void)self;
    (void)seat;
    (void)caps;
}

static void SeatName(void* /*data*/, struct wl_seat* /*seat*/, const char* /*name*/)
{
}

static const struct wl_seat_listener seatListener = {
    SeatCapabilities,
    SeatName
};


// ======================================================================
//  CWaylandWindow implementation
// ======================================================================

/************************************************************************
*    desc:  Constructor
************************************************************************/
CWaylandWindow::CWaylandWindow() :
    m_pDisplay(nullptr),
    m_pRegistry(nullptr),
    m_pCompositor(nullptr),
    m_pShm(nullptr),
    m_pSurface(nullptr),
    m_pSeat(nullptr),
    m_pKeyboard(nullptr),
    m_pPointer(nullptr),
    m_pWmBase(nullptr),
    m_pXdgSurface(nullptr),
    m_pToplevel(nullptr),
    m_pDecorationManager(nullptr),
    m_pDecoration(nullptr),
    m_configured(false),
    m_fullscreen(false),
    m_width(0),
    m_height(0),
    m_lastMouseX(0),
    m_lastMouseY(0)
{
}   // Constructor


/************************************************************************
*    desc:  Destructor
************************************************************************/
CWaylandWindow::~CWaylandWindow()
{
    Destroy();

}   // Destructor


/************************************************************************
*    desc:  Create the Wayland window
************************************************************************/
void CWaylandWindow::Create(int width, int height, const char* title)
{
    m_width = width;
    m_height = height;

    // Connect to the Wayland display
    m_pDisplay = wl_display_connect(nullptr);
    if( m_pDisplay == nullptr )
        throw NExcept::CCriticalException("Wayland Window Error!",
            "Failed to connect to Wayland display.");

    // Get the registry and bind globals
    m_pRegistry = wl_display_get_registry(m_pDisplay);
    wl_registry_add_listener(m_pRegistry, &registryListener, this);

    // Roundtrip to receive all registry events
    wl_display_roundtrip(m_pDisplay);

    if( m_pCompositor == nullptr )
        throw NExcept::CCriticalException("Wayland Window Error!",
            "Failed to bind wl_compositor.");

    if( m_pShm == nullptr )
        throw NExcept::CCriticalException("Wayland Window Error!",
            "Failed to bind wl_shm.");

    if( m_pWmBase == nullptr )
        throw NExcept::CCriticalException("Wayland Window Error!",
            "Failed to bind xdg_wm_base. Your compositor may not support xdg-shell.");

    // Create the Wayland surface
    m_pSurface = wl_compositor_create_surface(m_pCompositor);
    if( m_pSurface == nullptr )
        throw NExcept::CCriticalException("Wayland Window Error!",
            "Failed to create wl_surface.");

    // Create xdg_surface
    m_pXdgSurface = xdg_wm_base_get_xdg_surface(m_pWmBase, m_pSurface);
    xdg_surface_add_listener(m_pXdgSurface, &xdgSurfaceListener, this);

    // Create xdg_toplevel
    m_pToplevel = xdg_surface_get_toplevel(m_pXdgSurface);
    xdg_toplevel_add_listener(m_pToplevel, &toplevelListener, this);

    // Set title
    if( title != nullptr && title[0] != '\0' )
        xdg_toplevel_set_title(m_pToplevel, title);

    // Request server-side decorations (title bar, close button, etc.)
    if( m_pDecorationManager != nullptr )
    {
        m_pDecoration = zxdg_decoration_manager_v1_get_toplevel_decoration(
            m_pDecorationManager, m_pToplevel);
        zxdg_toplevel_decoration_v1_set_mode(
            m_pDecoration, ZXDG_TOPLEVEL_DECORATION_V1_MODE_SERVER_SIDE);
    }

    // Bind keyboard and pointer from seat
    if( m_pSeat != nullptr )
    {
        m_pKeyboard = wl_seat_get_keyboard(m_pSeat);
        if( m_pKeyboard != nullptr )
            wl_keyboard_add_listener(m_pKeyboard, &keyboardListener, this);

        m_pPointer = wl_seat_get_pointer(m_pSeat);
        if( m_pPointer != nullptr )
            wl_pointer_add_listener(m_pPointer, &pointerListener, this);
    }

    // Commit the surface to trigger the configure event
    wl_surface_commit(m_pSurface);

    // Wait for the initial configure
    while( !m_configured )
        wl_display_roundtrip(m_pDisplay);

    // Create the framebuffer
    m_upFrameBuffer = std::make_unique<CWaylandFrameBuffer>(
        m_pDisplay, m_pShm, m_pSurface, m_width, m_height);

}   // Create


/************************************************************************
*    desc:  Destroy the window and free resources
************************************************************************/
void CWaylandWindow::Destroy()
{
    m_upFrameBuffer.reset();

    if( m_pPointer != nullptr )
    {
        wl_pointer_destroy(m_pPointer);
        m_pPointer = nullptr;
    }

    if( m_pKeyboard != nullptr )
    {
        wl_keyboard_destroy(m_pKeyboard);
        m_pKeyboard = nullptr;
    }

    if( m_pDecoration != nullptr )
    {
        zxdg_toplevel_decoration_v1_destroy(m_pDecoration);
        m_pDecoration = nullptr;
    }

    if( m_pDecorationManager != nullptr )
    {
        zxdg_decoration_manager_v1_destroy(m_pDecorationManager);
        m_pDecorationManager = nullptr;
    }

    if( m_pToplevel != nullptr )
    {
        xdg_toplevel_destroy(m_pToplevel);
        m_pToplevel = nullptr;
    }

    if( m_pXdgSurface != nullptr )
    {
        xdg_surface_destroy(m_pXdgSurface);
        m_pXdgSurface = nullptr;
    }

    if( m_pSurface != nullptr )
    {
        wl_surface_destroy(m_pSurface);
        m_pSurface = nullptr;
    }

    if( m_pSeat != nullptr )
    {
        wl_seat_destroy(m_pSeat);
        m_pSeat = nullptr;
    }

    if( m_pWmBase != nullptr )
    {
        xdg_wm_base_destroy(m_pWmBase);
        m_pWmBase = nullptr;
    }

    if( m_pShm != nullptr )
    {
        wl_shm_destroy(m_pShm);
        m_pShm = nullptr;
    }

    if( m_pCompositor != nullptr )
    {
        wl_compositor_destroy(m_pCompositor);
        m_pCompositor = nullptr;
    }

    if( m_pRegistry != nullptr )
    {
        wl_registry_destroy(m_pRegistry);
        m_pRegistry = nullptr;
    }

    if( m_pDisplay != nullptr )
    {
        wl_display_disconnect(m_pDisplay);
        m_pDisplay = nullptr;
    }

}   // Destroy


/************************************************************************
*    desc:  Show or hide the window
*           Wayland windows become visible on first commit.
*           To hide, we can unmap by destroying and recreating the
*           xdg surface, but for simplicity we just commit an
*           empty/full surface.
************************************************************************/
void CWaylandWindow::Show(bool visible)
{
    if( m_pSurface == nullptr || m_pDisplay == nullptr )
        return;

    if( visible )
    {
        // Attach, commit, and flush to show
        if( m_upFrameBuffer != nullptr )
            m_upFrameBuffer->Flip();

        // Ensure the compositor processes the frame
        wl_display_roundtrip(m_pDisplay);
    }
    else
    {
        // Attach a null buffer to hide
        wl_surface_attach(m_pSurface, nullptr, 0, 0);
        wl_surface_commit(m_pSurface);
        wl_display_flush(m_pDisplay);
    }

}   // Show


/************************************************************************
*    desc:  Set the window title
************************************************************************/
void CWaylandWindow::SetTitle(const std::string& title)
{
    if( m_pToplevel != nullptr )
        xdg_toplevel_set_title(m_pToplevel, title.c_str());

}   // SetTitle


/************************************************************************
*    desc:  Set full screen mode
************************************************************************/
void CWaylandWindow::SetFullScreen(bool fullscreen)
{
    if( m_pToplevel == nullptr || m_fullscreen == fullscreen )
        return;

    m_fullscreen = fullscreen;

    if( fullscreen )
        xdg_toplevel_set_fullscreen(m_pToplevel, nullptr);
    else
        xdg_toplevel_unset_fullscreen(m_pToplevel);

}   // SetFullScreen


/************************************************************************
*    desc:  Poll Wayland events and push to the event queue
************************************************************************/
void CWaylandWindow::PollEvents()
{
    if( m_pDisplay == nullptr )
        return;

    // Flush outgoing requests
    wl_display_flush(m_pDisplay);

    // Dispatch any pending events (non-blocking)
    wl_display_dispatch_pending(m_pDisplay);

    // Read events if available (non-blocking via prepare/read/cancel)
    while( wl_display_prepare_read(m_pDisplay) != 0 )
        wl_display_dispatch_pending(m_pDisplay);

    // Check if there are events to read without blocking
    struct pollfd pfd;
    pfd.fd = wl_display_get_fd(m_pDisplay);
    pfd.events = POLLIN;
    pfd.revents = 0;

    if( poll(&pfd, 1, 0) > 0 )
        wl_display_read_events(m_pDisplay);
    else
        wl_display_cancel_read(m_pDisplay);

    wl_display_dispatch_pending(m_pDisplay);

}   // PollEvents


/************************************************************************
*    desc:  Get the framebuffer owned by this window
************************************************************************/
IFrameBuffer* CWaylandWindow::GetFrameBuffer()
{
    return m_upFrameBuffer.get();

}   // GetFrameBuffer


// ======================================================================
//  Wayland listener callback implementations
// ======================================================================

/************************************************************************
*    desc:  Registry global — bind compositor, shm, seat, xdg_wm_base
************************************************************************/
void CWaylandWindow::OnRegistryGlobal(struct wl_registry* registry,
    uint32_t name, const char* interface, uint32_t version)
{
    if( strcmp(interface, wl_compositor_interface.name) == 0 )
    {
        m_pCompositor = static_cast<struct wl_compositor*>(
            wl_registry_bind(registry, name, &wl_compositor_interface, 4));
    }
    else if( strcmp(interface, wl_shm_interface.name) == 0 )
    {
        m_pShm = static_cast<struct wl_shm*>(
            wl_registry_bind(registry, name, &wl_shm_interface, 1));
    }
    else if( strcmp(interface, wl_seat_interface.name) == 0 )
    {
        m_pSeat = static_cast<struct wl_seat*>(
            wl_registry_bind(registry, name, &wl_seat_interface, 5));
        wl_seat_add_listener(m_pSeat, &seatListener, this);
    }
    else if( strcmp(interface, xdg_wm_base_interface.name) == 0 )
    {
        m_pWmBase = static_cast<struct xdg_wm_base*>(
            wl_registry_bind(registry, name, &xdg_wm_base_interface, 1));
        xdg_wm_base_add_listener(m_pWmBase, &wmBaseListener, this);
    }
    else if( strcmp(interface, zxdg_decoration_manager_v1_interface.name) == 0 )
    {
        m_pDecorationManager = static_cast<struct zxdg_decoration_manager_v1*>(
            wl_registry_bind(registry, name, &zxdg_decoration_manager_v1_interface, 1));
    }

}   // OnRegistryGlobal


/************************************************************************
*    desc:  xdg_wm_base ping — must respond to keep the connection alive
************************************************************************/
void CWaylandWindow::OnWmBasePing(struct xdg_wm_base* wmBase, uint32_t serial)
{
    xdg_wm_base_pong(wmBase, serial);

}   // OnWmBasePing


/************************************************************************
*    desc:  xdg_surface configure — acknowledge the configuration
************************************************************************/
void CWaylandWindow::OnXdgSurfaceConfigure(struct xdg_surface* surface, uint32_t serial)
{
    xdg_surface_ack_configure(surface, serial);
    m_configured = true;

}   // OnXdgSurfaceConfigure


/************************************************************************
*    desc:  xdg_toplevel configure — handle size changes
************************************************************************/
void CWaylandWindow::OnToplevelConfigure(struct xdg_toplevel* /*toplevel*/,
    int32_t width, int32_t height, struct wl_array* /*states*/)
{
    // If width/height are 0, the compositor is letting us choose
    if( width > 0 && height > 0 )
    {
        m_width = width;
        m_height = height;
    }

}   // OnToplevelConfigure


/************************************************************************
*    desc:  xdg_toplevel close — user clicked the close button
************************************************************************/
void CWaylandWindow::OnToplevelClose(struct xdg_toplevel* /*toplevel*/)
{
    CEvent event;
    std::memset(&event, 0, sizeof(event));
    event.type = EVENT_QUIT;
    CEventQueue::Instance().PushEvent(event);

}   // OnToplevelClose


/************************************************************************
*    desc:  Keyboard key event
*           Wayland key codes are Linux input event codes (evdev).
*           We pass them through directly for now.
************************************************************************/
void CWaylandWindow::OnKeyboardKey(struct wl_keyboard* /*keyboard*/,
    uint32_t /*serial*/, uint32_t /*time*/, uint32_t key, uint32_t state)
{
    CEvent event;
    std::memset(&event, 0, sizeof(event));

    event.key.type = (state == WL_KEYBOARD_KEY_STATE_PRESSED) ? EVENT_KEY_DOWN : EVENT_KEY_UP;
    event.key.keyCode = static_cast<int>(key);
    event.key.repeat = false;

    CEventQueue::Instance().PushEvent(event);

}   // OnKeyboardKey


/************************************************************************
*    desc:  Pointer motion event
************************************************************************/
void CWaylandWindow::OnPointerMotion(struct wl_pointer* /*pointer*/,
    uint32_t /*time*/, int32_t x, int32_t y)
{
    CEvent event;
    std::memset(&event, 0, sizeof(event));

    event.motion.type = EVENT_MOUSE_MOTION;
    event.motion.x = x;
    event.motion.y = y;
    event.motion.xrel = x - m_lastMouseX;
    event.motion.yrel = y - m_lastMouseY;

    m_lastMouseX = x;
    m_lastMouseY = y;

    CEventQueue::Instance().PushEvent(event);

}   // OnPointerMotion


/************************************************************************
*    desc:  Pointer button event
*           Wayland uses Linux input event codes for buttons:
*           BTN_LEFT=0x110, BTN_RIGHT=0x111, BTN_MIDDLE=0x112
*           We map to 1=left, 2=middle, 3=right
************************************************************************/
void CWaylandWindow::OnPointerButton(struct wl_pointer* /*pointer*/,
    uint32_t /*serial*/, uint32_t /*time*/, uint32_t button, uint32_t state)
{
    CEvent event;
    std::memset(&event, 0, sizeof(event));

    event.button.type = (state == WL_POINTER_BUTTON_STATE_PRESSED)
        ? EVENT_MOUSE_BUTTON_DOWN : EVENT_MOUSE_BUTTON_UP;

    // Map Linux button codes to simple 1/2/3
    switch( button )
    {
        case BTN_LEFT:   event.button.button = 1; break;
        case BTN_RIGHT:  event.button.button = 3; break;
        case BTN_MIDDLE: event.button.button = 2; break;
        default:         event.button.button = static_cast<uint8_t>(button); break;
    }

    event.button.x = m_lastMouseX;
    event.button.y = m_lastMouseY;

    CEventQueue::Instance().PushEvent(event);

}   // OnPointerButton

#endif  // __linux__
