
/************************************************************************
*    FILE NAME:       iwindow.h
*
*    DESCRIPTION:     Pure virtual window interface
************************************************************************/

#ifndef __iwindow_h__
#define __iwindow_h__

// Standard lib dependencies
#include <string>

// Forward declaration(s)
class IFrameBuffer;

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

#endif  // __iwindow_h__
