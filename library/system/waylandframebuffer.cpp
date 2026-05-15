
/************************************************************************
*    FILE NAME:       waylandframebuffer.cpp
*
*    DESCRIPTION:     Wayland framebuffer implementation using wl_shm
*                     with double buffering for tear-free rendering.
************************************************************************/

#ifdef __linux__

// Physical component dependency
#include <system/waylandframebuffer.h>

// Wayland lib dependencies
#include <wayland-client.h>

// Standard lib dependencies
#include <cstring>
#include <cerrno>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>

// Game lib dependencies
#include <utilities/exceptionhandling.h>

/************************************************************************
*    desc:  Create a temporary file descriptor for shared memory
************************************************************************/
static int CreateShmFd(int size)
{
    int fd = memfd_create("framebuffer", MFD_CLOEXEC);
    if( fd < 0 )
        return -1;

    if( ftruncate(fd, size) < 0 )
    {
        close(fd);
        return -1;
    }

    return fd;
}


/************************************************************************
*    desc:  Buffer release listener — compositor is done with this buffer
************************************************************************/
static void BufferRelease(void* data, struct wl_buffer* buffer)
{
    static_cast<CWaylandFrameBuffer*>(data)->OnBufferRelease(buffer);
}

static const struct wl_buffer_listener bufferListener = {
    BufferRelease
};


/************************************************************************
*    desc:  Frame callback listener — compositor is ready for next frame
************************************************************************/
static void FrameDone(void* data, struct wl_callback* callback, uint32_t /*time*/)
{
    wl_callback_destroy(callback);
    static_cast<CWaylandFrameBuffer*>(data)->OnFrameDone();
}

static const struct wl_callback_listener frameListener = {
    FrameDone
};


/************************************************************************
*    desc:  Create a single shm buffer
************************************************************************/
struct wl_buffer* CWaylandFrameBuffer::CreateBuffer(struct wl_shm* pShm, uint32_t*& pPixelsOut)
{
    int stride = m_width * 4;

    int fd = CreateShmFd(m_shmSize);
    if( fd < 0 )
        throw NExcept::CCriticalException("Wayland Framebuffer Error!",
            "Failed to create shared memory file.");

