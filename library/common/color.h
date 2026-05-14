/************************************************************************
*    FILE NAME:       color.h
*
*    DESCRIPTION:     Color class
************************************************************************/

#ifndef __color_h__
#define __color_h__

// Standard lib dependencies
#include <string>

// Boost lib dependencies
#include <boost/format.hpp>

// Game lib dependencies
#include <common/defs.h>

#pragma warning(disable : 4244)

/************************************************************************
*	Class: CColor
*
*	Description: Class for holding color values
************************************************************************/
class CColor
{
public:

    float r, g, b, a;

    /************************************************************************                                                             
    *    desc:  Constructor
    ************************************************************************/
    CColor():r(1),g(1),b(1),a(1){};
    CColor(float _r, float _g, float _b, float _a) : r(_r), g(_g), b(_b), a(_a)
    { Convert(); }

    // Copy constructor
    CColor( const CColor & color ) : r(color.r), g(color.g), b(color.b), a(color.a)
    { Convert(); }

    
    /************************************************************************                                                             
    *    desc:  Get functions
    ************************************************************************/
    float GetR() const
    { return r; }

    float GetG() const
    { return g; }

    float GetB() const
    { return b; }

    float GetA() const
    { return a; }

    /************************************************************************                                                             
    *    desc:  Convert from RGBA to 
    ************************************************************************/
    void Convert()
    {
        // See if we are using whole RGB numbers and do the conversion
        // 0.00390625f = 1 / 256;
        if( r > 1.5f )
                r *= defs_RGB_TO_DEC;

        if( g > 1.5f )
                g *= defs_RGB_TO_DEC;

        if( b > 1.5f )
                b *= defs_RGB_TO_DEC;

        if( a > 1.5f )
                a *= defs_RGB_TO_DEC;
    }

    /************************************************************************                                                             
    *    desc:  Clear the values
    ************************************************************************/
    void Clear()
    {
        r = 0;
        g = 0;
        b = 0;
        a = 0;
    }

    /************************************************************************                                                             
    *    desc:  Reset the values back to 1
    ************************************************************************/
    void Reset()
    {
        r = 1;
        g = 1;
        b = 1;
        a = 1;
    }

    /************************************************************************                                                             
    *    desc:  The equality operator 
    *
    *    param: CColor & color - color to equate
    *
    *    ret:	bool - true or false
    ************************************************************************/
    bool operator == ( const CColor & color ) const
    {
        if( (color.r == r) && (color.g == g) && (color.b == b) && (color.a == a) )
        {
            return true;
        }
        return false;

    }   // operator ==

    /************************************************************************                                                             
    *    desc:  The inequality operator 
    *
    *    param: CColor & color - color to check
    *
    *    ret:	bool - true or false
    ************************************************************************/
    bool operator != ( const CColor & color ) const
    {
        if( ( color.r != r ) || ( color.g != g ) || ( color.b != b ) || ( color.a != a ) )
        {
            return true;
        }
        return false;

    }   // operator !=


    /********************************************************************************************                                                             
    *    SUBTRACTION
    ********************************************************************************************/

    /************************************************************************                                                             
    *    desc:  The subtraction operator 
    *
    *    param: CColor & color - color to subtract
    *
    *    ret:	CColor - subtracted color
    ************************************************************************/
    CColor operator - ( const CColor & color ) const
    {
        CColor tmp;
        tmp.r = r - color.r;
        tmp.g = g - color.g;
        tmp.b = b - color.b;
        tmp.a = a - color.a;

        return tmp;

    }   // operator -

    /************************************************************************                                                             
    *    desc:  The subtraction operator 
    *
    *    param: type * value - value to subtract
    *
    *    ret:	CColor - subtracted color
    ************************************************************************/
    CColor operator - ( float value ) const
    {
        CColor tmp;
        tmp.r = r - value;
        tmp.g = g - value;
        tmp.b = b - value;
        tmp.a = a - value;

        return tmp;

    }   // operator -

    /************************************************************************                                                             
    *    desc:  The addition operator 
    *
    *    param:  CColor & color - color to add
    *
    *    return: CColor - added color
    ************************************************************************/
    CColor operator -= ( const CColor & color )
    {
        r -= color.r;
        g -= color.g;
        b -= color.b;
        a -= color.a;

        return *this;

    }   // operator -=

