
/************************************************************************
*    FILE NAME:       windowsframebuffer.cpp
*
*    DESCRIPTION:     Windows framebuffer implementation using DIB section
*                     with double buffering for tear-free rendering.
************************************************************************/

#ifdef _WIN32

// Physical component dependency
#include <system/windowsframebuffer.h>

// Standard lib dependencies
#include <cstring>

// Game lib dependencies
#include <utilities/exceptionhandling.h>

/************************************************************************
*    desc:  Constructor — creates two DIB sections for double buffering
************************************************************************/
CWindowsFrameBuffer::CWindowsFrameBuffer(HWND hWnd, HDC hDC, int width, int height) :
    m_hWnd(hWnd),
    m_hDC(hDC),
    m_backIndex(0),
    m_width(width),
    m_height(height)
{
    for( int i = 0; i < 2; ++i )
    {
        m_hMemDC[i] = nullptr;
        m_hBitmap[i] = nullptr;
        m_hOldBitmap[i] = nullptr;
        m_pPixels[i] = nullptr;
    }

    // Setup the bitmap info for a 32-bit top-down DIB
    BITMAPINFO bmi;
    std::memset(&bmi, 0, sizeof(bmi));
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = m_width;
    bmi.bmiHeader.biHeight = -m_height;     // Negative = top-down
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    for( int i = 0; i < 2; ++i )
    {
        // Create a memory DC compatible with the window DC
        m_hMemDC[i] = CreateCompatibleDC(m_hDC);
        if( m_hMemDC[i] == nullptr )
            throw NExcept::CCriticalException("Windows Framebuffer Error!",
                "Failed to create compatible DC.");

        // Create the DIB section — m_pPixels will point to the pixel data
        m_hBitmap[i] = CreateDIBSection(
            m_hDC, &bmi, DIB_RGB_COLORS,
            reinterpret_cast<void**>(&m_pPixels[i]),
            nullptr, 0 );

        if( m_hBitmap[i] == nullptr || m_pPixels[i] == nullptr )
            throw NExcept::CCriticalException("Windows Framebuffer Error!",
                "Failed to create DIB section.");

        // Select the bitmap into the memory DC
        m_hOldBitmap[i] = static_cast<HBITMAP>(SelectObject(m_hMemDC[i], m_hBitmap[i]));

        // Clear to black
        std::memset(m_pPixels[i], 0, m_width * m_height * sizeof(uint32_t));
    }

}


/************************************************************************
*    desc:  Destructor
************************************************************************/
CWindowsFrameBuffer::~CWindowsFrameBuffer()
{
    for( int i = 0; i < 2; ++i )
    {
        if( m_hMemDC[i] != nullptr )
        {
            if( m_hOldBitmap[i] != nullptr )
                SelectObject(m_hMemDC[i], m_hOldBitmap[i]);

            DeleteDC(m_hMemDC[i]);
            m_hMemDC[i] = nullptr;
        }

        if( m_hBitmap[i] != nullptr )
        {
            DeleteObject(m_hBitmap[i]);
            m_hBitmap[i] = nullptr;
        }

        m_pPixels[i] = nullptr;
    }

}


/************************************************************************
*    desc:  Get the raw pixel buffer (returns the back buffer)
************************************************************************/
uint32_t* CWindowsFrameBuffer::getPixels()
{
    return m_pPixels[m_backIndex];

}


/************************************************************************
*    desc:  Get framebuffer width
************************************************************************/
int CWindowsFrameBuffer::getWidth() const
{
    return m_width;

}


/************************************************************************
*    desc:  Get framebuffer height
************************************************************************/
int CWindowsFrameBuffer::getHeight() const
{
    return m_height;

}


/************************************************************************
*    desc:  Clear the pixel buffer to zero (black)
************************************************************************/
void CWindowsFrameBuffer::Clear()
{
    std::memset(m_pPixels[m_backIndex], 0, m_width * m_height * sizeof(uint32_t));

}


/************************************************************************
*    desc:  Resize the framebuffer to new dimensions
************************************************************************/
void CWindowsFrameBuffer::Resize(int width, int height)
{
    // Early out if dimensions haven't changed
    if( width == m_width && height == m_height )
        return;

    // Destroy old DIB sections
    for( int i = 0; i < 2; ++i )
    {
        if( m_hMemDC[i] != nullptr )
        {
            if( m_hOldBitmap[i] != nullptr )
                SelectObject(m_hMemDC[i], m_hOldBitmap[i]);

            DeleteDC(m_hMemDC[i]);
            m_hMemDC[i] = nullptr;
        }

        if( m_hBitmap[i] != nullptr )
        {
            DeleteObject(m_hBitmap[i]);
            m_hBitmap[i] = nullptr;
        }

        m_pPixels[i] = nullptr;
    }

    // Update dimensions
    m_width = width;
    m_height = height;

    // Recreate both DIB sections
    BITMAPINFO bmi;
    std::memset(&bmi, 0, sizeof(bmi));
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = m_width;
    bmi.bmiHeader.biHeight = -m_height;     // Negative = top-down
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    for( int i = 0; i < 2; ++i )
    {
        m_hMemDC[i] = CreateCompatibleDC(m_hDC);
        if( m_hMemDC[i] == nullptr )
            throw NExcept::CCriticalException("Windows Framebuffer Error!",
                "Failed to create compatible DC on resize.");

        m_hBitmap[i] = CreateDIBSection(
            m_hDC, &bmi, DIB_RGB_COLORS,
            reinterpret_cast<void**>(&m_pPixels[i]),
            nullptr, 0 );

        if( m_hBitmap[i] == nullptr || m_pPixels[i] == nullptr )
            throw NExcept::CCriticalException("Windows Framebuffer Error!",
                "Failed to create DIB section on resize.");

        m_hOldBitmap[i] = static_cast<HBITMAP>(SelectObject(m_hMemDC[i], m_hBitmap[i]));

        std::memset(m_pPixels[i], 0, m_width * m_height * sizeof(uint32_t));
    }

    // Reset back buffer index
    m_backIndex = 0;

}


/************************************************************************
*    desc:  Blit the back buffer to the window and swap
************************************************************************/
void CWindowsFrameBuffer::Flip()
{
    BitBlt(m_hDC, 0, 0, m_width, m_height, m_hMemDC[m_backIndex], 0, 0, SRCCOPY);

    // Swap to the other buffer
    m_backIndex = 1 - m_backIndex;

}

#endif
