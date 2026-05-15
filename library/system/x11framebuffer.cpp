
/************************************************************************
*    FILE NAME:       x11framebuffer.cpp
*
*    DESCRIPTION:     X11 framebuffer implementation using XImage
*                     with double buffering for tear-free rendering.
************************************************************************/

#ifdef __linux__

// Physical component dependency
#include <system/x11framebuffer.h>

// X11 lib dependencies
#include <X11/Xlib.h>
#include <X11/Xutil.h>

// Standard lib dependencies
#include <cstring>

// Game lib dependencies
#include <utilities/exceptionhandling.h>

/************************************************************************
*    desc:  Constructor — creates two pixel buffers and XImages
************************************************************************/
CX11FrameBuffer::CX11FrameBuffer(Display* pDisplay, Window window, GC gc, int width, int height) :
    m_pDisplay(pDisplay),
    m_window(window),
    m_gc(gc),
    m_backIndex(0),
    m_width(width),
    m_height(height)
{
    m_pPixels[0] = nullptr;
    m_pPixels[1] = nullptr;
    m_pImage[0] = nullptr;
    m_pImage[1] = nullptr;

    int screen = DefaultScreen(m_pDisplay);
    int depth = DefaultDepth(m_pDisplay, screen);
    Visual* pVisual = DefaultVisual(m_pDisplay, screen);

    for( int i = 0; i < 2; ++i )
    {
        // Allocate the pixel buffer
        m_pPixels[i] = new uint32_t[m_width * m_height];
        std::memset(m_pPixels[i], 0, m_width * m_height * sizeof(uint32_t));

        // Create an XImage wrapping our pixel buffer
        m_pImage[i] = XCreateImage(
            m_pDisplay,
            pVisual,
            depth,
            ZPixmap,
            0,                              // offset
            reinterpret_cast<char*>(m_pPixels[i]),
            m_width,
            m_height,
            32,                             // bitmap_pad (bits)
            0 );                            // bytes_per_line (0 = auto)

        if( m_pImage[i] == nullptr )
            throw NExcept::CCriticalException("X11 Framebuffer Error!",
                "Failed to create XImage.");

        // Set byte order to match the host
        m_pImage[i]->byte_order = LSBFirst;
    }

}   // Constructor


/************************************************************************
*    desc:  Destructor
************************************************************************/
CX11FrameBuffer::~CX11FrameBuffer()
{
    for( int i = 0; i < 2; ++i )
    {
        if( m_pImage[i] != nullptr )
        {
            // XDestroyImage frees the data pointer too, so null it
            // to prevent double-free since we allocated with new[]
            m_pImage[i]->data = nullptr;
            XDestroyImage(m_pImage[i]);
            m_pImage[i] = nullptr;
        }

        if( m_pPixels[i] != nullptr )
        {
            delete[] m_pPixels[i];
            m_pPixels[i] = nullptr;
        }
    }

}   // Destructor


/************************************************************************
*    desc:  Get the raw pixel buffer (returns the back buffer)
************************************************************************/
uint32_t* CX11FrameBuffer::GetPixels()
{
    return m_pPixels[m_backIndex];

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
    std::memset(m_pPixels[m_backIndex], 0, m_width * m_height * sizeof(uint32_t));

}   // Clear


/************************************************************************
*    desc:  Blit the back buffer to the window and swap
************************************************************************/
void CX11FrameBuffer::Flip()
{
    XPutImage(
        m_pDisplay,
        m_window,
        m_gc,
        m_pImage[m_backIndex],
        0, 0,           // src x, y
        0, 0,           // dest x, y
        m_width,
        m_height );

    XFlush(m_pDisplay);

    // Swap to the other buffer
    m_backIndex = 1 - m_backIndex;

}   // Flip

#endif  // __linux__
