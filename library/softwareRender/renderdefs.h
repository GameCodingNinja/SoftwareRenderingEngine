
/************************************************************************
*    FILE NAME:       renderdefs.h
*
*    DESCRIPTION:     List of render definations
************************************************************************/

#ifndef __render_defs_h__
#define __render_defs_h__

// Physical component dependency

// Standard lib dependencies

// Boost lib dependencies

// Game lib dependencies
#include <common/vertex2d.h>
#include <common/defs.h>

// Forward declaration(s)
class CSRTexture;

class CRender2d
{
public:

    CRender2d( CSRTexture * pText, SDL_Surface * pSurface ) :
        m_pText(pText),
        m_pSurface(pSurface)
    { }

    // Texture pointer
    CSRTexture * m_pText;

    // Surface pointer
    SDL_Surface * m_pSurface;

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


