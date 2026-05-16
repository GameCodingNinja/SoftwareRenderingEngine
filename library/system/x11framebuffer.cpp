
/************************************************************************
*    FILE NAME:       x11framebuffer.cpp
*
*    DESCRIPTION:     X11 framebuffer implementation using XImage
*                     with double buffering for tear-free rendering.
************************************************************************/

#ifdef __linux__

// Physical component dependency
#include <system/x11framebuffer.h>

// X11 lib dependencies
#include <X11/Xlib.h>
#include <X11/Xutil.h>

// Standard lib dependencies
#include <cstring>
#include <dlfcn.h>

// Game lib dependencies
#include <utilities/exceptionhandling.h>
#include <utilities/genfunc.h>

/************************************************************************
*    desc:  Constructor — creates two pixel buffers and XImages
************************************************************************/
CX11FrameBuffer::CX11FrameBuffer(Display* pDisplay, Window window, GC gc, int width, int height, bool vSync) :
    m_pDisplay(pDisplay),
    m_window(window),
    m_gc(gc),
    m_backIndex(0),
    m_width(width),
    m_height(height),
    m_vSync(vSync),
    m_pGLLib(nullptr),
    m_pGLXContext(nullptr),
    m_glXGetVideoSyncSGI(nullptr),
    m_glXWaitVideoSyncSGI(nullptr)
{
    m_pPixels[0] = nullptr;
    m_pPixels[1] = nullptr;
    m_pImage[0] = nullptr;
    m_pImage[1] = nullptr;

    int screen = DefaultScreen(m_pDisplay);
    int depth = DefaultDepth(m_pDisplay, screen);
    Visual* pVisual = DefaultVisual(m_pDisplay, screen);

    for( int i = 0; i < 2; ++i )
    {
        // Allocate the pixel buffer
        m_pPixels[i] = new uint32_t[m_width * m_height];
        std::memset(m_pPixels[i], 0, m_width * m_height * sizeof(uint32_t));

        // Create an XImage wrapping our pixel buffer
        m_pImage[i] = XCreateImage(
            m_pDisplay,
            pVisual,
            depth,
            ZPixmap,
            0,                              // offset
            reinterpret_cast<char*>(m_pPixels[i]),
            m_width,
            m_height,
            32,                             // bitmap_pad (bits)
            0 );                            // bytes_per_line (0 = auto)

        if( m_pImage[i] == nullptr )
            throw NExcept::CCriticalException("X11 Framebuffer Error!",
                "Failed to create XImage.");

        // Set byte order to match the host
        m_pImage[i]->byte_order = LSBFirst;
    }

    // Initialize GLX for vblank synchronization if VSync is enabled
    if( m_vSync )
        InitGLXVSync();

}   // Constructor


/************************************************************************
*    desc:  Destructor
************************************************************************/
CX11FrameBuffer::~CX11FrameBuffer()
{
    CleanupGLX();

    for( int i = 0; i < 2; ++i )
    {
        if( m_pImage[i] != nullptr )
        {
            // XDestroyImage frees the data pointer too, so null it
            // to prevent double-free since we allocated with new[]
            m_pImage[i]->data = nullptr;
            XDestroyImage(m_pImage[i]);
            m_pImage[i] = nullptr;
        }

        if( m_pPixels[i] != nullptr )
        {
            delete[] m_pPixels[i];
            m_pPixels[i] = nullptr;
        }
    }

}   // Destructor


/************************************************************************
*    desc:  Get the raw pixel buffer (returns the back buffer)
************************************************************************/
uint32_t* CX11FrameBuffer::GetPixels()
{
    return m_pPixels[m_backIndex];

}   // GetPixels


/************************************************************************
*    desc:  Get framebuffer width
************************************************************************/
int CX11FrameBuffer::GetWidth() const
{
    return m_width;

}   // GetWidth


/************************************************************************
*    desc:  Get framebuffer height
************************************************************************/
int CX11FrameBuffer::GetHeight() const
{
    return m_height;

}   // GetHeight