    void* data = mmap(nullptr, m_shmSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if( data == MAP_FAILED )
    {
        close(fd);
        throw NExcept::CCriticalException("Wayland Framebuffer Error!",
            "Failed to mmap shared memory.");
    }
    pPixelsOut = static_cast<uint32_t*>(data);
    std::memset(pPixelsOut, 0, m_shmSize);

    struct wl_shm_pool* pool = wl_shm_create_pool(pShm, fd, m_shmSize);
    struct wl_buffer* buffer = wl_shm_pool_create_buffer(
        pool, 0, m_width, m_height, stride, WL_SHM_FORMAT_XRGB8888);
    wl_buffer_add_listener(buffer, &bufferListener, this);

    wl_shm_pool_destroy(pool);
    close(fd);

    return buffer;
}


/************************************************************************
*    desc:  Constructor — creates two shm buffers for double buffering
************************************************************************/
CWaylandFrameBuffer::CWaylandFrameBuffer(
    struct wl_display* pDisplay, struct wl_shm* pShm, struct wl_surface* pSurface, int width, int height, bool vSync) :
    m_pDisplay(pDisplay),
    m_pSurface(pSurface),
    m_backIndex(0),
    m_shmSize(width * height * 4),
    m_width(width),
    m_height(height),
    m_pFrameCallback(nullptr),
    m_frameReady(true),
    m_vSync(vSync)
{
    m_pBuffer[0] = nullptr;
    m_pBuffer[1] = nullptr;
    m_pPixels[0] = nullptr;
    m_pPixels[1] = nullptr;

    // Both buffers start as available (not yet submitted)
    m_bufferReleased[0].store(true);
    m_bufferReleased[1].store(true);

    // Create both buffers
    m_pBuffer[0] = CreateBuffer(pShm, m_pPixels[0]);
    m_pBuffer[1] = CreateBuffer(pShm, m_pPixels[1]);

}   // Constructor


/************************************************************************
*    desc:  Destructor
************************************************************************/
CWaylandFrameBuffer::~CWaylandFrameBuffer()
{
    if( m_pFrameCallback != nullptr )
    {
        wl_callback_destroy(m_pFrameCallback);
        m_pFrameCallback = nullptr;
    }

    for( int i = 0; i < 2; ++i )
    {
        if( m_pBuffer[i] != nullptr )
        {
            wl_buffer_destroy(m_pBuffer[i]);
            m_pBuffer[i] = nullptr;
        }

        if( m_pPixels[i] != nullptr )
        {
            munmap(m_pPixels[i], m_shmSize);
            m_pPixels[i] = nullptr;
        }
    }

}   // Destructor


/************************************************************************
*    desc:  Get the raw pixel buffer (returns the back buffer)
************************************************************************/
uint32_t* CWaylandFrameBuffer::GetPixels()
{
    return m_pPixels[m_backIndex];

}   // GetPixels


/************************************************************************
*    desc:  Get framebuffer width
************************************************************************/
int CWaylandFrameBuffer::GetWidth() const
{
    return m_width;

}   // GetWidth


/************************************************************************
*    desc:  Get framebuffer height
************************************************************************/
int CWaylandFrameBuffer::GetHeight() const
{
    return m_height;

}   // GetHeight


/************************************************************************
*    desc:  Clear the pixel buffer to zero (black)
************************************************************************/
void CWaylandFrameBuffer::Clear()
{
    std::memset(m_pPixels[m_backIndex], 0, m_shmSize);

}   // Clear


/************************************************************************
*    desc:  Swap buffers and commit the back buffer to the compositor.
*           Uses frame callbacks to throttle to the display refresh rate.
************************************************************************/
void CWaylandFrameBuffer::Flip()
{
    // If VSync is enabled, wait for the compositor to signal it's ready
    if( m_vSync )
    {
        while( !m_frameReady.load() )
        {
            wl_display_dispatch(m_pDisplay);
        }
    }

    // Mark the buffer we're about to submit as owned by the compositor
    m_bufferReleased[m_backIndex].store(false);

    // Submit the current back buffer to the compositor
    wl_surface_attach(m_pSurface, m_pBuffer[m_backIndex], 0, 0);
    wl_surface_damage_buffer(m_pSurface, 0, 0, m_width, m_height);

    // Request a frame callback for vsync throttling
    if( m_vSync )
    {
        m_frameReady.store(false);
        m_pFrameCallback = wl_surface_frame(m_pSurface);
        wl_callback_add_listener(m_pFrameCallback, &frameListener, this);
    }

    wl_surface_commit(m_pSurface);
    wl_display_flush(m_pDisplay);

    // Swap to the other buffer index
    m_backIndex = 1 - m_backIndex;

    // Wait until the new back buffer has been released by the compositor
    // so we don't draw into a buffer it's still reading
    while( !m_bufferReleased[m_backIndex].load() )
    {
        wl_display_dispatch(m_pDisplay);
    }

}   // Flip


/************************************************************************
*    desc:  Called when the compositor releases a buffer
************************************************************************/
void CWaylandFrameBuffer::OnBufferRelease(struct wl_buffer* buffer)
{
    // Mark the released buffer as available for drawing
    for( int i = 0; i < 2; ++i )
    {
        if( m_pBuffer[i] == buffer )
        {
            m_bufferReleased[i].store(true);
            break;
        }
    }

}   // OnBufferRelease


/************************************************************************
*    desc:  Called when the compositor is ready for the next frame
************************************************************************/
void CWaylandFrameBuffer::OnFrameDone()
{
    m_pFrameCallback = nullptr;
    m_frameReady.store(true);

}   // OnFrameDone

#endif  // __linux__
