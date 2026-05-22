
/************************************************************************
*    FILE NAME:       windowswindow.h
*
*    DESCRIPTION:     Windows window implementation using Win32 API
************************************************************************/

#ifndef __windowswindow_h__
#define __windowswindow_h__

#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// Standard lib dependencies
#include <memory>
#include <string>

// Game lib dependencies
#include <system/iwindow.h>

// Forward declaration(s)
class CWindowsFrameBuffer;

class CWindowsWindow : public IWindow
{
public:

    // Constructor
    CWindowsWindow();

    // Destructor
    ~CWindowsWindow();

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

    // Win32 window procedure (must be public for static callback)
    LRESULT HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:

    // Window handle
    HWND m_hWnd;

    // Device context
    HDC m_hDC;

    // Window class name
    std::string m_className;

    // Owned framebuffer
    std::unique_ptr<CWindowsFrameBuffer> m_upFrameBuffer;

    // Track state
    bool m_fullscreen;

    // Saved window placement for fullscreen toggle
    WINDOWPLACEMENT m_savedPlacement;
    LONG m_savedStyle;
};

#endif

#endif
