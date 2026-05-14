
/************************************************************************
*    FILE NAME:       softwareRender.h
*
*    DESCRIPTION:     Software Rendering device
************************************************************************/

#ifndef __software_render_h__
#define __software_render_h__

// Physical component dependency

// Standard lib dependencies
#include <string>
#include <map>
#include <vector>

// Boost lib dependencies
#include <boost/asio.hpp>
#include <boost/thread.hpp>

// Game lib dependencies
#include <common/defs.h>
#include <common/size.h>
#include <common/vertex2d.h>

// Forward declaration(s)
struct SDL_Surface;
struct SDL_Window;
class CSRTexture;
class CMatrix;
class CRender2d;

class CSoftwareRender
{
public:

    // Get the instance of the singleton class
    static CSoftwareRender & Instance()
    {
        static CSoftwareRender softwareRender;
        return softwareRender;
    }

    // Create the SDL window surface
    void CreateSurface( SDL_Window * pWindow );

    // Get the SDL window surface
    SDL_Surface * GetSurface();

    // Display the rendered changes
    void Flip( SDL_Window * pWindow );

    // Clear the buffers
    void Clear();

    // Create a texture
    uint CreateTexture( uchar * pData, int w, int h );

    // Create the VBO
    uint CreateVBO( float * pData, uint count );

    // Create the IBO
    uint CreateIBO( uint * pData, uint sizeInBytes );

    // Delete the texture
    void DeleteTexture( uint Id );

    // Delete the VBO
    void DeleteVBO( uint Id );

    // Delete the IBO
    void DeleteIBO( uint Id );

    // Render
    void Render( const CMatrix & matrix, const uint vertCount, const uint indexCount, uint textId, uint vboId, uint iboId );

private:

    // Constructor
    CSoftwareRender();

	// Destructor
	~CSoftwareRender();

    // Get the texture
    CSRTexture * GetTexture( uint Id );

    // Get the VBO
    float * GetVBO( uint Id );

    // Get the IBO
    uint * GetIBO( uint Id );

    // Render the triangle
    //void RenderTri( CRender2d * pRender );

    // Push the job onto the worker threads
    void PushJob( CRender2d * pRender2d );

private:

    // The windows surface for 2d software rendering (Do Not FREE))
    // Will be freed if window is resized or program terminated
    SDL_Surface * m_pSurface;

    // ID inc
    uint m_textIdInc;
    uint m_vboIdInc;
    uint m_iboIdInc;

    // map of allocated textures
    std::map<uint, CSRTexture *> m_pTextureMap;

    // map of allocated vbo
    std::map<uint, float *> m_pVBOMap;

    // map of allocated ibo
    std::map<uint, uint *> m_pIBOMap;

    // Half size of view port
    CSize<float> m_halfScreen;

    // Boost ASIO service
    boost::asio::io_service m_asioService;

    // Boost ASIO service work
    std::unique_ptr<boost::asio::io_service::work> m_upAsioWork;

    // Boost thread group
    std::unique_ptr<boost::thread_group> m_upThreadGroup;

    // Vector of current jobs
    std::vector<boost::shared_future<int> > m_pendingJobs;

};

#endif  // __classtemplate_h__