    /************************************************************************                                                             
    *    desc:  The addition operator 
    *
    *    param: type value - value to add
    *
    *    ret:	CColor - added color
    ************************************************************************/
    CColor operator -= ( float value )
    {
        r -= value;
        g -= value;
        b -= value;
        a -= value;

        return *this;

    }   // operator -=


    /********************************************************************************************                                                             
    *    ADDITION
    ********************************************************************************************/

    /************************************************************************                                                             
    *    desc:  The addition operator 
    *
    *    param: CColor & color - color to add
    *
    *    ret:	CColor - added color
    ************************************************************************/
    CColor operator + ( const CColor & color ) const
    {
        CColor tmp;
        tmp.r = r + color.r;
        tmp.g = g + color.g;
        tmp.b = b + color.b;
        tmp.a = a + color.a;

        return tmp;

    }   // operator +

    /************************************************************************                                                             
    *    desc:  The addition operator 
    *
    *    param: type value - value to add
    *
    *    ret:	CColor - added color
    ************************************************************************/
    CColor operator + ( float value ) const
    {
        CColor tmp;
        tmp.r = r + value;
        tmp.g = g + value;
        tmp.b = b + value;
        tmp.a = a + value;

        return tmp;

    }   // operator +

    /************************************************************************                                                             
    *    desc:  The addition operator 
    *
    *    param: CColor & color - color to add
    *
    *    ret:	CColor - added color
    ************************************************************************/
    CColor operator += ( const CColor & color )
    {
        r += color.r;
        g += color.g;
        b += color.b;
        a += color.a;

        return *this;

    }   // operator +=

    /************************************************************************                                                             
    *    desc:  The addition operator 
    *
    *    param: type value - value to add
    *
    *    ret:	CColor - added color
    ************************************************************************/
    CColor operator += ( float value )
    {
        r += value;
        g += value;
        b += value;
        a += value;

        return *this;

    }   // operator +=


    /********************************************************************************************                                                             
    *    DIVISION
    ********************************************************************************************/

    /************************************************************************                                                             
    *    desc:  The division operator 
    *
    *    param: CColor & color - color to divide by
    *
    *    ret:	CColor - divided by color
    ************************************************************************/
    CColor operator / ( const CColor & color ) const
    {
        CColor tmp;
        tmp.r = r / color.r;
        tmp.g = g / color.g;
        tmp.b = b / color.b;
        tmp.a = a / color.a;

        return tmp;

    }   // operator /

    /************************************************************************                                                             
    *    desc:  The addition operator 
    *
    *    param: type value - value to divide by
    *
    *    ret:	CColor - divided by color
    ************************************************************************/
    CColor operator / ( float value ) const
    {
        CColor tmp;
        tmp.r = r / value;
        tmp.g = g / value;
        tmp.b = b / value;
        tmp.a = a / value;

        return tmp;

    }   // operator /

    /************************************************************************                                                             
    *    desc:  The division operator 
    *
    *    param: CColor & color / color to divide
    *
    *    ret:	CColor - divided color
    ************************************************************************/
    CColor operator /= ( const CColor & color )
    {
        r /= color.r;
        g /= color.g;
        b /= color.b;
        a /= color.a;

        return *this;

    }   // operator *=

    /************************************************************************                                                             
    *    desc:  The division operator 
    *
    *    param: type value / value to divide
    *
    *    ret:	CColor - divided color
    ************************************************************************/
    CColor operator /= ( float value )
    {
        r /= value;
        g /= value;
        b /= value;
        a /= value;

        return *this;

    }   // operator *=


    /********************************************************************************************                                                             
    *    MULTIPLICATION
    ********************************************************************************************/

    /************************************************************************                                                             
    *    desc:  The multiplication operator 
    *
    *    param: CColor & color * color to multiply
    *
    *    ret:	CColor - multiplied color
    ************************************************************************/
    CColor operator * ( const CColor & color ) const
    {
        CColor tmp;
        tmp.r = r * color.r;
        tmp.g = g * color.g;
        tmp.b = b * color.b;
        tmp.a = a * color.a;

        return tmp;

    }   // operator *

