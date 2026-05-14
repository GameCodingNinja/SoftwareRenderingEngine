/************************************************************************
*    FILE NAME:       quad.h
*
*    DESCRIPTION:     Quad class
************************************************************************/

#ifndef __quad_h__
#define __quad_h__

// Game lib dependencies
#include <common/point.h>

class CQuad 
{
public:

    CQuad(){};

    CPoint<float> point[4];

    // Copy constructor
    CQuad( const CQuad & obj )
    {
        *this = obj;
    }

    /************************************************************************                                                             
    *    desc:  The equality operator 
    *
    *    param:  CQuad & quad - quad to check
    *
    *    return: bool - true or false
    ************************************************************************/
    bool operator == ( const CQuad & quad ) const
    {
        if( (quad.point[0] == point[0]) && (quad.point[1] == point[1]) && 
            (quad.point[2] == point[2]) && (quad.point[3] == point[3]) )
        {
            return true;
        }
        return false;

    }   // operator ==

    /************************************************************************                                                             
    *    desc:  The inequality operator 
    *
    *    param:  CQuad & quad - quad to check
    *
    *    return: bool - true or false
    ************************************************************************/
    bool operator != ( const CQuad & quad ) const
    {
        if( (quad.point[0] != point[0]) || (quad.point[1] != point[1]) || 
            (quad.point[2] != point[2]) || (quad.point[3] != point[3]) )
        {
            return true;
        }
        return false;

    }   // operator !=

    /************************************************************************                                                             
    *    desc:  The subtraction operator 
    *
    *    param:  CQuad & quad - quad to subtract
    *
    *    return: CQuad - subtracted quad
    ************************************************************************/
    CQuad operator - ( const CQuad & quad ) const
    {
        CQuad tmp;

        for( int i = 0; i < 4; ++i )
            tmp.point[i] = point[i] - quad.point[i];

        return tmp;

    }   // operator -

    /************************************************************************                                                             
    *    desc:  The subtraction operator 
    *
    *    param:  float * value - value to subtract
    *
    *    return: CQuad - subtracted quad
    ************************************************************************/
    CQuad operator - ( float value ) const
    {
        CQuad tmp;

        for( int i = 0; i < 4; ++i )
            tmp.point[i] = point[i] - value;

        return tmp;

    }   // operator -

    /************************************************************************                                                             
    *    desc:  The subtraction operator 
    *
    *    param:  CPoint * _point - point to subtract
    *
    *    return: CQuad - subtracted quad
    ************************************************************************/
    CQuad operator - ( const CPoint<float> & _point ) const
    {
        CQuad tmp;

        for( int i = 0; i < 4; ++i )
            tmp.point[i] = point[i] - _point;

        return tmp;

    }   // operator -

    /************************************************************************                                                             
    *    desc:  The addition operator 
    *
    *    param:  CQuad & quad - quad to add
    *
    *    return: CQuad - added quad
    ************************************************************************/
    CQuad operator + ( const CQuad & quad ) const
    {
        CQuad tmp;

        for( int i = 0; i < 4; ++i )
            tmp.point[i] = point[i] + quad.point[i];

        return tmp;

    }   // operator +

    /************************************************************************                                                             
    *    desc:  The addition operator 
    *
    *    param:  float value - value to add
    *
    *    return: CQuad - added quad
    ************************************************************************/
    CQuad operator + ( float value ) const
    {
        CQuad tmp;

        for( int i = 0; i < 4; ++i )
            tmp.point[i] = point[i] + value;

        return tmp;

    }   // operator +

    /************************************************************************                                                             
    *    desc:  The subtraction operator 
    *
    *    param:  CPoint * _point - point to add
    *
    *    return: CQuad - added quad
    ************************************************************************/
    CQuad operator + ( const CPoint<float> & _point ) const
    {
        CQuad tmp;

        for( int i = 0; i < 4; ++i )
            tmp.point[i] = point[i] + _point;

        return tmp;

    }   // operator +

    /************************************************************************                                                             
    *    desc:  The multiplication operator 
    *
    *    param:  CQuad & quad * quad to multiply
    *
    *    return: CQuad - multiplied quad
    ************************************************************************/
    CQuad operator * ( const CQuad & quad ) const
    {
        CQuad tmp;

        for( int i = 0; i < 4; ++i )
            tmp.point[i] = point[i] * quad.point[i];

        return tmp;

    }   // operator *

    /************************************************************************                                                             
    *    desc:  The multiplication operator 
    *
    *    param:  float value - value to multiply
    *
    *    return: CQuad - multiplied quad
    ************************************************************************/
    CQuad operator * ( float value ) const
    {
        CQuad tmp;

        for( int i = 0; i < 4; ++i )
            tmp.point[i] = point[i] * value;

        return tmp;

    }   // operator *

    /************************************************************************                                                             
    *    desc:  The subtraction operator 
    *
    *    param:  float * value - point to multiply
    *
    *    return: CQuad - multiplied quad
    ************************************************************************/
    CQuad operator * ( const CPoint<float> & _point ) const
    {
        CQuad tmp;

        for( int i = 0; i < 4; ++i )
            tmp.point[i] = point[i] * _point;

        return tmp;

    }   // operator *

    /************************************************************************                                                             
    *    desc:  The multiplication operator 
    *
    *    param:  CQuad & quad * quad to divide by
    *
    *    return: CQuad - divided by quad
    ************************************************************************/
    CQuad operator / ( const CQuad & quad ) const
    {
        CQuad tmp;

        for( int i = 0; i < 4; ++i )
            tmp.point[i] = point[i] / quad.point[i];

        return tmp;

    }   // operator /

