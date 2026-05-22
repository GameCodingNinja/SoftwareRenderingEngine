
/************************************************************************
*    FILE NAME:       render2d.h
*
*    DESCRIPTION:     Render data passed to worker threads for
*                     2D triangle rasterization
************************************************************************/

#pragma once

// Game lib dependencies
#include <softwareRender/renderdefs.h>

// Forward declaration(s)
class CSRTexture;

class CRender2d
{
public:

    CRender2d( CSRTexture * pText, CSurfaceData * pSurface, uint32_t colorR, uint32_t colorG, uint32_t colorB, uint32_t colorA, FragmentShaderFunc shader = nullptr ) :
        m_pText(pText),
        m_pSurface(pSurface),
        m_cr(colorR), m_cg(colorG), m_cb(colorB), m_ca(colorA),
        m_shader(shader)
    {
        // Set up the uniform block
        m_uniforms.cr = colorR;
        m_uniforms.cg = colorG;
        m_uniforms.cb = colorB;
        m_uniforms.ca = colorA;

        // If no custom shader was provided, use the default fallback
        if( m_shader == nullptr )
            m_shader = NShaderDefault::shaderDefault;
    }

    // Texture pointer
    CSRTexture * m_pText;

    // Surface data pointer
    CSurfaceData * m_pSurface;

    // Color modulation (0-255 fixed-point, 255 = 1.0 = no change)
    uint32_t m_cr, m_cg, m_cb, m_ca;

    // The fragment shader function called per pixel
    FragmentShaderFunc m_shader;

    // Uniform data passed to the shader
    SShaderUniforms m_uniforms;

    // Three 2D vertexes
    CVertex m_vec[TRI];

    // Cull if the projected point are outside the screen
    bool Cull( int screenW, int screenH )
    {
        int XMinInVis(0), XMaxInVis(0), YMinInVis(0), YMaxInVis(0);

        // Determine location of panel's 2D points
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
