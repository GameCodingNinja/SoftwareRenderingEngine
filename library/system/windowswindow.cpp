
/************************************************************************
*    FILE NAME:       windowswindow.cpp
*
*    DESCRIPTION:     Windows window implementation using Win32 API
************************************************************************/

#ifdef _WIN32

// Physical component dependency
#include <system/windowswindow.h>
#include <system/windowsframebuffer.h>

// Standard lib dependencies
#include <cstring>

// Game lib dependencies
#include <system/event.h>
#include <system/eventqueue.h>
#include <utilities/exceptionhandling.h>
#include <utilities/genfunc.h>


/************************************************************************
*    desc:  Static WndProc — routes messages to the instance
************************************************************************/
static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    CWindowsWindow* pWindow = nullptr;

    if( msg == WM_NCCREATE )
    {
        // Store the instance pointer passed via CreateWindowEx
        CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
        pWindow = static_cast<CWindowsWindow*>(pCreate->lpCreateParams);
        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWindow));
    }
    else
    {
        pWindow = reinterpret_cast<CWindowsWindow*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
    }

    if( pWindow != nullptr )
        return pWindow->HandleMessage(hWnd, msg, wParam, lParam);

    return DefWindowProc(hWnd, msg, wParam, lParam);
}


/************************************************************************
*    desc:  Translate Win32 virtual key code to engine key code
*           For now, pass through the VK code directly.
************************************************************************/
static EKeyCode TranslateVirtualKey(WPARAM vk)
{
    // Printable ASCII: Windows VK codes for 0-9 and A-Z match ASCII uppercase.
    // Our EKeyCode uses lowercase for letters, so convert.
    if( vk >= 'A' && vk <= 'Z' )
        return static_cast<EKeyCode>(vk - 'A' + KEY_A);

    if( vk >= '0' && vk <= '9' )
        return static_cast<EKeyCode>(vk);

    switch( vk )
    {
        case VK_BACK:       return KEY_BACKSPACE;
        case VK_TAB:        return KEY_TAB;
        case VK_RETURN:     return KEY_RETURN;
        case VK_ESCAPE:     return KEY_ESCAPE;
        case VK_SPACE:      return KEY_SPACE;
        case VK_DELETE:     return KEY_DELETE;

        // Navigation
        case VK_UP:         return KEY_UP;
        case VK_DOWN:       return KEY_DOWN;
        case VK_LEFT:       return KEY_LEFT;
        case VK_RIGHT:      return KEY_RIGHT;
        case VK_INSERT:     return KEY_INSERT;
        case VK_HOME:       return KEY_HOME;
        case VK_END:        return KEY_END;
        case VK_PRIOR:      return KEY_PAGEUP;
        case VK_NEXT:       return KEY_PAGEDOWN;

        // Function keys
        case VK_F1:         return KEY_F1;
        case VK_F2:         return KEY_F2;
        case VK_F3:         return KEY_F3;
        case VK_F4:         return KEY_F4;
        case VK_F5:         return KEY_F5;
        case VK_F6:         return KEY_F6;
        case VK_F7:         return KEY_F7;
        case VK_F8:         return KEY_F8;
        case VK_F9:         return KEY_F9;
        case VK_F10:        return KEY_F10;
        case VK_F11:        return KEY_F11;
        case VK_F12:        return KEY_F12;

        // Modifiers
        case VK_LSHIFT:     return KEY_LSHIFT;
        case VK_RSHIFT:     return KEY_RSHIFT;
        case VK_LCONTROL:   return KEY_LCTRL;
        case VK_RCONTROL:   return KEY_RCTRL;
        case VK_LMENU:      return KEY_LALT;
        case VK_RMENU:      return KEY_RALT;

        // Lock keys
        case VK_CAPITAL:    return KEY_CAPSLOCK;
        case VK_NUMLOCK:    return KEY_NUMLOCK;
        case VK_SCROLL:     return KEY_SCROLLLOCK;

        // Special
        case VK_SNAPSHOT:   return KEY_PRINTSCREEN;
        case VK_PAUSE:      return KEY_PAUSE;

        // Numpad
        case VK_NUMPAD0:    return KEY_KP_0;
        case VK_NUMPAD1:    return KEY_KP_1;
        case VK_NUMPAD2:    return KEY_KP_2;
        case VK_NUMPAD3:    return KEY_KP_3;
        case VK_NUMPAD4:    return KEY_KP_4;
        case VK_NUMPAD5:    return KEY_KP_5;
        case VK_NUMPAD6:    return KEY_KP_6;
        case VK_NUMPAD7:    return KEY_KP_7;
        case VK_NUMPAD8:    return KEY_KP_8;
        case VK_NUMPAD9:    return KEY_KP_9;
        case VK_DECIMAL:    return KEY_KP_PERIOD;
        case VK_DIVIDE:     return KEY_KP_DIVIDE;
        case VK_MULTIPLY:   return KEY_KP_MULTIPLY;
        case VK_SUBTRACT:   return KEY_KP_MINUS;
        case VK_ADD:        return KEY_KP_PLUS;

        // Punctuation (OEM keys — US layout)
        case VK_OEM_1:      return KEY_SEMICOLON;
        case VK_OEM_PLUS:   return KEY_EQUALS;
        case VK_OEM_COMMA:  return KEY_COMMA;
        case VK_OEM_MINUS:  return KEY_MINUS;
        case VK_OEM_PERIOD: return KEY_PERIOD;
        case VK_OEM_2:      return KEY_SLASH;
        case VK_OEM_3:      return KEY_GRAVE;
        case VK_OEM_4:      return KEY_LEFTBRACKET;
        case VK_OEM_5:      return KEY_BACKSLASH;
        case VK_OEM_6:      return KEY_RIGHTBRACKET;
        case VK_OEM_7:      return KEY_APOSTROPHE;

        default:            return KEY_UNKNOWN;
    }

}


