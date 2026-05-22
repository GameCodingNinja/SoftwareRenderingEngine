
/************************************************************************
*    FILE NAME:       softwareRender.h
*
*    DESCRIPTION:     Software Rendering device
************************************************************************/

#ifndef __software_render_h__
#define __software_render_h__

// Standard lib dependencies
#include <string>
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

    // Render 2D (orthographic)
    void render2D( const CMatrix & matrix, const uint vertCount, const uint indexCount, const CTexture * pTexture, float * pVBO, uint * pIBO, const CColor<float> & color = CColor<float>(), FragmentShaderFunc shader = nullptr );

    // Render 3D (perspective with z-buffer)
    void render3D( const CMatrix & matrix, const uint vertCount, const uint indexCount, const CTexture * pTexture, float * pVBO, uint * pIBO, const CColor<float> & color = CColor<float>(), FragmentShaderFunc shader = nullptr );

    // Render 2D fixed-function (orthographic, no shader)
    void renderFixedFunction2D( const CMatrix & matrix, const uint vertCount, const uint indexCount, const CTexture * pTexture, float * pVBO, uint * pIBO, const CColor<float> & color = CColor<float>(), bool blendAlpha = false );

    // Render 3D fixed-function (perspective with z-buffer, no shader)
    void renderFixedFunction3D( const CMatrix & matrix, const uint vertCount, const uint indexCount, const CTexture * pTexture, float * pVBO, uint * pIBO, const CColor<float> & color = CColor<float>() );

    // Clear the z-buffer
    void clearZBuffer();

private:

    // Constructor
    CSoftwareRender();

    // Destructor
    ~CSoftwareRender();

private:

    // Surface data for the render target
    CSurfaceData m_surfaceData;

    // Half size of view port
    CSize<float> m_halfScreen;

    // Z-buffer for 3D depth testing
    std::vector<int32_t> m_zBuffer;

};

#endif
