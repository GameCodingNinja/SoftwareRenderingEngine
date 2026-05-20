
/************************************************************************
*    FILE NAME:       x11window.cpp
*
*    DESCRIPTION:     X11 window implementation using Xlib
************************************************************************/

#ifdef __linux__

// Physical component dependency
#include <system/x11window.h>
#include <system/x11framebuffer.h>

// Standard lib dependencies
#include <cstring>

// X11 lib dependencies
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>

// Game lib dependencies
#include <system/event.h>
#include <system/eventqueue.h>
#include <utilities/exceptionhandling.h>
#include <utilities/genfunc.h>
#include <utilities/settings.h>

/************************************************************************
*    desc:  Translate X11 keysym to engine key code
*           For now, pass through the X11 keysym directly.
*           A full mapping table can be added later.
************************************************************************/
static EKeyCode TranslateKeysym(KeySym sym)
{
    // Printable ASCII maps directly
    if( sym >= 0x20 && sym <= 0x7E )
        return static_cast<EKeyCode>(sym);

    switch( sym )
    {
        case XK_BackSpace:    return KEY_BACKSPACE;
        case XK_Tab:          return KEY_TAB;
        case XK_Return:       return KEY_RETURN;
        case XK_Escape:       return KEY_ESCAPE;
        case XK_Delete:       return KEY_DELETE;
        case XK_Up:           return KEY_UP;
        case XK_Down:         return KEY_DOWN;
        case XK_Left:         return KEY_LEFT;
        case XK_Right:        return KEY_RIGHT;
        case XK_Insert:       return KEY_INSERT;
        case XK_Home:         return KEY_HOME;
        case XK_End:          return KEY_END;
        case XK_Page_Up:      return KEY_PAGEUP;
        case XK_Page_Down:    return KEY_PAGEDOWN;
        case XK_F1:           return KEY_F1;
        case XK_F2:           return KEY_F2;
        case XK_F3:           return KEY_F3;
        case XK_F4:           return KEY_F4;
        case XK_F5:           return KEY_F5;
        case XK_F6:           return KEY_F6;
        case XK_F7:           return KEY_F7;
        case XK_F8:           return KEY_F8;
        case XK_F9:           return KEY_F9;
        case XK_F10:          return KEY_F10;
        case XK_F11:          return KEY_F11;
        case XK_F12:          return KEY_F12;
        case XK_Shift_L:      return KEY_LSHIFT;
        case XK_Shift_R:      return KEY_RSHIFT;
        case XK_Control_L:    return KEY_LCTRL;
        case XK_Control_R:    return KEY_RCTRL;
        case XK_Alt_L:        return KEY_LALT;
        case XK_Alt_R:        return KEY_RALT;
        case XK_Caps_Lock:    return KEY_CAPSLOCK;
        case XK_Num_Lock:     return KEY_NUMLOCK;
        case XK_Scroll_Lock:  return KEY_SCROLLLOCK;
        case XK_Print:        return KEY_PRINTSCREEN;
        case XK_Pause:        return KEY_PAUSE;
        case XK_KP_0:         return KEY_KP_0;
        case XK_KP_1:         return KEY_KP_1;
        case XK_KP_2:         return KEY_KP_2;
        case XK_KP_3:         return KEY_KP_3;
        case XK_KP_4:         return KEY_KP_4;
        case XK_KP_5:         return KEY_KP_5;
        case XK_KP_6:         return KEY_KP_6;
        case XK_KP_7:         return KEY_KP_7;
        case XK_KP_8:         return KEY_KP_8;
        case XK_KP_9:         return KEY_KP_9;
        case XK_KP_Decimal:   return KEY_KP_PERIOD;
        case XK_KP_Divide:    return KEY_KP_DIVIDE;
        case XK_KP_Multiply:  return KEY_KP_MULTIPLY;
        case XK_KP_Subtract:  return KEY_KP_MINUS;
        case XK_KP_Add:       return KEY_KP_PLUS;
        case XK_KP_Enter:     return KEY_KP_ENTER;
        case XK_KP_Equal:     return KEY_KP_EQUALS;
        default:              return KEY_UNKNOWN;
    }

}   // TranslateKeysym


/************************************************************************
*    desc:  Constructor
************************************************************************/
CX11Window::CX11Window() :
    m_pDisplay(nullptr),
    m_window(0),
    m_gc(nullptr),
    m_wmDeleteMessage(0),
    m_fullscreen(false)
{
}   // Constructor


/************************************************************************
*    desc:  Destructor
************************************************************************/
CX11Window::~CX11Window()
{
    Destroy();

}   // Destructor


