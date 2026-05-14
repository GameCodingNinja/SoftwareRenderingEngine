
/************************************************************************
*    FILE NAME:       dynamicoffset.h
*
*    DESCRIPTION:     Dynamic Offset class
************************************************************************/  

#ifndef __dynamic_offset_h__
#define __dynamic_offset_h__

// Game lib dependencies
#include <common/point.h>
#include <common/size.h>
#include <common/defs.h>

// disable perfomance warning forcing value to bool 'true' or 'false'
#pragma warning(disable : 4800)

class CDynamicOffset
{
public:

    // EDynamicOffset
    enum EDynamicOffset
    {
        EDO_NULL=0,
        EDO_LEFT=1,
        EDO_RIGHT=2,
        EDO_HORZ_CENTER=4,
        EDO_TOP=8,
        EDO_BOTTOM=16,
        EDO_VERT_CENTER=32,
    };

    CDynamicOffset() : bitMask(EDO_NULL)
    {}

    // And the bit mask
    void AndBitMask( EDynamicOffset value )
    { bitMask |= value; }

    // Set/Get X
    void SetX( float value )
    { point.x = value; }

    float GetX() const
    { return point.x; }

    // Set/Get Y
    void SetY( float value )
    { point.y = value; }

    float GetY() const
    { return point.y; }

    // Check if bits were set
    bool IsBitSet( EDynamicOffset value )
    { return (bitMask & value); }

    // Is the dynamic offset being used
    bool IsDynamicOffset() const
    { return (bitMask != EDO_NULL); }

    // Get the dynamic position
    CPoint<float> GetDynamicPos( CPoint<float> pos, CSize<float> screenCenter )
    {
        // Strip out any fractional component for correct rendering
        screenCenter.Round();

        if( IsBitSet( CDynamicOffset::EDO_LEFT ) )
            pos.x = -(screenCenter.w - point.x);

        else if( IsBitSet( CDynamicOffset::EDO_RIGHT ) )
            pos.x = screenCenter.w - point.x;

        else if( IsBitSet( CDynamicOffset::EDO_HORZ_CENTER ) )
            pos.x = point.x;

        if( IsBitSet( CDynamicOffset::EDO_TOP ) )
            pos.y = screenCenter.h - point.y;
            
        else if( IsBitSet( CDynamicOffset::EDO_BOTTOM ) )
            pos.y = -(screenCenter.h - point.y);

        else if( IsBitSet( EDO_VERT_CENTER ) )
            pos.y = point.y;

        return pos;
    }

    // Get the dynamic position
    CPoint<float> GetDynamicPos( CSize<float> screenCenter )
    {
        return GetDynamicPos( CPoint<float>(), screenCenter );
    }

private:

    // bit mask
    uint bitMask;

    // offset
    CPoint<float> point;

};

#endif  // __dynamic_offset_h__