/************************************************************************
*    desc:  Clear the pixel buffer to zero (black)
************************************************************************/
void CX11FrameBuffer::Clear()
{
    std::memset(m_pPixels[m_backIndex], 0, m_width * m_height * sizeof(uint32_t));

}   // Clear


/************************************************************************
*    desc:  Blit the back buffer to the window and swap
************************************************************************/
void CX11FrameBuffer::Flip()
{
    // Wait for the next vblank if VSync is enabled
    if( m_vSync && m_glXWaitVideoSyncSGI != nullptr )
    {
        unsigned int count;
        m_glXGetVideoSyncSGI(&count);
        m_glXWaitVideoSyncSGI(2, (count + 1) % 2, &count);
    }

    // DEBUG: Dump first frame to PPM
    static bool dumped = false;
    if( !dumped )
    {
        dumped = true;
        FILE* fp = fopen("/tmp/framebuffer_dump.ppm", "wb");
        if( fp )
        {
            fprintf(fp, "P6\n%d %d\n255\n", m_width, m_height);
            for(int i = 0; i < m_width * m_height; ++i)
            {
                uint32_t px = m_pPixels[m_backIndex][i];
                // Format is 0xAARRGGBB
                unsigned char rgb[3] = {
                    (unsigned char)((px >> 16) & 0xFF),
                    (unsigned char)((px >>  8) & 0xFF),
                    (unsigned char)( px        & 0xFF)
                };
                fwrite(rgb, 1, 3, fp);
            }
            fclose(fp);
            printf("DEBUG: Framebuffer dumped to /tmp/framebuffer_dump.ppm\n");
        }
    }

    XPutImage(
        m_pDisplay,
        m_window,
        m_gc,
        m_pImage[m_backIndex],
        0, 0,           // src x, y
        0, 0,           // dest x, y
        m_width,
        m_height );

    XFlush(m_pDisplay);

    // Swap to the other buffer
    m_backIndex = 1 - m_backIndex;

}   // Flip


