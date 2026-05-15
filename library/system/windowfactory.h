
/************************************************************************
*    FILE NAME:       windowfactory.h
*
*    DESCRIPTION:     Factory function to create the appropriate
*                     native window based on the platform and
*                     available display server.
************************************************************************/

#ifndef __windowfactory_h__
#define __windowfactory_h__

// Standard lib dependencies
#include <memory>

// Forward declaration(s)
class IWindow;

// Create a native window for the current platform
// Linux: tries Wayland first, falls back to X11
// Windows: creates a Win32 window
std::unique_ptr<IWindow> CreateNativeWindow();

#endif  // __windowfactory_h__
