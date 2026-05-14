/************************************************************************
*    FILE NAME:       collisionfunc2d.cpp
*
*    DESCRIPTION:     2D collision functions
************************************************************************/  

// Physical component dependency
#include <utilities/mathfunc.h>

namespace NMathFunc
{
    /************************************************************************
    *    desc:  Calculate the dot product of the passed in values  
    *
    *	 param: const CPoint/CWorldPoint & p1 - point to compare
    *			const CPoint/CWorldPoint & p2 - point to compare 
    *
    *	 ret:	float/CWorldValue - result of dot product
    ************************************************************************/
    float DotProduct2D( const CPoint<float> & p1, const CPoint<float> & p2 )
    {
        return p1.x * p2.x + p1.y * p2.y;

    }	// DotProduct2D 

    CWorldValue DotProduct2D( const CPoint<CWorldValue> & p1, const CPoint<CWorldValue> & p2 )
    {
        return p1.x * p2.x + p1.y * p2.y;

    }	// DotProduct2D 

    CWorldValue DotProduct2D( const CPoint<CWorldValue> & p1, const CPoint<float> & p2 )
    {
        return p1.x * p2.x + p1.y * p2.y;

    }	// DotProduct2D


    /************************************************************************
    *    desc:  Calculate the cross product of the passed in values  
    *
    *	 param: CPoint/float - value to compare
    *			CPoint/float - value to compare 
    *
    *	 ret:	float/CPoint - result of cross product
    ************************************************************************/
    float CrossProduct2D( const CPoint<float> & p1, const CPoint<float> & p2 )
    {
        return p1.x * p2.y - p1.y * p2.x;

    }	// CrossProduct2D

    CPoint<float> CrossProduct2D( const CPoint<float> & p, float s )
    {
        return CPoint<float>( s * p.y, -s * p.x );

    }	// CrossProduct2D
 
    CPoint<float> CrossProduct2D( float s, const CPoint<float> & p )
    {
        return CPoint<float>( -s * p.y, s * p.x );

    }	// CrossProduct2D


    /************************************************************************
    *    desc:  Perform a modulus operation on the passed in floats 
    *
    *	 param: float v1, v2 - values to mod
    *
    *	 ret:	float - result
    ************************************************************************/
    float Modulus( float v1, float v2 )
    {
        return v1 - v2 * floor(v1 / v2);

    }	// Modulus
}

