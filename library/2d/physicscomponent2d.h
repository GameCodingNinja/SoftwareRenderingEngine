
/************************************************************************
*    FILE NAME:       physicscomponent2d.h
*
*    DESCRIPTION:     Class for handling the physics part of the sprite
************************************************************************/

#ifndef __physics_component_2d_h__
#define __physics_component_2d_h__

// Physical component dependency

// Standard lib dependencies

// Boost lib dependencies

// Game lib dependencies

// Forward declaration(s)
class CObjectData2D;
class CSprite2D;

class CPhysicsComponent2d
{
public:

    // Constructor
    CPhysicsComponent2d( CSprite2D & sprite, const CObjectData2D & objectData );

    // Destructor
    ~CPhysicsComponent2d();

    // Update the physics
    void Update();

    // Is this component active?
    bool IsActive();

private:

    // The sprite the physics will control
    CSprite2D & m_sprite;

};

#endif  // __physics_component_2d_h__


