
/************************************************************************
*    FILE NAME:       bitmask.h
*
*    DESCRIPTION:     Class to handle a bit mask
************************************************************************/

#ifndef __bit_mask_h__
#define __bit_mask_h__

// Game lib dependencies
#include <common/defs.h>

class CBitmask
{
public:

    // Constructor
    CBitmask();

    // Copy Constructors
    CBitmask( const uint args );

    // Add the passed in parameters to the bitmask
    void Add( const uint args );

    // Remove the passed in parameters from the bitmask
    void Remove( const uint args );

    // Set all parameters equal to zero except for the passed parameters
    void RemoveAllExcept( const uint args );

    // Set the bitmask to zero
    void Clear();

    // Check if one of the passed in parameters is set
    bool IsSet( const uint args ) const;

    // Check if all of the passed in parameters are set
    bool AreAllSet( const uint args ) const;

    // Get a copy of the bitmask including the past in parameters
    const uint GetIncluding( const uint args ) const;

    // Get a copy of the bitmask excluding the past in parameters
    const uint GetExcluding( const uint args ) const;

    // Overloaded assignment operator
    void operator = ( const uint value );

    // Get the bitmask as an uint
    operator uint() const { return bitmask; }

private:

    // Parameters of bit mask
    uint bitmask;

};

#endif  // __bit_mask_h__


