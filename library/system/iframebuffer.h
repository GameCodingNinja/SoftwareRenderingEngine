
/************************************************************************
*    FILE NAME:       iframebuffer.h
*
*    DESCRIPTION:     Pure virtual framebuffer interface
************************************************************************/

#ifndef __iframebuffer_h__
#define __iframebuffer_h__

// Standard lib dependencies
#include <cstdint>

class IFrameBuffer
{
public:

    virtual ~IFrameBuffer() = default;

    // Get the raw pixel buffer (XRGB 32-bit)
    virtual uint32_t* GetPixels() = 0;

    // Get framebuffer dimensions
    virtual int GetWidth() const = 0;
    virtual int GetHeight() const = 0;

    // Clear the pixel buffer to zero (black)
    virtual void Clear() = 0;

    // Display the pixel buffer contents in the window
    virtual void Flip() = 0;
};

#endif  // __iframebuffer_h__
