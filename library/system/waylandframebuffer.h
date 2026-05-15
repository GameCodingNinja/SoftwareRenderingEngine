
/************************************************************************
*    FILE NAME:       waylandframebuffer.h
*
*    DESCRIPTION:     Wayland framebuffer implementation using wl_shm
************************************************************************/

#ifndef __waylandframebuffer_h__
#define __waylandframebuffer_h__

#ifdef __linux__

// Game lib dependencies
#include <system/iframebuffer.h>

// Forward declaration(s)
struct wl_shm;
struct wl_surface;
struct wl_buffer;

class CWaylandFrameBuffer : public IFrameBuffer
{
public:

    // Constructor
    CWaylandFrameBuffer(struct wl_display* pDisplay, struct wl_shm* pShm, struct wl_surface* pSurface, int width, int height);

    // Destructor
    ~CWaylandFrameBuffer();

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

    // Wayland handles (not owned — the window class owns these)
    struct wl_display* m_pDisplay;
    struct wl_surface* m_pSurface;

    // Wayland buffer
    struct wl_buffer* m_pBuffer;

    // Pixel buffer (mmap'd shared memory)
    uint32_t* m_pPixels;

    // Shared memory fd and size
    int m_shmFd;
    int m_shmSize;

    // Dimensions
    int m_width;
    int m_height;
};

#endif  // __linux__

#endif  // __waylandframebuffer_h__
