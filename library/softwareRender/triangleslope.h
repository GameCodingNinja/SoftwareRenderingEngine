
/************************************************************************
*    FILE NAME:       triangleslope.h
*
*    DESCRIPTION:     Triangle slope class
************************************************************************/

#ifndef __triangle_slope_h__
#define __triangle_slope_h__

// Standard lib dependencies
#include <vector>

// Game lib dependencies
#include <common/vertex2d.h>

class CTriangleSlope
{
public:

    enum ESlopeType
    {
        EST_LEFT,
        EST_RIGHT
    };

    // Constructor
    CTriangleSlope( const CVertex2D trans[], int vTop, ESlopeType slopeType );

	// Destructor
	~CTriangleSlope();

    // Init the slope
    bool Init();

    // Inc the slope
    void Inc();

    // slopt position
    CVertex2D m_slope;

    // Length of slope
    int m_length;

    // Y offset
    int y;

private:

    // Clip to the top of the buffer
    void ClipTop();

private:

    // Vector of projected verts
    const CVertex2D * m_pVert;

    // Top vert index
    int m_vTop;

    // Next vert index
    int m_vNext;

    // step interpolation
    CVertex2D m_step;

    // The type of slope
    const ESlopeType m_slopeType;

};

#endif  // __triangle_slope_h__


