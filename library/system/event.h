
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
*    Event type enum
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
*    Key event data
************************************************************************/
struct CKeyEvent
{
    EEventType type;        // EVENT_KEY_DOWN or EVENT_KEY_UP
    int keyCode;            // Platform-independent key code
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
    uint8_t button;         // Button index (1=left, 2=middle, 3=right)
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

#endif  // __event_h__
