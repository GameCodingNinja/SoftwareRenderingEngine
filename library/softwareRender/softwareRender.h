
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
#include <common/point.h>
#include <common/vertex.h>
#include <common/light.h>
#include <common/lightdefs.h>
#include <softwareRender/renderdefs.h>
#include <softwareRender/render2d.h>
#include <softwareRender/render3d.h>

// Forward declaration(s)
class CTexture;
class CMatrix;
class IFrameBuffer;
class CVisualComponent2d;
class CVisualComponent3d;

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

    // Set the active lights for rendering
    void setLights( const std::vector<CLight> & lights );

    // Clear the active lights (reverts to default)
    void clearLights();

    // Render 2D (orthographic)
    void render2D( const CMatrix & matrix, const CVisualComponent2d & visualComponent );

    // Render 3D (perspective with z-buffer)
    void render3D( const CMatrix & matrix, const CMatrix & modelViewMatrix, const CVisualComponent3d & visualComponent );

    // Render 2D fixed-function (orthographic, no shader)
    void renderFixedFunction2D( const CMatrix & matrix, const CVisualComponent2d & visualComponent );

    // Render 3D fixed-function (perspective with z-buffer, no shader)
    void renderFixedFunction3D( const CMatrix & matrix, const CVisualComponent3d & visualComponent );

    // Clear the z-buffer
    void clearZBuffer();

private:

    // Constructor
    CSoftwareRender();

    // Destructor
    ~CSoftwareRender();

    // Struct for unique transformed positions
    struct SUniqueVert
    {
        CPoint<float> pos;  // Clip-space position (x, y, z from matrix transform)
        float w;            // Clip-space W (represents -eye.z for PerspectiveFovRH)
    };

    // Struct for clipping interpolation (stack-local, not per-mesh)
    struct SClipVert
    {
        CPoint<float> pos;
        float w;
        float u, v;
        CPoint<float> norm;
    };

public:

    // Compute lighting color from a normal and position against a light list
    static CColor<float> computeVertexLighting(
        const CPoint<float> & transNorm,
        const CPoint<float> & viewPos,
        const std::vector<CLight> & lights );

private:

    // Surface data for the render target
    CSurfaceData m_surfaceData;

    // Half size of view port
    CSize<float> m_halfScreen;

    // Z-buffer for 3D depth testing
    std::vector<int32_t> m_zBuffer;

    // Reusable scratch buffers for transformed vertices (avoids per-draw heap allocation)
    std::vector<CVertex> m_transVerts2D;

    // Reusable scratch buffer for unique transformed positions
    std::vector<SUniqueVert> m_transUniqueVerts;

    // Reusable scratch buffer for transformed normals (per-vertex)
    std::vector<CPoint<float>> m_transNormals;

    // Active light list pointer (nullptr = use defaults)
    const std::vector<CLight> * m_pLights;

    // Default lights (ambient + directional)
    std::vector<CLight> m_defaultLights;

};

#endif
