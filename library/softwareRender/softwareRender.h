
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
class CSRTexture;
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
    void SetSurface( IFrameBuffer * pFrameBuffer );

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

    // Render 2D (orthographic)
    void Render2D( const CMatrix & matrix, const uint vertCount, const uint indexCount, uint textId, uint vboId, uint iboId, const CColor<float> & color = CColor<float>(), FragmentShaderFunc shader = nullptr );

    // Render 3D (perspective with z-buffer)
    void Render3D( const CMatrix & matrix, const uint vertCount, const uint indexCount, uint textId, uint vboId, uint iboId, const CColor<float> & color = CColor<float>(), FragmentShaderFunc shader = nullptr );

    // Clear the z-buffer
    void ClearZBuffer();

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

private:

    // Surface data for the render target
    CSurfaceData m_surfaceData;

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

    // Z-buffer for 3D depth testing
    std::vector<int32_t> m_zBuffer;

};

#endif  // __software_render_h__