/************************************************************************
*    desc:  Map Win32 mouse button message to button index
*           1=left, 2=middle, 3=right
************************************************************************/
static EMouseButton MapMouseButton(UINT msg)
{
    switch( msg )
    {
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:   return MOUSE_BUTTON_LEFT;
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:   return MOUSE_BUTTON_MIDDLE;
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:   return MOUSE_BUTTON_RIGHT;
        default:              return static_cast<EMouseButton>(0);
    }

}


/************************************************************************
*    desc:  Constructor
************************************************************************/
CWindowsWindow::CWindowsWindow() :
    m_hWnd(nullptr),
    m_hDC(nullptr),
    m_className("SoftwareRenderWindow"),
    m_fullscreen(false),
    m_savedStyle(0)
{
    std::memset(&m_savedPlacement, 0, sizeof(m_savedPlacement));
    m_savedPlacement.length = sizeof(WINDOWPLACEMENT);

}


/************************************************************************
*    desc:  Destructor
************************************************************************/
CWindowsWindow::~CWindowsWindow()
{
    Destroy();

}


/************************************************************************
*    desc:  Create the window
************************************************************************/
void CWindowsWindow::Create(int width, int height, const char* title)
{
    HINSTANCE hInstance = GetModuleHandle(nullptr);

    // Register window class
    WNDCLASSEXA wc;
    std::memset(&wc, 0, sizeof(wc));
    wc.cbSize = sizeof(WNDCLASSEXA);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
    wc.lpszClassName = m_className.c_str();

    if( !RegisterClassExA(&wc) )
    {
        DWORD err = GetLastError();
        throw NExcept::CCriticalException("Windows Window Error!",
            NGenFunc::FormatString("Failed to register window class. GetLastError: %d", err));
    }

    // Calculate window rect to get the desired client area size
    RECT rect = { 0, 0, width, height };
    DWORD style = WS_OVERLAPPEDWINDOW;
    AdjustWindowRect(&rect, style, FALSE);

    int windowWidth = rect.right - rect.left;
    int windowHeight = rect.bottom - rect.top;

    // Center the window on the screen
    int screenW = GetSystemMetrics(SM_CXSCREEN);
    int screenH = GetSystemMetrics(SM_CYSCREEN);
    int posX = (screenW - windowWidth) / 2;
    int posY = (screenH - windowHeight) / 2;

    // Create the window (pass 'this' so WndProc can find us)
    m_hWnd = CreateWindowExA(
        0,
        m_className.c_str(),
        (title != nullptr) ? title : "",
        style,
        posX, posY,
        windowWidth, windowHeight,
        nullptr,        // parent
        nullptr,        // menu
        hInstance,
        this );         // lpParam → stored in GWLP_USERDATA

    if( m_hWnd == nullptr )
    {
        DWORD err = GetLastError();
        throw NExcept::CCriticalException("Windows Window Error!",
            NGenFunc::FormatString("Failed to create window. GetLastError: %d", err));
    }

    // Get the device context
    m_hDC = GetDC(m_hWnd);
    if( m_hDC == nullptr )
        throw NExcept::CCriticalException("Windows Window Error!",
            "Failed to get device context.");

    // Create the framebuffer
    m_upFrameBuffer = std::make_unique<CWindowsFrameBuffer>(
        m_hWnd, m_hDC, width, height);

}


/************************************************************************
*    desc:  Destroy the window and free resources
************************************************************************/
void CWindowsWindow::Destroy()
{
    m_upFrameBuffer.reset();

    if( m_hDC != nullptr && m_hWnd != nullptr )
    {
        ReleaseDC(m_hWnd, m_hDC);
        m_hDC = nullptr;
    }

    if( m_hWnd != nullptr )
    {
        DestroyWindow(m_hWnd);
        m_hWnd = nullptr;
    }

    UnregisterClassA(m_className.c_str(), GetModuleHandle(nullptr));

}


/************************************************************************
*    desc:  Show or hide the window
************************************************************************/
void CWindowsWindow::Show(bool visible)
{
    if( m_hWnd == nullptr )
        return;

    ShowWindow(m_hWnd, visible ? SW_SHOW : SW_HIDE);
    if( visible )
        UpdateWindow(m_hWnd);

}


