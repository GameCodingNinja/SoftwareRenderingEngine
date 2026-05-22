
/************************************************************************
*    FILE NAME:       event.h
*
*    DESCRIPTION:     Custom event types and structures for the engine.
************************************************************************/

#ifndef __event_h__
#define __event_h__

// Standard lib dependencies
#include <cstdint>

/************************************************************************
*    Event type enum, using hex ranges for grouping
************************************************************************/
enum EEventType : uint32_t
{
    EVENT_NONE              = 0,

    // Application events (0x100 range)
    EVENT_QUIT              = 0x100,

    // Keyboard events (0x300 range)
    EVENT_KEY_DOWN          = 0x300,
    EVENT_KEY_UP            = 0x301,

    // Mouse events (0x400 range)
    EVENT_MOUSE_MOTION      = 0x400,
    EVENT_MOUSE_BUTTON_DOWN = 0x401,
    EVENT_MOUSE_BUTTON_UP   = 0x402,

    // User events (0x8000 range)
    EVENT_USER              = 0x8000,
};


/************************************************************************
*    Platform-independent key codes
*    Printable ASCII keys use their ASCII values directly (0x00–0x7F).
*    Non-printable keys use values starting at 0x100 to avoid collision.
************************************************************************/
enum EKeyCode : int
{
    KEY_UNKNOWN         = 0,

    // --- Printable keys (ASCII values) ---
    KEY_BACKSPACE       = 0x08,
    KEY_TAB             = 0x09,
    KEY_RETURN          = 0x0D,
    KEY_ESCAPE          = 0x1B,
    KEY_SPACE           = 0x20,
    KEY_EXCLAIM         = 0x21,     // !
    KEY_QUOTEDBL        = 0x22,     // "
    KEY_HASH            = 0x23,     // #
    KEY_DOLLAR          = 0x24,     // $
    KEY_PERCENT         = 0x25,     // %
    KEY_AMPERSAND       = 0x26,     // &
    KEY_APOSTROPHE      = 0x27,     // '
    KEY_LEFTPAREN       = 0x28,     // (
    KEY_RIGHTPAREN      = 0x29,     // )
    KEY_ASTERISK        = 0x2A,     // *
    KEY_PLUS            = 0x2B,     // +
    KEY_COMMA           = 0x2C,     // ,
    KEY_MINUS           = 0x2D,     // -
    KEY_PERIOD          = 0x2E,     // .
    KEY_SLASH           = 0x2F,     // /
    KEY_0               = 0x30,
    KEY_1               = 0x31,
    KEY_2               = 0x32,
    KEY_3               = 0x33,
    KEY_4               = 0x34,
    KEY_5               = 0x35,
    KEY_6               = 0x36,
    KEY_7               = 0x37,
    KEY_8               = 0x38,
    KEY_9               = 0x39,
    KEY_COLON           = 0x3A,     // :
    KEY_SEMICOLON       = 0x3B,     // ;
    KEY_LESS            = 0x3C,     // <
    KEY_EQUALS          = 0x3D,     // =
    KEY_GREATER         = 0x3E,     // >
    KEY_QUESTION        = 0x3F,     // ?
    KEY_AT              = 0x40,     // @
    KEY_LEFTBRACKET     = 0x5B,     // [
    KEY_BACKSLASH       = 0x5C,     // 
    KEY_RIGHTBRACKET    = 0x5D,     // ]
    KEY_CARET           = 0x5E,     // ^
    KEY_UNDERSCORE      = 0x5F,     // _
    KEY_GRAVE           = 0x60,     // `
    KEY_A               = 0x61,     // a
    KEY_B               = 0x62,
    KEY_C               = 0x63,
    KEY_D               = 0x64,
    KEY_E               = 0x65,
    KEY_F               = 0x66,
    KEY_G               = 0x67,
    KEY_H               = 0x68,
    KEY_I               = 0x69,
    KEY_J               = 0x6A,
    KEY_K               = 0x6B,
    KEY_L               = 0x6C,
    KEY_M               = 0x6D,
    KEY_N               = 0x6E,
    KEY_O               = 0x6F,
    KEY_P               = 0x70,
    KEY_Q               = 0x71,
    KEY_R               = 0x72,
    KEY_S               = 0x73,
    KEY_T               = 0x74,
    KEY_U               = 0x75,
    KEY_V               = 0x76,
    KEY_W               = 0x77,
    KEY_X               = 0x78,
    KEY_Y               = 0x79,
    KEY_Z               = 0x7A,
    KEY_DELETE           = 0x7F,

