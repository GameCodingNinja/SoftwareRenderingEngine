
/************************************************************************
*    FILE NAME:       triangleslope.cpp
*
*    DESCRIPTION:     Triangle slope class
************************************************************************/

// Physical component dependency
#include <softwareRender/triangleslope.h>

// Standard lib dependencies

// Boost lib dependencies

// Game lib dependencies

// Game dependencies


/************************************************************************
*    desc:  Constructor
************************************************************************/
CTriangleSlope::CTriangleSlope( const CVertex2D trans[], int vTop, ESlopeType slopeType ) :
    m_pVert( trans ),
    m_vTop( vTop ),
    m_vNext( vTop ),
    m_length(0),
    m_slopeType( slopeType ),
    y(0)
{
}   // constructor


/************************************************************************
*    desc:  destructor
************************************************************************/
CTriangleSlope::~CTriangleSlope()
{
}	// destructor


/************************************************************************
*    desc:  Init the slope
************************************************************************/
bool CTriangleSlope::Init()
{
    if( m_length == 0 )
    {
        // Determin the next vert based on what side we are on
        if( m_slopeType == EST_LEFT )
        {
            m_vNext = m_vTop - 1;

            if( m_vNext < 0 )
                m_vNext = 2;
        }
        else
        {
            m_vNext = m_vTop + 1;

            if( m_vNext > 2 )
                m_vNext = 0;
        }

        // convert each y to an int first to avoid percision errors
        m_length = (int)m_pVert[m_vNext].vert.y - (int)m_pVert[m_vTop].vert.y;
        
        if( m_length > 0 )
        {
            // Init the step interpolation
            // The length needs to be an int for these divisions since plotting pixels is an exact number.
            // Using a floating point for the length introduces plotting errors and aritfacts
            m_step.vert.x = (m_pVert[m_vNext].vert.x - m_pVert[m_vTop].vert.x) / m_length;
            m_step.vert.z = (m_pVert[m_vNext].vert.z - m_pVert[m_vTop].vert.z) / m_length;
            m_step.uv.u   = (m_pVert[m_vNext].uv.u   - m_pVert[m_vTop].uv.u)   / m_length;
            m_step.uv.v   = (m_pVert[m_vNext].uv.v   - m_pVert[m_vTop].uv.v)   / m_length;

            // Init the slope starting point
            m_slope.vert.x = m_pVert[m_vTop].vert.x;
            m_slope.vert.z = m_pVert[m_vTop].vert.z;
            m_slope.uv.u   = m_pVert[m_vTop].uv.u;
            m_slope.uv.v   = m_pVert[m_vTop].uv.v;

            // Use an int for the y position
            y = m_pVert[m_vTop].vert.y;

            // See if any clipping is needed
            ClipTop();
        }

        // Move it to the next point on the triangle next time through
        m_vTop = m_vNext;

        return true;
    }

    return false;

}	// Init


/************************************************************************
*    desc:  Clip to the top of the buffer
************************************************************************/
void CTriangleSlope::ClipTop()
{
    // Perform object-precision clip on top of edge 
    // Is the top edge of the polygon above the Y of the screen?
    // If so, recalculate the X offset and set the Y to 0
    if( y < 0 )
    {
        float step(-y);

        // calculate a new x offset based on the y being zero
        m_slope.vert.x = m_pVert[m_vTop].vert.x + 
            ((m_pVert[m_vNext].vert.x - m_pVert[m_vTop].vert.x) / 
            ((int)m_pVert[m_vNext].vert.y - (int)m_pVert[m_vTop].vert.y)) * step;

        // Reset the Y to the top of the buffer
        y = 0;

        // calculate the new slope positions
        m_slope.vert.z += m_step.vert.z * step;
        m_slope.uv.u   += m_step.uv.u   * step;
        m_slope.uv.v   += m_step.uv.v   * step;

        // Reduce the length
        m_length -= step;
    }

}   // ClipTop


/************************************************************************
*    desc:  Increment the slope
************************************************************************/
void CTriangleSlope::Inc()
{
    // Init the step interpolation
    m_slope.vert.x += m_step.vert.x;
    m_slope.vert.z += m_step.vert.z;
    m_slope.uv.u   += m_step.uv.u;
    m_slope.uv.v   += m_step.uv.v;

    --m_length;
    ++y;

}   // Inc


