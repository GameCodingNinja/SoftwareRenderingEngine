
/************************************************************************
*    FILE NAME:       bitmask.cpp
*
*    DESCRIPTION:     Class to handle a bit mask
************************************************************************/

// Physical component dependency
#include <common/bitmask.h>

/************************************************************************
*    desc:  Constructer
************************************************************************/
CBitmask::CBitmask()
    : bitmask(0)
{
}

CBitmask::CBitmask( const uint args )
    : bitmask(args)
{
}


/************************************************************************
*    desc:  Add the passed in parameters to the bitmask
*
*	 param:	const uint args - parameters to Add
************************************************************************/
void CBitmask::Add( const uint args )
{
    bitmask |= args;

}


/************************************************************************
*    desc:  Remove the passed in parameters from the bitmask
*
*	 param:	const uint args - parameters to remove
************************************************************************/
void CBitmask::Remove( const uint args )
{
    bitmask &= args ^ -1;

}


/************************************************************************
*    desc:  Remove all parameters except for the ones passed in
*
*	 param:	const uint args - parameters to not remove
************************************************************************/
void CBitmask::RemoveAllExcept( const uint args )
{
    bitmask &= args;

}


/************************************************************************
*    desc:  Set the bitmask to zero
************************************************************************/
void CBitmask::Clear()
{
    bitmask = 0;

}


/************************************************************************
*    desc:  Check if one of the passed in parameters is set
*
*	 param:	const uint args - parameters to check
************************************************************************/
bool CBitmask::IsSet( const uint args ) const
{
    return (bitmask & args) != 0;

}


/************************************************************************
*    desc:  Check if all of the passed in parameters are set
*
*	 param:	const uint args - parameters to check
************************************************************************/
bool CBitmask::AreAllSet( const uint args ) const
{
    return (bitmask & args) == args;

}


/************************************************************************
*    desc:  Get a copy of the bitmask including the past in parameters
*
*	 param:	const uint args - parameters to set
************************************************************************/
const uint CBitmask::GetIncluding( const uint args ) const
{
    return bitmask | args;

}


/************************************************************************
*    desc:  Get a copy of the bitmask excluding the past in parameters
*
*	 param:	const uint args - parameters to remove
************************************************************************/
const uint CBitmask::GetExcluding( const uint args ) const
{
    return bitmask & (args ^ -1);

}


/************************************************************************
*    desc:  Overloaded assignment operator
*
*	 param:	const uint value - value to set to
************************************************************************/
void CBitmask::operator = ( const uint value )
{
    bitmask = value;

}

