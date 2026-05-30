
/************************************************************************
*    FILE NAME:       windowsframebuffer.h
*
*    DESCRIPTION:     Windows framebuffer implementation using DIB section
*                     with double buffering for tear-free rendering.
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

    // Get the raw pixel buffer (returns the back buffer)
    uint32_t* getPixels() override;

    // Get framebuffer dimensions
    int getWidth() const override;
    int getHeight() const override;

    // Clear the pixel buffer to zero (black)
    void Clear() override;

    // Resize the framebuffer to new dimensions
    void Resize(int width, int height) override;

    // Swap buffers and display the back buffer contents
    void Flip() override;

private:

    // Window handles (not owned — the window class owns these)
    HWND m_hWnd;
    HDC m_hDC;

    // Double buffer: two memory DCs and DIB sections
    HDC m_hMemDC[2];
    HBITMAP m_hBitmap[2];
    HBITMAP m_hOldBitmap[2];
    uint32_t* m_pPixels[2];

    // Which buffer the engine is drawing to (0 or 1)
    int m_backIndex;

    // Dimensions
    int m_width;
    int m_height;
};

#endif

#endif
