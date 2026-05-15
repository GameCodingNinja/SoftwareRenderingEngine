
/************************************************************************
*    FILE NAME:       x11window.h
*
*    DESCRIPTION:     X11 window implementation using Xlib
************************************************************************/

#ifndef __x11window_h__
#define __x11window_h__

#ifdef __linux__

// Standard lib dependencies
#include <memory>

// Game lib dependencies
#include <system/iwindow.h>

// Forward declaration(s)
class CX11FrameBuffer;
typedef struct _XDisplay Display;
typedef unsigned long XID;
typedef XID Window;
typedef XID Atom;
typedef struct _XGC *GC;

class CX11Window : public IWindow
{
public:

    // Constructor
    CX11Window();

    // Destructor
    ~CX11Window();

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

private:

    // X11 display connection
    Display* m_pDisplay;

    // X11 window handle
    Window m_window;

    // Graphics context
    GC m_gc;

    // WM_DELETE_WINDOW atom for clean close
    Atom m_wmDeleteMessage;

    // Owned framebuffer
    std::unique_ptr<CX11FrameBuffer> m_upFrameBuffer;

    // Track fullscreen state
    bool m_fullscreen;
};

#endif  // __linux__

#endif  // __x11window_h__
