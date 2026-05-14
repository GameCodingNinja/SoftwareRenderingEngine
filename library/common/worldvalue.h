
/************************************************************************
*    FILE NAME:       worldvalue.h
*
*    DESCRIPTION:     World value class
************************************************************************/

#ifndef __world_value_h__
#define __world_value_h__


class CWorldValue
{
public:

    // Constructors
    CWorldValue();
    CWorldValue( const CWorldValue & value );
    CWorldValue( int _i );
    CWorldValue( float _f );
    CWorldValue( int _i, float _f );
    CWorldValue( float _f, int _i );

    // Conform the world value to the sector size
    void ConformValue();
    CWorldValue GetConformedValue() const;

    // Convert the value in single float form
    void ToFloat();
    float GetFloat() const;
    operator float() const { return GetFloat(); }

    // Comparison Operators
    bool operator == ( const CWorldValue & value ) const;
    bool operator == ( const float value ) const;

    bool operator != ( const CWorldValue & value ) const;
    bool operator != ( const float value ) const;

    bool operator < ( const CWorldValue & value ) const;
    bool operator < ( const float value ) const;

    bool operator > ( const CWorldValue & value ) const;
    bool operator > ( const float value ) const;

    bool operator <= ( const CWorldValue & value ) const;
    bool operator <= ( const float value ) const;

    bool operator >= ( const CWorldValue & value ) const;
    bool operator >= ( const float value ) const;

    // Assignment Operator
    CWorldValue & operator = ( const CWorldValue & value );

    // Addition Operators
    CWorldValue operator + ( const CWorldValue & value ) const;
    CWorldValue operator + ( const float value ) const;
    void operator += ( const CWorldValue & value );
    void operator += ( const float value );

    // Subtraction Operators
    CWorldValue operator - ( const CWorldValue & value ) const;
    CWorldValue operator - ( const float value ) const;
    void operator -= ( const CWorldValue & value );
    void operator -= ( const float value );

    // Multiplication Operators
    CWorldValue operator * ( const CWorldValue & value ) const;
    CWorldValue operator * ( const int value ) const;
    CWorldValue operator * ( const float value ) const;
    void operator *= ( const CWorldValue & value );
    void operator *= ( const int value );
    void operator *= ( const float value );

    // Division Operators
    CWorldValue operator / ( const CWorldValue & value ) const;
    CWorldValue operator / ( const float value ) const;
    void operator /= ( const CWorldValue & value );
    void operator /= ( const float value );

    // Negative Operator
    CWorldValue operator - () const;

public:

    // The int and float values of the world point
    int i;
    float f;

    // sector size
    static const int SECTOR_SIZE;
    static const int HALF_SECTOR_SIZE;
};

#endif  // __classtemplate_h__


