
/************************************************************************
*    FILE NAME:       renderdefs.h
*
*    DESCRIPTION:     List of render definations
************************************************************************/

#ifndef __render_defs_h__
#define __render_defs_h__

// Standard lib dependencies
#include <cstdint>

// Game lib dependencies
#include <common/vertex2d.h>
#include <common/defs.h>

// Forward declaration(s)
class CSRTexture;

/************************************************************************
*    Surface data struct for the render pipeline
************************************************************************/
struct CSurfaceData
{
    uint32_t* pixels = nullptr;
    int w = 0;
    int h = 0;
};


/************************************************************************
*    Render data passed to worker threads for triangle rasterization
************************************************************************/
class CRender2d
{
public:

    CRender2d( CSRTexture * pText, CSurfaceData * pSurface, uint32_t colorR, uint32_t colorG, uint32_t colorB, uint32_t colorA, bool applyColor, bool blendAlpha ) :
        m_pText(pText),
        m_pSurface(pSurface),
        m_cr(colorR), m_cg(colorG), m_cb(colorB), m_ca(colorA),
        m_applyColor(applyColor),
        m_blendAlpha(blendAlpha)
    { }

    // Texture pointer
    CSRTexture * m_pText;

    // Surface data pointer
    CSurfaceData * m_pSurface;

    // Color modulation (0-255 fixed-point, 255 = 1.0 = no change)
    uint32_t m_cr, m_cg, m_cb, m_ca;

    // Flag to apply color modulation
    bool m_applyColor;

    // Flag to enable per-pixel alpha blending
    bool m_blendAlpha;

    // Three 2D vertexes
    CVertex2D m_vec[TRI];

    // Cull if the projected point are outside the screen
    bool Cull( int screenW, int screenH )
    {
        int XMinInVis(0), XMaxInVis(0), YMinInVis(0), YMaxInVis(0);

        // Determine location of panel's 2D points
        for( int i = 0; i < TRI; ++i )
        {
            if( m_vec[ i ].vert.x < 0 ) // 0 was MINX
                ++XMinInVis;

            else if( m_vec[ i ].vert.x > screenW )
                ++XMaxInVis;

            if( m_vec[ i ].vert.y < 0 )  // 0 was MINY
                ++YMinInVis;

            else if( m_vec[ i ].vert.y > screenH )
                ++YMaxInVis;
        }

        if( TRI > XMinInVis && TRI > YMinInVis && 
            TRI > XMaxInVis && TRI > YMaxInVis )
            return false;

        return true;

    }   // Cull

};

#endif  // __render_defs_h__
