
/************************************************************************
*    FILE NAME:       worldvalue.cpp
*
*    DESCRIPTION:     World value class
************************************************************************/

// Physical component dependency
#include <common/worldvalue.h>

// Standard lib dependencies
#include <math.h>
#include <complex>

/************************************************************************
*    desc:  Constructer
************************************************************************/
CWorldValue::CWorldValue()
    : f(0),
      i(0)
{
}   // Constructor

CWorldValue::CWorldValue( const CWorldValue & value )
{
    i = value.i;
    f = value.f;
    ConformValue();

}   // Constructor

CWorldValue::CWorldValue( int _i )
    : i(0)
{
    f = static_cast<float>(_i);
    ConformValue();

}   // Constructor

CWorldValue::CWorldValue( float _f )
    : i(0)
{
    f = _f;
    ConformValue();

}   // Constructor

CWorldValue::CWorldValue( int _i, float _f )
{
    i = _i;
    f = _f;
    ConformValue();

}   // Constructor 

CWorldValue::CWorldValue( float _f, int _i )
{
    i = _i;
    f = _f;
    ConformValue();

}   // Constructor


/************************************************************************
*    desc:  Conform the value to the sector size
************************************************************************/
void CWorldValue::ConformValue()
{
    if( SECTOR_SIZE > 0 && std::abs(f) > HALF_SECTOR_SIZE )
    {
        // Adjusted absolute value
        int iAdjust = 0;

        if( f > 0 )
            iAdjust = int( ( f / float(SECTOR_SIZE) ) + 0.5f );
        else
            iAdjust = int( ( f / float(SECTOR_SIZE) ) - 0.5f );

        f -= iAdjust * SECTOR_SIZE;
        i += iAdjust;
    }

}   // ConformValue


/************************************************************************
*    desc:  Return a copy of the value conformed to the sector size
*
*	 ret:	CWorldValue - conformed value
************************************************************************/
CWorldValue CWorldValue::GetConformedValue() const
{
    return CWorldValue(i,f);

}   // GetConformedValue


/************************************************************************
*    desc:  Convert the world value in single float form
************************************************************************/
void CWorldValue::ToFloat()
{
    if( i != 0 )
    {
        f += SECTOR_SIZE * i;
        i = 0;
    }

}   // ToFloat


/************************************************************************
*    desc:  Return a copy of the world value in single float form
*
*	 ret:	float - equal float value
************************************************************************/
float CWorldValue::GetFloat() const
{
    if( i == 0 )
        return f;

    return f + (SECTOR_SIZE * i);

}   // GetFloat


/***********************************************************************************
*	COMPARISON OPERATORS
***********************************************************************************/
/************************************************************************
*    desc:  Equality operator
*
*	 param: const value - value to compare
*
*	 ret:	bool - true or false
************************************************************************/
bool CWorldValue::operator == ( const CWorldValue & value ) const
{
    if( i == value.i && f == value.f )
        return true;

    return false;

}	// operator ==

bool CWorldValue::operator == ( const float value ) const
{
    if( GetFloat() == value )
        return true;

    return false;

}   // operator ==


/************************************************************************
*    desc:  Inequality operator
*
*	 param: const value - value to compare
*
*	 ret:	bool - true or false
************************************************************************/
bool CWorldValue::operator != ( const CWorldValue & value ) const
{
    if( i != value.i || f != value.f )
        return true;

    return false;

}   // operator !=

bool CWorldValue::operator != ( const float value ) const
{
    if( GetFloat() != value )
        return true;

    return false;

}   // operator !=


/************************************************************************
*    desc:  Less than operator
*
*	 param: const value - value to compare
*
*	 ret:	bool - true or false
************************************************************************/
bool CWorldValue::operator < ( const CWorldValue & value ) const
{
    if( i < value.i )
        return true;

    if( i == value.i && f < value.f )
        return true;

    return false;

}   // operator <

bool CWorldValue::operator < ( const float value ) const
{
    if( GetFloat() < value )
        return true;

    return false;

}   // operator <


/************************************************************************
*    desc:  Greater than operator
*
*	 param: const value - value to compare
*
*	 ret:	bool - true or false
************************************************************************/
bool CWorldValue::operator > ( const CWorldValue & value ) const
{
    if( i > value.i )
        return true;

    if( i == value.i && f > value.f )
        return true;

    return false;

}   // operator >

bool CWorldValue::operator > ( const float value ) const
{
    if( GetFloat() > value )
        return true;

    return false;

}   // operator >


/************************************************************************
*    desc:  Less than or equal to operator
*
*	 param: const value - value to compare
*
*	 ret:	bool - true or false
************************************************************************/
bool CWorldValue::operator <= ( const CWorldValue & value ) const
{
    if( i < value.i )
        return true;

    if( i == value.i && f <= value.f )
        return true;

    return false;

}   // operator <=

bool CWorldValue::operator <= ( const float value ) const
{
    if( GetFloat() <= value )
        return true;

    return false;

}   // operator <=


/************************************************************************
*    desc:  Greater than or equal to operator
*
*	 param: const value - value to compare
*
*	 ret:	bool - true or false
************************************************************************/
bool CWorldValue::operator >= ( const CWorldValue & value ) const
{
    if( i > value.i )
        return true;

    if( i == value.i && f >= value.f )
        return true;

    return false;

}   // operator >