/************************************************************************
*    desc:  Create the window
************************************************************************/
void CX11Window::Create(int width, int height, const char* title)
{
    // Open the X display
    m_pDisplay = XOpenDisplay(nullptr);
    if( m_pDisplay == nullptr )
        throw NExcept::CCriticalException("X11 Window Error!",
            "Failed to open X display.");

    int screen = DefaultScreen(m_pDisplay);
    Window rootWindow = RootWindow(m_pDisplay, screen);
    Visual* pVisual = DefaultVisual(m_pDisplay, screen);
    int depth = DefaultDepth(m_pDisplay, screen);

    // Set window attributes
    XSetWindowAttributes xattr;
    xattr.background_pixel = BlackPixel(m_pDisplay, screen);
    xattr.border_pixel = 0;
    xattr.colormap = XCreateColormap(m_pDisplay, rootWindow, pVisual, AllocNone);
    xattr.event_mask =
        KeyPressMask | KeyReleaseMask |
        ButtonPressMask | ButtonReleaseMask |
        PointerMotionMask |
        StructureNotifyMask |
        ExposureMask;

    // Create the window
    m_window = XCreateWindow(
        m_pDisplay,
        rootWindow,
        0, 0,                       // position (WM will override)
        width, height,
        0,                          // border width
        depth,
        InputOutput,
        pVisual,
        CWBackPixel | CWBorderPixel | CWColormap | CWEventMask,
        &xattr );

    if( m_window == 0 )
        throw NExcept::CCriticalException("X11 Window Error!",
            "Failed to create X11 window.");

    // Set the window title
    if( title != nullptr && title[0] != '\0' )
        XStoreName(m_pDisplay, m_window, title);

    // Register for WM_DELETE_WINDOW so clicking X sends ClientMessage
    m_wmDeleteMessage = XInternAtom(m_pDisplay, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(m_pDisplay, m_window, &m_wmDeleteMessage, 1);

    // Create the graphics context
    XGCValues gcv;
    gcv.graphics_exposures = False;
    m_gc = XCreateGC(m_pDisplay, m_window, GCGraphicsExposures, &gcv);

    if( m_gc == nullptr )
        throw NExcept::CCriticalException("X11 Window Error!",
            "Failed to create graphics context.");

    // Create the framebuffer with VSync setting
    m_upFrameBuffer = std::make_unique<CX11FrameBuffer>(
        m_pDisplay, m_window, m_gc, width, height,
        CSettings::Instance().getVSync());

}   // Create


/************************************************************************
*    desc:  Destroy the window and free resources
************************************************************************/
void CX11Window::Destroy()
{
    // Destroy framebuffer first (it references display/window/gc)
    m_upFrameBuffer.reset();

    if( m_pDisplay != nullptr )
    {
        if( m_gc != nullptr )
        {
            XFreeGC(m_pDisplay, m_gc);
            m_gc = nullptr;
        }

        if( m_window != 0 )
        {
            XDestroyWindow(m_pDisplay, m_window);
            m_window = 0;
        }

        XCloseDisplay(m_pDisplay);
        m_pDisplay = nullptr;
    }

}   // Destroy


/************************************************************************
*    desc:  Show or hide the window
************************************************************************/
void CX11Window::Show(bool visible)
{
    if( m_pDisplay == nullptr )
        return;

    if( visible )
    {
        XMapWindow(m_pDisplay, m_window);
        XFlush(m_pDisplay);
    }
    else
    {
        XUnmapWindow(m_pDisplay, m_window);
        XFlush(m_pDisplay);
    }

}   // Show


/************************************************************************
*    desc:  Set the window title
************************************************************************/
void CX11Window::SetTitle(const std::string& title)
{
    if( m_pDisplay == nullptr )
        return;

    XStoreName(m_pDisplay, m_window, title.c_str());
    XFlush(m_pDisplay);

}   // SetTitle


/************************************************************************
*    desc:  Set full screen mode using _NET_WM_STATE_FULLSCREEN
************************************************************************/
void CX11Window::SetFullScreen(bool fullscreen)
{
    if( m_pDisplay == nullptr || m_fullscreen == fullscreen )
        return;

    m_fullscreen = fullscreen;

    Atom wmState = XInternAtom(m_pDisplay, "_NET_WM_STATE", False);
    Atom wmFullscreen = XInternAtom(m_pDisplay, "_NET_WM_STATE_FULLSCREEN", False);

    XEvent xev;
    std::memset(&xev, 0, sizeof(xev));
    xev.type = ClientMessage;
    xev.xclient.window = m_window;
    xev.xclient.message_type = wmState;
    xev.xclient.format = 32;
    xev.xclient.data.l[0] = fullscreen ? 1 : 0;    // _NET_WM_STATE_ADD or _REMOVE
    xev.xclient.data.l[1] = wmFullscreen;
    xev.xclient.data.l[2] = 0;

    XSendEvent(
        m_pDisplay,
        DefaultRootWindow(m_pDisplay),
        False,
        SubstructureRedirectMask | SubstructureNotifyMask,
        &xev );

    XFlush(m_pDisplay);

}   // SetFullScreen


/************************************************************************
*    desc:  Poll native X11 events and push them to the event queue
************************************************************************/
void CX11Window::PollEvents()
{
    if( m_pDisplay == nullptr )
        return;

    XEvent xev;

    while( XPending(m_pDisplay) > 0 )
    {
        XNextEvent(m_pDisplay, &xev);

        CEvent event;
        std::memset(&event, 0, sizeof(event));

        switch( xev.type )
        {
            case KeyPress:
            {
                KeySym sym = XLookupKeysym(&xev.xkey, 0);
                event.key.type = EVENT_KEY_DOWN;
                event.key.keyCode = TranslateKeysym(sym);
                event.key.repeat = false;
                CEventQueue::Instance().PushEvent(event);
                break;
            }

            case KeyRelease:
            {
                // Check for auto-repeat: if the next event is a KeyPress
                // for the same key at the same time, it's a repeat
                bool isRepeat = false;
                if( XPending(m_pDisplay) > 0 )
                {
                    XEvent nextEvent;
                    XPeekEvent(m_pDisplay, &nextEvent);
                    if( nextEvent.type == KeyPress &&
                        nextEvent.xkey.keycode == xev.xkey.keycode &&
                        nextEvent.xkey.time == xev.xkey.time )
                    {
                        // Consume the paired KeyPress
                        XNextEvent(m_pDisplay, &nextEvent);
                        isRepeat = true;

                        // Push as a repeat key down
                        event.key.type = EVENT_KEY_DOWN;
                        KeySym sym = XLookupKeysym(&xev.xkey, 0);
                        event.key.keyCode = TranslateKeysym(sym);
                        event.key.repeat = true;
                        CEventQueue::Instance().PushEvent(event);
                        break;
                    }
                }

                if( !isRepeat )
                {
                    KeySym sym = XLookupKeysym(&xev.xkey, 0);
                    event.key.type = EVENT_KEY_UP;
                    event.key.keyCode = TranslateKeysym(sym);
                    event.key.repeat = false;
                    CEventQueue::Instance().PushEvent(event);
                }
                break;
            }

            case ButtonPress:
            {
                event.button.type = EVENT_MOUSE_BUTTON_DOWN;
                switch( xev.xbutton.button )
                {
                    case 1: event.button.button = MOUSE_BUTTON_LEFT; break;
                    case 2: event.button.button = MOUSE_BUTTON_MIDDLE; break;
                    case 3: event.button.button = MOUSE_BUTTON_RIGHT; break;
                    default: event.button.button = static_cast<EMouseButton>(xev.xbutton.button); break;
                }
                event.button.x = xev.xbutton.x;
                event.button.y = xev.xbutton.y;
                CEventQueue::Instance().PushEvent(event);
                break;
            }

            case ButtonRelease:
            {
                event.button.type = EVENT_MOUSE_BUTTON_UP;
                switch( xev.xbutton.button )
                {
                    case 1: event.button.button = MOUSE_BUTTON_LEFT; break;
                    case 2: event.button.button = MOUSE_BUTTON_MIDDLE; break;
                    case 3: event.button.button = MOUSE_BUTTON_RIGHT; break;
                    default: event.button.button = static_cast<EMouseButton>(xev.xbutton.button); break;
                }
                event.button.x = xev.xbutton.x;
                event.button.y = xev.xbutton.y;
                CEventQueue::Instance().PushEvent(event);
                break;
            }

            case MotionNotify:
            {
                event.motion.type = EVENT_MOUSE_MOTION;
                event.motion.x = xev.xmotion.x;
                event.motion.y = xev.xmotion.y;
                event.motion.xrel = 0;
                event.motion.yrel = 0;
                CEventQueue::Instance().PushEvent(event);
                break;
            }

            case ClientMessage:
            {
                if( static_cast<Atom>(xev.xclient.data.l[0]) == m_wmDeleteMessage )
                {
                    event.type = EVENT_QUIT;
                    CEventQueue::Instance().PushEvent(event);
                }
                break;
            }

            default:
                break;
        }
    }

}   // PollEvents


/************************************************************************
*    desc:  Get the framebuffer owned by this window
************************************************************************/
IFrameBuffer* CX11Window::GetFrameBuffer()
{
    return m_upFrameBuffer.get();

}   // GetFrameBuffer

#endif  // __linux__
