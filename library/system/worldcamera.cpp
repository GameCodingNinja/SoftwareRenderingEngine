/************************************************************************
*    FILE NAME:       worldcamera.cpp
*
*    DESCRIPTION:     Class that holds the world camera position and 
*					  rotation
************************************************************************/

// Physical component dependency
#include <system/worldcamera.h>


/************************************************************************
*    desc:  Constructor                                                             
************************************************************************/
CWorldCamera::CWorldCamera()
{
}   // Constructor


/************************************************************************
*    desc:  Destructor                                                             
************************************************************************/
CWorldCamera::~CWorldCamera()
{
}   // Destructor


/************************************************************************
*    desc:  Set the camera's world position
*
*	 param:	CWorldPoint & pos - position to set to
************************************************************************/  
void CWorldCamera::SetPos( const CPoint<CWorldValue> & pos )
{
    CObject::SetPos( -pos );

}   // setPos


/************************************************************************
*    desc:  Increment the camera's world position
*
*	 param:	CPoint & pos - amount to increment
************************************************************************/  
void CWorldCamera::IncPos( const CPoint<CWorldValue> & pos )
{
    CObject::IncPos( -pos );

}   // incPos

