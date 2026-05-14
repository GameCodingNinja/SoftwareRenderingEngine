
/************************************************************************
*    FILE NAME:       radian.h
*
*    DESCRIPTION:     3D Radian class
************************************************************************/  

#ifndef __radian_h__
#define __radian_h__

// Physical component dependency
#include <common/point.h>

// Game lib dependencies
#include <common/defs.h>

#pragma warning(disable : 4244)

template <class type>
class CRadian : public CPoint<type>
{
public:

    // constructor
    CRadian()
    {
    }

    // Copy constructor
    CRadian( float _x, float _y, float _z )
        : CPoint<type>( _x, _y, _z )
    {
    }

    // Copy constructor
    CRadian( const CPoint<type> &obj )
    {
        CPoint<type>::x = obj.x * defs_DEG_TO_RAD;
        CPoint<type>::y = obj.y * defs_DEG_TO_RAD;
        CPoint<type>::z = obj.z * defs_DEG_TO_RAD;
    }

};

#endif  // __radian_h__

