
/************************************************************************
*    FILE NAME:       waylandwindow.h
*
*    DESCRIPTION:     Wayland window implementation
************************************************************************/

#ifndef __waylandwindow_h__
#define __waylandwindow_h__

#ifdef __linux__

// Standard lib dependencies
#include <memory>
#include <string>
#include <cstdint>

// Game lib dependencies
#include <system/iwindow.h>

// Forward declaration(s)
class CWaylandFrameBuffer;
struct wl_display;
struct wl_registry;
struct wl_compositor;
struct wl_shm;
struct wl_surface;
struct wl_seat;
struct wl_keyboard;
struct wl_pointer;
struct xdg_wm_base;
struct xdg_surface;
struct xdg_toplevel;
struct zxdg_decoration_manager_v1;
struct zxdg_toplevel_decoration_v1;

class CWaylandWindow : public IWindow
{
public:

    // Constructor
    CWaylandWindow();

    // Destructor
    ~CWaylandWindow();

    // Create the window
    void Create(int width, int height, const char* title = "") override;

    // Destroy the window
    void Destroy() override;

    // Show or hide the window
    void Show(bool visible) override;

    // Set the window title
    void SetTitle(const std::string& title) override;

    // Set full screen mode
    void SetFullScreen(bool fullscreen) override;

    // Poll native events and push them to the event queue
    void PollEvents() override;

    // Get the framebuffer owned by this window
    IFrameBuffer* GetFrameBuffer() override;

    // --- Wayland listener callbacks (must be public for C callbacks) ---

    // Registry
    void OnRegistryGlobal(struct wl_registry* registry, uint32_t name,
                          const char* interface, uint32_t version);

    // xdg_wm_base
    void OnWmBasePing(struct xdg_wm_base* wmBase, uint32_t serial);

    // xdg_surface
    void OnXdgSurfaceConfigure(struct xdg_surface* surface, uint32_t serial);

    // xdg_toplevel
    void OnToplevelConfigure(struct xdg_toplevel* toplevel, int32_t width,
                             int32_t height, struct wl_array* states);
    void OnToplevelClose(struct xdg_toplevel* toplevel);

    // wl_keyboard
    void OnKeyboardKey(struct wl_keyboard* keyboard, uint32_t serial,
                       uint32_t time, uint32_t key, uint32_t state);

    // wl_pointer
    void OnPointerMotion(struct wl_pointer* pointer, uint32_t time,
                         int32_t x, int32_t y);
    void OnPointerButton(struct wl_pointer* pointer, uint32_t serial,
                         uint32_t time, uint32_t button, uint32_t state);

private:

    // Wayland globals
    struct wl_display* m_pDisplay;
    struct wl_registry* m_pRegistry;
    struct wl_compositor* m_pCompositor;
    struct wl_shm* m_pShm;
    struct wl_surface* m_pSurface;
    struct wl_seat* m_pSeat;
    struct wl_keyboard* m_pKeyboard;
    struct wl_pointer* m_pPointer;

    // xdg shell
    struct xdg_wm_base* m_pWmBase;
    struct xdg_surface* m_pXdgSurface;
    struct xdg_toplevel* m_pToplevel;

    // Server-side decorations
    struct zxdg_decoration_manager_v1* m_pDecorationManager;
    struct zxdg_toplevel_decoration_v1* m_pDecoration;

    // Owned framebuffer
    std::unique_ptr<CWaylandFrameBuffer> m_upFrameBuffer;

    // Track state
    bool m_configured;
    bool m_fullscreen;
    int m_width;
    int m_height;

    // Last known mouse position (for relative motion)
    int m_lastMouseX;
    int m_lastMouseY;
};

#endif

#endif
