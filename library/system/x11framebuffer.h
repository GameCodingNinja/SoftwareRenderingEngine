
/************************************************************************
*    FILE NAME:       x11framebuffer.h
*
*    DESCRIPTION:     X11 framebuffer implementation using XImage
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

    // Get the raw pixel buffer
    uint32_t* GetPixels() override;

    // Get framebuffer dimensions
    int GetWidth() const override;
    int GetHeight() const override;

    // Clear the pixel buffer to zero (black)
    void Clear() override;

    // Display the pixel buffer contents in the window
    void Flip() override;

private:

    // X11 handles (not owned — the window class owns these)
    Display* m_pDisplay;
    Window m_window;
    GC m_gc;

    // Pixel buffer
    uint32_t* m_pPixels;

    // XImage wrapping the pixel buffer
    XImage* m_pImage;

    // Dimensions
    int m_width;
    int m_height;
};

#endif  // __linux__

#endif  // __x11framebuffer_h__
