
/************************************************************************
*    FILE NAME:       softwareRender.h
*
*    DESCRIPTION:     Software Rendering device
************************************************************************/

#ifndef __software_render_h__
#define __software_render_h__

// Standard lib dependencies
#include <string>
#include <map>
#include <vector>
#include <future>

// Game lib dependencies
#include <common/defs.h>
#include <common/size.h>
#include <common/color.h>
#include <common/vertex.h>
#include <softwareRender/renderdefs.h>
#include <softwareRender/render2d.h>
#include <softwareRender/render3d.h>

// Forward declaration(s)
class CTexture;
class CMatrix;
class IFrameBuffer;

class CSoftwareRender
{
public:

    // Get the instance of the singleton class
    static CSoftwareRender & Instance()
    {
        static CSoftwareRender softwareRender;
        return softwareRender;
    }

    // Set the surface data from a framebuffer
    void setSurface( IFrameBuffer * pFrameBuffer );

    // Create the VBO
    uint createVBO( float * pData, uint count );

    // Create the IBO
    uint createIBO( uint * pData, uint sizeInBytes );

    // Delete the VBO
    void deleteVBO( uint Id );

    // Delete the IBO
    void deleteIBO( uint Id );

    // Render 2D (orthographic)
    void render2D( const CMatrix & matrix, const uint vertCount, const uint indexCount, const CTexture * pTexture, uint vboId, uint iboId, const CColor<float> & color = CColor<float>(), FragmentShaderFunc shader = nullptr );

    // Render 3D (perspective with z-buffer)
    void render3D( const CMatrix & matrix, const uint vertCount, const uint indexCount, const CTexture * pTexture, uint vboId, uint iboId, const CColor<float> & color = CColor<float>(), FragmentShaderFunc shader = nullptr );

    // Clear the z-buffer
    void clearZBuffer();

private:

    // Constructor
    CSoftwareRender();

    // Destructor
    ~CSoftwareRender();

    // Get the VBO
    float * getVBO( uint Id );

    // Get the IBO
    uint * getIBO( uint Id );

private:

    // Surface data for the render target
    CSurfaceData m_surfaceData;

    // ID inc
    uint m_vboIdInc;
    uint m_iboIdInc;

    // map of allocated vbo
    std::map<uint, float *> m_pVBOMap;

    // map of allocated ibo
    std::map<uint, uint *> m_pIBOMap;

    // Half size of view port
    CSize<float> m_halfScreen;

    // Z-buffer for 3D depth testing
    std::vector<int32_t> m_zBuffer;

};

#endif
