
/************************************************************************
*    FILE NAME:       waylandframebuffer.h
*
*    DESCRIPTION:     Wayland framebuffer implementation using wl_shm
*                     with double buffering for tear-free rendering.
************************************************************************/

#ifndef __waylandframebuffer_h__
#define __waylandframebuffer_h__

#ifdef __linux__

// Standard lib dependencies
#include <atomic>

// Game lib dependencies
#include <system/iframebuffer.h>

// Forward declaration(s)
struct wl_shm;
struct wl_surface;
struct wl_buffer;
struct wl_display;
struct wl_callback;

class CWaylandFrameBuffer : public IFrameBuffer
{
public:

    // Constructor
    CWaylandFrameBuffer(struct wl_display* pDisplay, struct wl_shm* pShm, struct wl_surface* pSurface, int width, int height, bool vSync);

    // Destructor
    ~CWaylandFrameBuffer();

    // Get the raw pixel buffer (returns the back buffer)
    uint32_t* GetPixels() override;

    // Get framebuffer dimensions
    int GetWidth() const override;
    int GetHeight() const override;

    // Clear the pixel buffer to zero (black)
    void Clear() override;

    // Swap buffers and display the back buffer contents
    void Flip() override;

    // Called when the compositor releases a buffer
    void OnBufferRelease(struct wl_buffer* buffer);

    // Called when the compositor is ready for the next frame
    void OnFrameDone();

private:

    // Create a single shm buffer and return the pixel pointer
    struct wl_buffer* CreateBuffer(struct wl_shm* pShm, uint32_t*& pPixelsOut);

    // Wayland handles (not owned — the window class owns these)
    struct wl_display* m_pDisplay;
    struct wl_surface* m_pSurface;

    // Double buffer: two wl_buffers and their pixel data
    struct wl_buffer* m_pBuffer[2];
    uint32_t* m_pPixels[2];

    // Which buffer the engine is drawing to (0 or 1)
    int m_backIndex;

    // Shared memory size per buffer
    int m_shmSize;

    // Dimensions
    int m_width;
    int m_height;

    // Frame callback for vsync throttling
    struct wl_callback* m_pFrameCallback;
    std::atomic<bool> m_frameReady;

    // Per-buffer release tracking — true means safe to draw into
    std::atomic<bool> m_bufferReleased[2];

    // VSync enabled flag
    bool m_vSync;
};

#endif  // __linux__

#endif  // __waylandframebuffer_h__
