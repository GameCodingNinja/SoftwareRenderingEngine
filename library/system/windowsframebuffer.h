
/************************************************************************
*    FILE NAME:       windowsframebuffer.h
*
*    DESCRIPTION:     Windows framebuffer implementation using DIB section
************************************************************************/

#ifndef __windowsframebuffer_h__
#define __windowsframebuffer_h__

#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// Game lib dependencies
#include <system/iframebuffer.h>

class CWindowsFrameBuffer : public IFrameBuffer
{
public:

    // Constructor
    CWindowsFrameBuffer(HWND hWnd, HDC hDC, int width, int height);

    // Destructor
    ~CWindowsFrameBuffer();

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

    // Window handles (not owned — the window class owns these)
    HWND m_hWnd;
    HDC m_hDC;

    // Memory DC and DIB section
    HDC m_hMemDC;
    HBITMAP m_hBitmap;
    HBITMAP m_hOldBitmap;

    // Pixel buffer (points into the DIB section)
    uint32_t* m_pPixels;

    // Dimensions
    int m_width;
    int m_height;
};

#endif  // _WIN32

#endif  // __windowsframebuffer_h__