    // --- Non-printable keys (0x100+) ---

    // Navigation
    KEY_UP              = 0x100,
    KEY_DOWN            = 0x101,
    KEY_LEFT            = 0x102,
    KEY_RIGHT           = 0x103,
    KEY_INSERT          = 0x104,
    KEY_HOME            = 0x105,
    KEY_END             = 0x106,
    KEY_PAGEUP          = 0x107,
    KEY_PAGEDOWN        = 0x108,

    // Function keys
    KEY_F1              = 0x110,
    KEY_F2              = 0x111,
    KEY_F3              = 0x112,
    KEY_F4              = 0x113,
    KEY_F5              = 0x114,
    KEY_F6              = 0x115,
    KEY_F7              = 0x116,
    KEY_F8              = 0x117,
    KEY_F9              = 0x118,
    KEY_F10             = 0x119,
    KEY_F11             = 0x11A,
    KEY_F12             = 0x11B,

    // Modifier keys
    KEY_LSHIFT          = 0x130,
    KEY_RSHIFT          = 0x131,
    KEY_LCTRL           = 0x132,
    KEY_RCTRL           = 0x133,
    KEY_LALT            = 0x134,
    KEY_RALT            = 0x135,

    // Lock keys
    KEY_CAPSLOCK        = 0x140,
    KEY_NUMLOCK         = 0x141,
    KEY_SCROLLLOCK      = 0x142,

    // Special
    KEY_PRINTSCREEN     = 0x150,
    KEY_PAUSE           = 0x151,

    // Numpad
    KEY_KP_0            = 0x160,
    KEY_KP_1            = 0x161,
    KEY_KP_2            = 0x162,
    KEY_KP_3            = 0x163,
    KEY_KP_4            = 0x164,
    KEY_KP_5            = 0x165,
    KEY_KP_6            = 0x166,
    KEY_KP_7            = 0x167,
    KEY_KP_8            = 0x168,
    KEY_KP_9            = 0x169,
    KEY_KP_PERIOD       = 0x16A,
    KEY_KP_DIVIDE       = 0x16B,
    KEY_KP_MULTIPLY     = 0x16C,
    KEY_KP_MINUS        = 0x16D,
    KEY_KP_PLUS         = 0x16E,
    KEY_KP_ENTER        = 0x16F,
    KEY_KP_EQUALS       = 0x170,
};


/************************************************************************
*    Mouse button constants
************************************************************************/
enum EMouseButton : uint8_t
{
    MOUSE_BUTTON_LEFT   = 1,
    MOUSE_BUTTON_MIDDLE = 2,
    MOUSE_BUTTON_RIGHT  = 3,
};


/************************************************************************
*    Key event data
************************************************************************/
struct CKeyEvent
{
    EEventType type;        // EVENT_KEY_DOWN or EVENT_KEY_UP
    EKeyCode keyCode;       // Platform-independent key code
    bool repeat;            // true if this is a key repeat
};


/************************************************************************
*    Mouse motion event data
************************************************************************/
struct CMouseMotionEvent
{
    EEventType type;        // EVENT_MOUSE_MOTION
    int x;                  // X coordinate, relative to window
    int y;                  // Y coordinate, relative to window
    int xrel;               // Relative motion in X
    int yrel;               // Relative motion in Y
};


/************************************************************************
*    Mouse button event data
************************************************************************/
struct CMouseButtonEvent
{
    EEventType type;        // EVENT_MOUSE_BUTTON_DOWN or EVENT_MOUSE_BUTTON_UP
    EMouseButton button;    // MOUSE_BUTTON_LEFT, MOUSE_BUTTON_MIDDLE, MOUSE_BUTTON_RIGHT
    int x;                  // X coordinate, relative to window
    int y;                  // Y coordinate, relative to window
};


/************************************************************************
*    User-defined event data
************************************************************************/
struct CUserEvent
{
    EEventType type;        // EVENT_USER or higher
    int code;               // User-defined event code
    void* data1;            // User-defined data pointer
    void* data2;            // User-defined data pointer
};


/************************************************************************
*    Union of all event types
*    The type field is shared across all sub-structs so you can
*    check event.type then access the appropriate member.
************************************************************************/
union CEvent
{
    EEventType type;                // Event type, shared with all events
    CKeyEvent key;                  // Keyboard event data
    CMouseMotionEvent motion;       // Mouse motion event data
    CMouseButtonEvent button;       // Mouse button event data
    CUserEvent user;                // User-defined event data
};

#endif
