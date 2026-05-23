
/************************************************************************
*    FILE NAME:       triangleslope.h
*
*    DESCRIPTION:     Triangle slope class (templated on vertex type)
************************************************************************/

#ifndef __triangle_slope_h__
#define __triangle_slope_h__

// Standard lib dependencies
#include <vector>

// Game lib dependencies
#include <common/vertex.h>

template <typename VertexType>
class CTriangleSlope
{
public:

    enum ESlopeType
    {
        EST_LEFT,
        EST_RIGHT
    };

    // Constructor
    CTriangleSlope( const VertexType trans[], int vTop, ESlopeType slopeType ) :
        m_length(0),
        y(0),
        m_pVert( trans ),
        m_vTop( vTop ),
        m_vNext( vTop ),
        m_slopeType( slopeType )
    {
    }

    // Destructor
    ~CTriangleSlope()
    {
    }

    // Init the slope
    bool Init()
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
    }

    // Increment the slope
    void Inc()
    {
        m_slope.vert.x += m_step.vert.x;
        m_slope.vert.z += m_step.vert.z;
        m_slope.uv.u   += m_step.uv.u;
        m_slope.uv.v   += m_step.uv.v;

        --m_length;
        ++y;
    }

    // Advance the slope by multiple lines at once (skip scanlines)
    void Advance( int lines )
    {
        m_slope.vert.x += m_step.vert.x * lines;
        m_slope.vert.z += m_step.vert.z * lines;
        m_slope.uv.u   += m_step.uv.u   * lines;
        m_slope.uv.v   += m_step.uv.v   * lines;

        y += lines;
        m_length -= lines;
    }

    // slope position
    VertexType m_slope;

    // Length of slope
    int m_length;

    // Y offset
    int y;

private:

    // Clip to the top of the buffer
    void ClipTop()
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
    }

private:

    // Pointer to projected verts
    const VertexType * m_pVert;

    // Top vert index
    int m_vTop;

    // Next vert index
    int m_vNext;

    // step interpolation
    VertexType m_step;

    // The type of slope
    const ESlopeType m_slopeType;

};

// Convenience typedefs
using CTriangleSlope2d = CTriangleSlope<CVertex2d>;
using CTriangleSlope3d = CTriangleSlope<CVertex3d>;

#endif

