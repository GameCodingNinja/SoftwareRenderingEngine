
/************************************************************************
*    FILE NAME:       x11framebuffer.h
*
*    DESCRIPTION:     X11 framebuffer implementation using XImage
*                     with double buffering for tear-free rendering.
************************************************************************/

#ifndef __x11framebuffer_h__
#define __x11framebuffer_h__

#ifdef __linux__

// Game lib dependencies
#include <system/iframebuffer.h>

// Forward declaration(s) — avoid pulling X11 headers into everything
typedef struct _XDisplay Display;
typedef unsigned long XID;
typedef XID Window;
typedef struct _XGC *GC;
typedef struct _XImage XImage;

class CX11FrameBuffer : public IFrameBuffer
{
public:

    // Constructor
    CX11FrameBuffer(Display* pDisplay, Window window, GC gc, int width, int height);

    // Destructor
    ~CX11FrameBuffer();

    // Get the raw pixel buffer (returns the back buffer)
    uint32_t* GetPixels() override;

    // Get framebuffer dimensions
    int GetWidth() const override;
    int GetHeight() const override;

    // Clear the pixel buffer to zero (black)
    void Clear() override;

    // Swap buffers and display the back buffer contents
    void Flip() override;

private:

    // X11 handles (not owned — the window class owns these)
    Display* m_pDisplay;
    Window m_window;
    GC m_gc;

    // Double buffer: two pixel buffers and XImages
    uint32_t* m_pPixels[2];
    XImage* m_pImage[2];

    // Which buffer the engine is drawing to (0 or 1)
    int m_backIndex;

    // Dimensions
    int m_width;
    int m_height;
};

#endif  // __linux__

#endif  // __x11framebuffer_h__
