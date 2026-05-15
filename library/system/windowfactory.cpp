
/************************************************************************
*    FILE NAME:       windowfactory.cpp
*
*    DESCRIPTION:     Factory function to create the appropriate
*                     native window based on the platform.
************************************************************************/

// Physical component dependency
#include <system/windowfactory.h>

// Game lib dependencies
#include <system/iwindow.h>
#include <utilities/genfunc.h>
#include <utilities/exceptionhandling.h>

#ifdef __linux__
    #include <system/waylandwindow.h>
    #include <system/x11window.h>
    #include <wayland-client.h>
    #include <X11/Xlib.h>
#endif

#ifdef _WIN32
    #include <system/windowswindow.h>
#endif


/************************************************************************
*    desc:  Create a native window for the current platform
************************************************************************/
std::unique_ptr<IWindow> CreateNativeWindow()
{
    #ifdef __linux__

    // Try Wayland first
    struct wl_display* pDisplay = wl_display_connect(nullptr);
    if( pDisplay != nullptr )
    {
        wl_display_disconnect(pDisplay);
        NGenFunc::PostDebugMsg("Windowing system: Wayland");
        return std::make_unique<CWaylandWindow>();
    }

    // Fall back to X11
    Display* pX11Display = XOpenDisplay(nullptr);
    if( pX11Display != nullptr )
    {
        XCloseDisplay(pX11Display);
        NGenFunc::PostDebugMsg("Windowing system: X11");
        return std::make_unique<CX11Window>();
    }

    throw NExcept::CCriticalException("Window Creation Error!",
        "No display server available. Neither Wayland nor X11 could be connected.");

    #elif defined(_WIN32)

    NGenFunc::PostDebugMsg("Windowing system: Windows");
    return std::make_unique<CWindowsWindow>();

    #else

    throw NExcept::CCriticalException("Window Creation Error!",
        "Unsupported platform.");

    #endif
}
