
/************************************************************************
*    FILE NAME:       render3d.h
*
*    DESCRIPTION:     Render data passed to worker threads for
*                     3D triangle rasterization
************************************************************************/

#pragma once

// Game lib dependencies
#include <softwareRender/renderdefs.h>

// Forward declaration(s)
class CTexture;

class CRender3d
{
public:

    // Fixed-function constructor
    CRender3d( const CTexture * pText, CSurfaceData * pSurface, int32_t * pZBuffer, const CColor<uint32_t> & color, bool applyColor ) :
        m_pText(pText),
        m_pSurface(pSurface),
        m_pZBuffer(pZBuffer),
        m_color(color),
        m_shader(nullptr),
        m_applyColor(applyColor)
    {
    }

    CRender3d( const CTexture * pText, CSurfaceData * pSurface, int32_t * pZBuffer, const CColor<uint32_t> & color, FragmentShaderFunc shader = nullptr ) :
        m_pText(pText),
        m_pSurface(pSurface),
        m_pZBuffer(pZBuffer),
        m_color(color),
        m_shader(shader),
        m_applyColor(false)
    {
        if( m_shader == nullptr )
            m_shader = NShaderDefault::shaderDefault;
    }

    // Texture pointer
    const CTexture * m_pText;

    // Surface data pointer
    CSurfaceData * m_pSurface;

    // Z-buffer pointer
    int32_t * m_pZBuffer;

    // Color modulation (0-255 fixed-point, 255 = 1.0 = no change)
    CColor<uint32_t> m_color;

    // The fragment shader function called per pixel
    FragmentShaderFunc m_shader;

    // Three vertexes (screen-projected with 1/Z, U/Z, V/Z)
    CVertex3d m_vec[TRI];

    // Cached triangle Y bounds for strip binning
    float m_triYMin;
    float m_triYMax;

    // Fixed-function flag
    bool m_applyColor;

    // Cull if the projected points are outside the screen
    bool Cull( int screenW, int screenH )
    {
        int XMinInVis(0), XMaxInVis(0), YMinInVis(0), YMaxInVis(0);

        // Determine location of panel's 2D projected points
        for( int i = 0; i < TRI; ++i )
        {
            if( m_vec[ i ].vert.x < 0 )
                ++XMinInVis;

            else if( m_vec[ i ].vert.x > screenW )
                ++XMaxInVis;

            if( m_vec[ i ].vert.y < 0 )
                ++YMinInVis;

            else if( m_vec[ i ].vert.y > screenH )
                ++YMaxInVis;
        }

        if( TRI > XMinInVis && TRI > YMinInVis && 
            TRI > XMaxInVis && TRI > YMaxInVis )
            return false;

        return true;
    }
};
