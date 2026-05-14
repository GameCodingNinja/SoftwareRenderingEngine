
/************************************************************************
*    FILE NAME:       physicscomponent2d.cpp
*
*    DESCRIPTION:     Class for handling the physics part of the sprite
************************************************************************/

// Physical component dependency
#include <2d/physicscomponent2d.h>

// Standard lib dependencies

// Boost lib dependencies

// Game lib dependencies
#include <objectdata/objectdata2d.h>
#include <objectdata/objectphysicsdata2d.h>


/************************************************************************
*    desc:  Constructer
************************************************************************/
CPhysicsComponent2d::CPhysicsComponent2d( CSprite2D & sprite, const CObjectData2D & objectData )
                   : m_sprite(sprite)
{
}   // constructor


/************************************************************************
*    desc:  destructer                                                             
************************************************************************/
CPhysicsComponent2d::~CPhysicsComponent2d()
{
}   // destructer


/************************************************************************
*    desc:  Update the physics                                                           
************************************************************************/
void CPhysicsComponent2d::Update()
{
    if( IsActive() )
    {
    }

}   // Update


/************************************************************************
*    desc:  Is this component active?
************************************************************************/
bool CPhysicsComponent2d::IsActive()
{
    return false;

}   // IsActive



