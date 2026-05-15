
/************************************************************************
*    FILE NAME:       waylandframebuffer.cpp
*
*    DESCRIPTION:     Wayland framebuffer implementation using wl_shm
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
*           Uses memfd_create (Linux 3.17+) for anonymous shared memory
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
*    desc:  Buffer release listener — called when compositor is done
*           with the buffer. We use a single buffer so this is a no-op.
************************************************************************/
static void BufferRelease(void* /*data*/, struct wl_buffer* /*buffer*/)
{
}

static const struct wl_buffer_listener bufferListener = {
    BufferRelease
};


/************************************************************************
*    desc:  Constructor
************************************************************************/
CWaylandFrameBuffer::CWaylandFrameBuffer(
    struct wl_display* pDisplay, struct wl_shm* pShm, struct wl_surface* pSurface, int width, int height) :
    m_pDisplay(pDisplay),
    m_pSurface(pSurface),
    m_pBuffer(nullptr),
    m_pPixels(nullptr),
    m_shmFd(-1),
    m_shmSize(0),
    m_width(width),
    m_height(height)
{
    int stride = m_width * 4;
    m_shmSize = stride * m_height;

    // Create shared memory file
    m_shmFd = CreateShmFd(m_shmSize);
    if( m_shmFd < 0 )
        throw NExcept::CCriticalException("Wayland Framebuffer Error!",
            "Failed to create shared memory file.");

    // Map the shared memory
    void* data = mmap(nullptr, m_shmSize, PROT_READ | PROT_WRITE, MAP_SHARED, m_shmFd, 0);
    if( data == MAP_FAILED )
    {
        close(m_shmFd);
        throw NExcept::CCriticalException("Wayland Framebuffer Error!",
            "Failed to mmap shared memory.");
    }
    m_pPixels = static_cast<uint32_t*>(data);

    // Clear the buffer
    std::memset(m_pPixels, 0, m_shmSize);

    // Create wl_shm_pool and wl_buffer
    struct wl_shm_pool* pool = wl_shm_create_pool(pShm, m_shmFd, m_shmSize);
    m_pBuffer = wl_shm_pool_create_buffer(pool, 0, m_width, m_height, stride, WL_SHM_FORMAT_XRGB8888);
    wl_buffer_add_listener(m_pBuffer, &bufferListener, this);

    // Pool can be destroyed immediately — the buffer retains the reference
    wl_shm_pool_destroy(pool);

    // Close the fd — the mmap and wayland pool keep the memory alive
    close(m_shmFd);
    m_shmFd = -1;

}   // Constructor


/************************************************************************
*    desc:  Destructor
************************************************************************/
CWaylandFrameBuffer::~CWaylandFrameBuffer()
{
    if( m_pBuffer != nullptr )
    {
        wl_buffer_destroy(m_pBuffer);
        m_pBuffer = nullptr;
    }

    if( m_pPixels != nullptr )
    {
        munmap(m_pPixels, m_shmSize);
        m_pPixels = nullptr;
    }

    if( m_shmFd >= 0 )
    {
        close(m_shmFd);
        m_shmFd = -1;
    }

}   // Destructor


/************************************************************************
*    desc:  Get the raw pixel buffer
************************************************************************/
uint32_t* CWaylandFrameBuffer::GetPixels()
{
    return m_pPixels;

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
    std::memset(m_pPixels, 0, m_shmSize);

}   // Clear


/************************************************************************
*    desc:  Attach and commit the buffer to the Wayland surface
************************************************************************/
void CWaylandFrameBuffer::Flip()
{
    wl_surface_attach(m_pSurface, m_pBuffer, 0, 0);
    wl_surface_damage_buffer(m_pSurface, 0, 0, m_width, m_height);
    wl_surface_commit(m_pSurface);
    wl_display_flush(m_pDisplay);

}   // Flip

#endif  // __linux__
