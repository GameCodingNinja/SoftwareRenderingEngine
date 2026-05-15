
/************************************************************************
*    FILE NAME:       windowsframebuffer.cpp
*
*    DESCRIPTION:     Windows framebuffer implementation using DIB section
************************************************************************/

#ifdef _WIN32

// Physical component dependency
#include <system/windowsframebuffer.h>

// Standard lib dependencies
#include <cstring>

// Game lib dependencies
#include <utilities/exceptionhandling.h>

/************************************************************************
*    desc:  Constructor — creates a DIB section for the pixel buffer
************************************************************************/
CWindowsFrameBuffer::CWindowsFrameBuffer(HWND hWnd, HDC hDC, int width, int height) :
    m_hWnd(hWnd),
    m_hDC(hDC),
    m_hMemDC(nullptr),
    m_hBitmap(nullptr),
    m_hOldBitmap(nullptr),
    m_pPixels(nullptr),
    m_width(width),
    m_height(height)
{
    // Create a memory DC compatible with the window DC
    m_hMemDC = CreateCompatibleDC(m_hDC);
    if( m_hMemDC == nullptr )
        throw NExcept::CCriticalException("Windows Framebuffer Error!",
            "Failed to create compatible DC.");

    // Setup the bitmap info for a 32-bit top-down DIB
    BITMAPINFO bmi;
    std::memset(&bmi, 0, sizeof(bmi));
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = m_width;
    bmi.bmiHeader.biHeight = -m_height;     // Negative = top-down
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    // Create the DIB section — m_pPixels will point to the pixel data
    m_hBitmap = CreateDIBSection(
        m_hDC, &bmi, DIB_RGB_COLORS,
        reinterpret_cast<void**>(&m_pPixels),
        nullptr, 0 );

    if( m_hBitmap == nullptr || m_pPixels == nullptr )
        throw NExcept::CCriticalException("Windows Framebuffer Error!",
            "Failed to create DIB section.");

    // Select the bitmap into the memory DC
    m_hOldBitmap = static_cast<HBITMAP>(SelectObject(m_hMemDC, m_hBitmap));

    // Clear to black
    std::memset(m_pPixels, 0, m_width * m_height * sizeof(uint32_t));

}   // Constructor


/************************************************************************
*    desc:  Destructor
************************************************************************/
CWindowsFrameBuffer::~CWindowsFrameBuffer()
{
    if( m_hMemDC != nullptr )
    {
        if( m_hOldBitmap != nullptr )
            SelectObject(m_hMemDC, m_hOldBitmap);

        DeleteDC(m_hMemDC);
        m_hMemDC = nullptr;
    }

    if( m_hBitmap != nullptr )
    {
        DeleteObject(m_hBitmap);
        m_hBitmap = nullptr;
    }

    m_pPixels = nullptr;

}   // Destructor


/************************************************************************
*    desc:  Get the raw pixel buffer
************************************************************************/
uint32_t* CWindowsFrameBuffer::GetPixels()
{
    return m_pPixels;

}   // GetPixels


/************************************************************************
*    desc:  Get framebuffer width
************************************************************************/
int CWindowsFrameBuffer::GetWidth() const
{
    return m_width;

}   // GetWidth


/************************************************************************
*    desc:  Get framebuffer height
************************************************************************/
int CWindowsFrameBuffer::GetHeight() const
{
    return m_height;

}   // GetHeight


/************************************************************************
*    desc:  Clear the pixel buffer to zero (black)
************************************************************************/
void CWindowsFrameBuffer::Clear()
{
    std::memset(m_pPixels, 0, m_width * m_height * sizeof(uint32_t));

}   // Clear


/************************************************************************
*    desc:  Blit the DIB to the window
************************************************************************/
void CWindowsFrameBuffer::Flip()
{
    BitBlt(m_hDC, 0, 0, m_width, m_height, m_hMemDC, 0, 0, SRCCOPY);

}   // Flip

#endif  // _WIN32