    /************************************************************************                                                             
    *    desc:  The multiplication operator 
    *
    *    param:  float value - value to divide by
    *
    *    return: CQuad - divided by quad
    ************************************************************************/
    CQuad operator / ( float value ) const
    {
        CQuad tmp;

        for( int i = 0; i < 4; ++i )
            tmp.point[i] = point[i] / value;

        return tmp;

    }   // operator /

    /************************************************************************                                                             
    *    desc:  The subtraction operator 
    *
    *    param:  CPoint * _point - point to divide by
    *
    *    return: CQuad - divided by quad
    ************************************************************************/
    CQuad operator / ( const CPoint<float> & _point ) const
    {
        CQuad tmp;

        for( int i = 0; i < 4; ++i )
            tmp.point[i] = point[i] / _point;

        return tmp;

    }   // operator /

    /************************************************************************                                                             
    *    desc:  The addition operator 
    *
    *    param:  CQuad & quad - quad to add
    *
    *    return: CQuad - added quad
    ************************************************************************/
    CQuad operator += ( const CQuad & quad )
    {
        for( int i = 0; i < 4; ++i )
            point[i] += quad.point[i];

        return *this;

    }   // operator +=

    /************************************************************************                                                             
    *    desc:  The addition operator 
    *
    *    param:  float value - value to add
    *
    *    return: CQuad - added quad
    ************************************************************************/
    CQuad operator += ( float value )
    {
        for( int i = 0; i < 4; ++i )
            point[i] += value;

        return *this;

    }   // operator +=

    /************************************************************************                                                             
    *    desc:  The addition operator 
    *
    *    param:  CPoint * _point - point to add
    *
    *    return: CQuad - added quad
    ************************************************************************/
    CQuad operator += ( const CPoint<float> & _point )
    {
        for( int i = 0; i < 4; ++i )
            point[i] += _point;

        return *this;

    }   // operator +=

    /************************************************************************                                                             
    *    desc:  The subtraction operator 
    *
    *    param:  CQuad & quad - quad to subtract
    *
    *    return: CQuad - subtracted quad
    ************************************************************************/
    CQuad operator -= ( const CQuad & quad )
    {
        for( int i = 0; i < 4; ++i )
            point[i] -= quad.point[i];

        return *this;

    }   // operator -=

    /************************************************************************                                                             
    *    desc:  The subtraction operator 
    *
    *    param:  float value - value to subtract
    *
    *    return: CQuad - subtracted quad
    ************************************************************************/
    CQuad operator -= ( float value )
    {
        for( int i = 0; i < 4; ++i )
            point[i] -= value;

        return *this;

    }   // operator -=

    /************************************************************************                                                             
    *    desc:  The subtraction operator 
    *
    *    param:  CPoint * _point - point to subtract
    *
    *    return: CQuad - subtracted quad
    ************************************************************************/
    CQuad operator -= ( const CPoint<float> & _point )
    {
        for( int i = 0; i < 4; ++i )
            point[i] -= _point;

        return *this;

    }   // operator -=

    /************************************************************************                                                             
    *    desc:  The multiplication operator 
    *
    *    param:  CQuad & quad - quad to multiply
    *
    *    return: CQuad - multiplied quad
    ************************************************************************/
    CQuad operator *= ( const CQuad & quad )
    {
        for( int i = 0; i < 4; ++i )
            point[i] *= quad.point[i];

        return *this;

    }   // operator *=

    /************************************************************************                                                             
    *    desc:  The multiplication operator 
    *
    *    param:  float value * value to multiply
    *
    *    return: CQuad - multiplied quad
    ************************************************************************/
    CQuad operator *= ( float value )
    {
        for( int i = 0; i < 4; ++i )
            point[i] *= value;

        return *this;

    }   // operator *=

    /************************************************************************                                                             
    *    desc:  The subtraction operator 
    *
    *    param:  CPoint * _point - point to multiply
    *
    *    return: CQuad - multiplied quad
    ************************************************************************/
    CQuad operator *= ( const CPoint<float> & _point )
    {
        for( int i = 0; i < 4; ++i )
            point[i] *= _point;

        return *this;

    }   // operator *=

    /************************************************************************                                                             
    *    desc:  The division operator 
    *
    *    param:  CQuad & quad / quad to divide by
    *
    *    return: CQuad - divided by quad
    ************************************************************************/
    CQuad operator /= ( const CQuad & quad )
    {
        for( int i = 0; i < 4; ++i )
            point[i] /= quad.point[i];

        return *this;

    }   // operator /=

    /************************************************************************                                                             
    *    desc:  The division operator 
    *
    *    param:  float value / value to divide by
    *
    *    return: CQuad - divided by quad
    ************************************************************************/
    CQuad operator /= ( float value )
    {
        for( int i = 0; i < 4; ++i )
            point[i] /= value;

        return *this;

    }   // operator /=

    /************************************************************************                                                             
    *    desc:  The division operator 
    *
    *    param:  CPoint * _point - value to divide by
    *
    *    return: CQuad - divided by quad
    ************************************************************************/
    CQuad operator /= ( const CPoint<float> & _point )
    {
        for( int i = 0; i < 4; ++i )
            point[i] /= _point;

        return *this;

    }   // operator /=

    /************************************************************************                                                             
    *    desc:  Is the point in the Quad
    *           Note: Fast but does not work when quad is rotated
    ************************************************************************/
    bool IsPointInQuad( float x, float y )
    {
        bool result(false);
        for( int i = 0, j = 3; i < 4; j = i++ )
        {
            if( ((point[i].y > y) != (point[j].y > y)) && 
                (x < (point[j].x - point[i].x) * (y - point[i].y) / (point[j].y - point[i].y) + point[i].x) )
            {
                result = !result;
            }
        }

        return result;
    }

};

#endif  // __quad_h__