/************************************************************************
*    desc:  Set the window title
************************************************************************/
void CWindowsWindow::SetTitle(const std::string& title)
{
    if( m_hWnd != nullptr )
        SetWindowTextA(m_hWnd, title.c_str());

}


/************************************************************************
*    desc:  Set full screen mode
*           Saves/restores window placement and style for toggling
************************************************************************/
void CWindowsWindow::SetFullScreen(bool fullscreen)
{
    if( m_hWnd == nullptr || m_fullscreen == fullscreen )
        return;

    m_fullscreen = fullscreen;

    if( fullscreen )
    {
        // Save current window state
        m_savedStyle = GetWindowLong(m_hWnd, GWL_STYLE);
        GetWindowPlacement(m_hWnd, &m_savedPlacement);

        // Remove window decorations and go fullscreen
        SetWindowLong(m_hWnd, GWL_STYLE, m_savedStyle & ~WS_OVERLAPPEDWINDOW);

        MONITORINFO mi;
        mi.cbSize = sizeof(mi);
        GetMonitorInfo(MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTOPRIMARY), &mi);

        SetWindowPos(m_hWnd, HWND_TOP,
            mi.rcMonitor.left, mi.rcMonitor.top,
            mi.rcMonitor.right - mi.rcMonitor.left,
            mi.rcMonitor.bottom - mi.rcMonitor.top,
            SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
    }
    else
    {
        // Restore window decorations and position
        SetWindowLong(m_hWnd, GWL_STYLE, m_savedStyle);
        SetWindowPlacement(m_hWnd, &m_savedPlacement);
        SetWindowPos(m_hWnd, nullptr, 0, 0, 0, 0,
            SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
            SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
    }

}


/************************************************************************
*    desc:  Poll Win32 messages and push to the event queue
************************************************************************/
void CWindowsWindow::PollEvents()
{
    MSG msg;

    while( PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE) )
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

}


/************************************************************************
*    desc:  Get the framebuffer owned by this window
************************************************************************/
IFrameBuffer* CWindowsWindow::GetFrameBuffer()
{
    return m_upFrameBuffer.get();

}


/************************************************************************
*    desc:  Handle Win32 messages — translate to engine events
************************************************************************/
LRESULT CWindowsWindow::HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    CEvent event;
    std::memset(&event, 0, sizeof(event));

    switch( msg )
    {
        case WM_CLOSE:
        {
            event.type = EVENT_QUIT;
            CEventQueue::Instance().PushEvent(event);
            return 0;
        }

        case WM_SIZE:
        {
            int newWidth = LOWORD(lParam);
            int newHeight = HIWORD(lParam);

            // Only push the event on actual size changes with valid dimensions
            if( newWidth > 0 && newHeight > 0 &&
                m_upFrameBuffer != nullptr &&
                (newWidth != m_upFrameBuffer->getWidth() ||
                 newHeight != m_upFrameBuffer->getHeight()) )
            {
                event.resize.type = EVENT_WINDOW_RESIZE;
                event.resize.width = newWidth;
                event.resize.height = newHeight;
                CEventQueue::Instance().PushEvent(event);
            }
            return 0;
        }

        case WM_KEYDOWN:
        {
            event.key.type = EVENT_KEY_DOWN;
            event.key.keyCode = TranslateVirtualKey(wParam);
            event.key.repeat = (lParam & 0x40000000) != 0;
            CEventQueue::Instance().PushEvent(event);
            return 0;
        }

        case WM_KEYUP:
        {
            event.key.type = EVENT_KEY_UP;
            event.key.keyCode = TranslateVirtualKey(wParam);
            event.key.repeat = false;
            CEventQueue::Instance().PushEvent(event);
            return 0;
        }

        case WM_MOUSEMOVE:
        {
            event.motion.type = EVENT_MOUSE_MOTION;
            event.motion.x = LOWORD(lParam);
            event.motion.y = HIWORD(lParam);
            event.motion.xrel = 0;
            event.motion.yrel = 0;
            CEventQueue::Instance().PushEvent(event);
            return 0;
        }

        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN:
        {
            event.button.type = EVENT_MOUSE_BUTTON_DOWN;
            event.button.button = MapMouseButton(msg);
            event.button.x = LOWORD(lParam);
            event.button.y = HIWORD(lParam);
            CEventQueue::Instance().PushEvent(event);
            return 0;
        }

        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
        case WM_MBUTTONUP:
        {
            event.button.type = EVENT_MOUSE_BUTTON_UP;
            event.button.button = MapMouseButton(msg);
            event.button.x = LOWORD(lParam);
            event.button.y = HIWORD(lParam);
            CEventQueue::Instance().PushEvent(event);
            return 0;
        }

        case WM_DESTROY:
        {
            PostQuitMessage(0);
            return 0;
        }

        default:
            break;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);

}

#endif
