
/************************************************************************
*    FILE NAME:       x11framebuffer.cpp
*
*    DESCRIPTION:     X11 framebuffer implementation using XImage
************************************************************************/

#ifdef __linux__

// Physical component dependency
#include <system/x11framebuffer.h>

// X11 lib dependencies
#include <X11/Xlib.h>
#include <X11/Xutil.h>

// Standard lib dependencies
#include <cstring>
#include <cstdlib>

// Game lib dependencies
#include <utilities/exceptionhandling.h>

/************************************************************************
*    desc:  Constructor
************************************************************************/
CX11FrameBuffer::CX11FrameBuffer(Display* pDisplay, Window window, GC gc, int width, int height) :
    m_pDisplay(pDisplay),
    m_window(window),
    m_gc(gc),
    m_pPixels(nullptr),
    m_pImage(nullptr),
    m_width(width),
    m_height(height)
{
    // Allocate the pixel buffer
    m_pPixels = new uint32_t[m_width * m_height];
    std::memset(m_pPixels, 0, m_width * m_height * sizeof(uint32_t));

    // Get the default visual's depth
    int screen = DefaultScreen(m_pDisplay);
    int depth = DefaultDepth(m_pDisplay, screen);
    Visual* pVisual = DefaultVisual(m_pDisplay, screen);

    // Create an XImage wrapping our pixel buffer
    m_pImage = XCreateImage(
        m_pDisplay,
        pVisual,
        depth,
        ZPixmap,
        0,                              // offset
        reinterpret_cast<char*>(m_pPixels),
        m_width,
        m_height,
        32,                             // bitmap_pad (bits)
        0 );                            // bytes_per_line (0 = auto)

    if( m_pImage == nullptr )
        throw NExcept::CCriticalException("X11 Framebuffer Error!",
            "Failed to create XImage.");

    // Set byte order to match the host
    m_pImage->byte_order = LSBFirst;

}   // Constructor


/************************************************************************
*    desc:  Destructor
************************************************************************/
CX11FrameBuffer::~CX11FrameBuffer()
{
    if( m_pImage != nullptr )
    {
        // XDestroyImage frees the data pointer too, so null it
        // to prevent double-free since we allocated with new[]
        m_pImage->data = nullptr;
        XDestroyImage(m_pImage);
        m_pImage = nullptr;
    }

    if( m_pPixels != nullptr )
    {
        delete[] m_pPixels;
        m_pPixels = nullptr;
    }

}   // Destructor


/************************************************************************
*    desc:  Get the raw pixel buffer
************************************************************************/
uint32_t* CX11FrameBuffer::GetPixels()
{
    return m_pPixels;

}   // GetPixels


/************************************************************************
*    desc:  Get framebuffer width
************************************************************************/
int CX11FrameBuffer::GetWidth() const
{
    return m_width;

}   // GetWidth


/************************************************************************
*    desc:  Get framebuffer height
************************************************************************/
int CX11FrameBuffer::GetHeight() const
{
    return m_height;

}   // GetHeight


/************************************************************************
*    desc:  Clear the pixel buffer to zero (black)
************************************************************************/
void CX11FrameBuffer::Clear()
{
    std::memset(m_pPixels, 0, m_width * m_height * sizeof(uint32_t));

}   // Clear


/************************************************************************
*    desc:  Display the pixel buffer contents in the window via XPutImage
************************************************************************/
void CX11FrameBuffer::Flip()
{
    XPutImage(
        m_pDisplay,
        m_window,
        m_gc,
        m_pImage,
        0, 0,           // src x, y
        0, 0,           // dest x, y
        m_width,
        m_height );

    XFlush(m_pDisplay);

}   // Flip

#endif  // __linux__