bool CWorldValue::operator >= ( const float value ) const
{
    if( GetFloat() >= value )
        return true;

    return false;

}   // operator >


/***********************************************************************************
*	ASSIGNMENT OPERATOR
***********************************************************************************/
/************************************************************************
*    desc:  Assignment operator
*
*	 param: const point - point assign
************************************************************************/
CWorldValue & CWorldValue::operator = ( const CWorldValue & value )
{
    i = value.i;
    f = value.f;

    return *this;

}   // operator =


/***********************************************************************************
*	ADDITION OPERATORS
***********************************************************************************/
/************************************************************************
*    desc:  Addition operator
*
*	 param: const value - value to add
*
*	 ret:	CWorldValue - summation of values
************************************************************************/
CWorldValue CWorldValue::operator + ( const CWorldValue & value ) const
{
    return CWorldValue( i + value.i, f + value.f );

}   // operator +

CWorldValue CWorldValue::operator + ( const float value ) const
{
    return *this + CWorldValue(value);

}   // operator +

/************************************************************************
*    desc:  Addition operator
*
*	 param: const value - value to add
************************************************************************/
void CWorldValue::operator += ( const CWorldValue & value )
{
    *this = *this + value;

}   // operator +=

void CWorldValue::operator += ( const float value )
{
    *this = *this + value;

}   // operator +=


/***********************************************************************************
*	SUBTRACTION OPERATORS
***********************************************************************************/
/************************************************************************
*    desc:  Subtraction operator
*
*	 param: const value - value to subtract
*
*	 ret:	CWorldValue - subtraction of values
************************************************************************/
CWorldValue CWorldValue::operator - ( const CWorldValue & value ) const
{
    return CWorldValue( i - value.i, f - value.f );

}   // operator -

CWorldValue CWorldValue::operator - ( const float value ) const
{
    return *this - CWorldValue(value);

}   // operator -

/************************************************************************
*    desc:  Subtraction operator
*
*	 param: const value - value to subtract
************************************************************************/
void CWorldValue::operator -= ( const CWorldValue & value )
{
    *this = *this - value;

}   // operator -=

void CWorldValue::operator -= ( const float value )
{
    *this = *this - value;

}   // operator -=


/***********************************************************************************
*	MULTIPLICATION OPERATORS
***********************************************************************************/
/************************************************************************
*    desc:  Multiplication operator
*
*	 param: const value - value to multiply
*
*	 ret:	CWorldValue - product of values
************************************************************************/
CWorldValue CWorldValue::operator * ( const CWorldValue & value ) const
{
    return *this * value.GetFloat();

}   // operator *

CWorldValue CWorldValue::operator * ( const int value ) const
{
    return CWorldValue( i * value, f * value );

}   // operator *

CWorldValue CWorldValue::operator * ( const float value ) const
{
    CWorldValue tmp;
    float intMultiple;

    intMultiple = float(i) * value;
    tmp.i = int(intMultiple);
    intMultiple -= tmp.i;

    tmp.f = ( f * value ) + ( intMultiple * SECTOR_SIZE );

    tmp.ConformValue();

    return tmp;

}   // operator *

/************************************************************************
*    desc:  Multiplication operator
*
*	 param: const value - value to multiply
************************************************************************/
void CWorldValue::operator *= ( const CWorldValue & value )
{
    *this = *this * value;

}   // operator *=

void CWorldValue::operator *= ( const int value )
{
    *this = *this * value;

}   // operator *=

void CWorldValue::operator *= ( const float value )
{
    *this = *this * value;

}   // operator *=


/***********************************************************************************
*	DIVISION OPERATORS
*
*	WARNING: Using any division against a float value will result in an imprecise
*			 answer if the integer value is very high
***********************************************************************************/
/************************************************************************
*    desc:  Division operator
*
*	 param: const value - value to divide
*
*	 ret:	CWorldValue - division of values
************************************************************************/
CWorldValue CWorldValue::operator / ( const CWorldValue & value ) const
{
    return *this / value.GetFloat();

}   // operator /

CWorldValue CWorldValue::operator / ( const float value ) const
{
    CWorldValue tmp;
    double intMultiple;

    intMultiple = double(i) / (double)value;
    tmp.i = int(intMultiple);
    intMultiple -= tmp.i;

    tmp.f = ( f / value ) + static_cast<float>( intMultiple * SECTOR_SIZE );

    tmp.ConformValue();

    return tmp;

}   // operator /

/************************************************************************
*    desc:  Division operator
*
*	 param: const value - value to divide
************************************************************************/
void CWorldValue::operator /= ( const CWorldValue & value )
{
    *this = *this / value;

}   // operator /=

void CWorldValue::operator /= ( const float value )
{
    *this = *this / value;

}   // operator /=


/***********************************************************************************
*	NEGATIVE OPERATOR
***********************************************************************************/
/************************************************************************
*    desc:  Return a copy of the value with its signs flipped
*
*	 ret:	CWorldValue - value with signs flipped
************************************************************************/
CWorldValue CWorldValue::operator - () const
{
    return CWorldValue(-i,-f);

}   // operator -