    /************************************************************************                                                             
    *    desc:  The multiplication operator 
    *
    *    param: type value - value to multiply
    *
    *    ret:	CColor - multiplied color
    ************************************************************************/
    CColor operator * ( float value ) const
    {
        CColor tmp;
        tmp.r = r * value;
        tmp.g = g * value;
        tmp.b = b * value;
        tmp.a = a * value;

        return tmp;

    }   // operator *

    /************************************************************************                                                             
    *    desc:  The multiplication operator 
    *
    *    param: CColor & point - color to multiply
    *
    *    ret:	CColor - multiplied color
    ************************************************************************/
    CColor operator *= ( const CColor & color )
    {
        r *= color.r;
        g *= color.g;
        b *= color.b;
        a *= color.a;

        return *this;

    }   // operator *=

    /************************************************************************                                                             
    *    desc:  The multiplication operator 
    *
    *    param: type value * value to multiply
    *
    *    ret:	CColor - multiplied color
    ************************************************************************/
    CColor operator *= ( float value )
    {
        r *= value;
        g *= value;
        b *= value;
        a *= value;

        return *this;

    }   // operator *=


    /************************************************************************                                                             
    *    desc:  Set the point data
    ************************************************************************/
    void Set( float _r, float _g, float _b, float _a )
    {
        r = _r;
        g = _g;
        b = _b;
        a = _a;

    }   // Set


    /************************************************************************                                                             
    *    desc:  HSV transformation 
    *
    *    param: type hue - hue shift (in degrees)
    *    param: type sat - saturation multiplier (scalar)
    *    param: type val - value multiplier (scalar)
    *
    *    ret:	CColor - adjusted color
    ************************************************************************/
    /*CColor TransformHSV( float hue, float sat, float val )
    {
        float VSU = val * sat * cos(hue * defs_DEG_TO_RAD);
        float VSW = val * sat * sin(hue * defs_DEG_TO_RAD);

        CColor ret(r,g,b,a);

        ret.r = (.299 * val + .701 * VSU + .168 * VSW) * r
                  + (.587 * val - .587 * VSU + .330 * VSW) * g
                  + (.114 * val - .114 * VSU - .497 * VSW) * b;
        ret.g = (.299 * val - .299 * VSU - .328 * VSW) * r
                  + (.587 * val + .413 * VSU + .035 * VSW) * g
                  + (.114 * val - .114 * VSU + .292 * VSW) * b;
        ret.b = (.299 * val - .3 * VSU + 1.25 * VSW) * r
                  + (.587 * val - .588 * VSU - 1.05 * VSW) * g
                  + (.114 * val + .886 * VSU -.203 * VSW) * b;

        return ret;
    }*/

    void TransformHSV( float hue, float sat, float val )
    {
        float VSU = val * sat * cos(hue * defs_DEG_TO_RAD);
        float VSW = val * sat * sin(hue * defs_DEG_TO_RAD);

        float _r(r), _g(g), _b(b);

        r = (.299 * val + .701 * VSU + .168 * VSW) * _r
          + (.587 * val - .587 * VSU + .330 * VSW) * _g
          + (.114 * val - .114 * VSU - .497 * VSW) * _b;
        g = (.299 * val - .299 * VSU - .328 * VSW) * _r
          + (.587 * val + .413 * VSU + .035 * VSW) * _g
          + (.114 * val - .114 * VSU + .292 * VSW) * _b;
        b = (.299 * val - .3 * VSU + 1.25 * VSW) * _r
          + (.587 * val - .588 * VSU - 1.05 * VSW) * _g
          + (.114 * val + .886 * VSU -.203 * VSW) * _b;
    }


    /************************************************************************                                                             
    *    desc:  Get a string of the color's data
    *
    *	 ret:	string - string of the data
    ************************************************************************/
    std::string Dump() const
    {
        std::string printout = "Color( " +
            str( boost::format("%d") % r ) + "r, " +
            str( boost::format("%d") % g ) + "g, " +
            str( boost::format("%d") % b ) + "b, " +
            str( boost::format("%d") % a ) + "a )";

        return printout;

    }	// Dump

};


#endif  // __color_h__