/************************************************************************
*    desc:  Initialize GLX for vblank synchronization.
*           Dynamically loads libGL.so.1 and creates a minimal GLX
*           context just for glXWaitVideoSyncSGI — no OpenGL rendering.
************************************************************************/
void CX11FrameBuffer::InitGLXVSync()
{
    // Dynamically load libGL to avoid a link-time dependency
    m_pGLLib = dlopen("libGL.so.1", RTLD_NOW | RTLD_GLOBAL);
    if( m_pGLLib == nullptr )
    {
        NGenFunc::PostDebugMsg("VSync: Could not load libGL.so.1 — VSync disabled.");
        m_vSync = false;
        return;
    }

    // Load the GLX functions we need
    using glXChooseVisualFunc   = XVisualInfo* (*)(Display*, int, int*);
    using glXCreateContextFunc  = void* (*)(Display*, XVisualInfo*, void*, int);
    using glXMakeCurrentFunc    = int (*)(Display*, unsigned long, void*);
    using glXDestroyContextFunc = void (*)(Display*, void*);
    using glXGetProcAddressFunc = void* (*)(const unsigned char*);

    auto pGlXChooseVisual   = reinterpret_cast<glXChooseVisualFunc>(dlsym(m_pGLLib, "glXChooseVisual"));
    auto pGlXCreateContext  = reinterpret_cast<glXCreateContextFunc>(dlsym(m_pGLLib, "glXCreateContext"));
    auto pGlXMakeCurrent    = reinterpret_cast<glXMakeCurrentFunc>(dlsym(m_pGLLib, "glXMakeCurrent"));
    auto pGlXDestroyContext = reinterpret_cast<glXDestroyContextFunc>(dlsym(m_pGLLib, "glXDestroyContext"));
    auto pGlXGetProcAddress = reinterpret_cast<glXGetProcAddressFunc>(dlsym(m_pGLLib, "glXGetProcAddressARB"));

    if( !pGlXChooseVisual || !pGlXCreateContext || !pGlXMakeCurrent || !pGlXGetProcAddress )
    {
        NGenFunc::PostDebugMsg("VSync: Missing GLX functions — VSync disabled.");
        CleanupGLX();
        m_vSync = false;
        return;
    }

    // Choose a minimal visual for the GLX context
    int screen = DefaultScreen(m_pDisplay);
    int attribs[] = { 4 /*GLX_RGBA*/, 0 /*None*/ };
    XVisualInfo* pVisInfo = pGlXChooseVisual(m_pDisplay, screen, attribs);
    if( pVisInfo == nullptr )
    {
        NGenFunc::PostDebugMsg("VSync: glXChooseVisual failed — VSync disabled.");
        CleanupGLX();
        m_vSync = false;
        return;
    }

    // Create a direct rendering context (required by GLX_SGI_video_sync)
    m_pGLXContext = pGlXCreateContext(m_pDisplay, pVisInfo, nullptr, 1 /*True/Direct*/);
    XFree(pVisInfo);

    if( m_pGLXContext == nullptr )
    {
        NGenFunc::PostDebugMsg("VSync: glXCreateContext failed — VSync disabled.");
        CleanupGLX();
        m_vSync = false;
        return;
    }

    // Make the context current on our window
    if( !pGlXMakeCurrent(m_pDisplay, m_window, m_pGLXContext) )
    {
        NGenFunc::PostDebugMsg("VSync: glXMakeCurrent failed — VSync disabled.");
        pGlXDestroyContext(m_pDisplay, m_pGLXContext);
        m_pGLXContext = nullptr;
        CleanupGLX();
        m_vSync = false;
        return;
    }

    // Load the GLX_SGI_video_sync extension functions
    m_glXGetVideoSyncSGI = reinterpret_cast<int(*)(unsigned int*)>(
        pGlXGetProcAddress(reinterpret_cast<const unsigned char*>("glXGetVideoSyncSGI")));
    m_glXWaitVideoSyncSGI = reinterpret_cast<int(*)(int, int, unsigned int*)>(
        pGlXGetProcAddress(reinterpret_cast<const unsigned char*>("glXWaitVideoSyncSGI")));

    if( m_glXGetVideoSyncSGI == nullptr || m_glXWaitVideoSyncSGI == nullptr )
    {
        NGenFunc::PostDebugMsg("VSync: GLX_SGI_video_sync not available — VSync disabled.");
        pGlXMakeCurrent(m_pDisplay, 0, nullptr);
        pGlXDestroyContext(m_pDisplay, m_pGLXContext);
        m_pGLXContext = nullptr;
        CleanupGLX();
        m_vSync = false;
        return;
    }

    NGenFunc::PostDebugMsg("VSync: GLX_SGI_video_sync initialized successfully.");

}   // InitGLXVSync


/************************************************************************
*    desc:  Clean up GLX resources
************************************************************************/
void CX11FrameBuffer::CleanupGLX()
{
    if( m_pGLXContext != nullptr && m_pGLLib != nullptr )
    {
        auto pGlXMakeCurrent = reinterpret_cast<int(*)(Display*, unsigned long, void*)>(
            dlsym(m_pGLLib, "glXMakeCurrent"));
        auto pGlXDestroyContext = reinterpret_cast<void(*)(Display*, void*)>(
            dlsym(m_pGLLib, "glXDestroyContext"));

        if( pGlXMakeCurrent )
            pGlXMakeCurrent(m_pDisplay, 0, nullptr);

        if( pGlXDestroyContext )
            pGlXDestroyContext(m_pDisplay, m_pGLXContext);

        m_pGLXContext = nullptr;
    }

    m_glXGetVideoSyncSGI = nullptr;
    m_glXWaitVideoSyncSGI = nullptr;

    if( m_pGLLib != nullptr )
    {
        dlclose(m_pGLLib);
        m_pGLLib = nullptr;
    }

}   // CleanupGLX

#endif  // __linux__
