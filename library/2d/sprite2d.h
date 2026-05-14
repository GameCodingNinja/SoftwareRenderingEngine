
/************************************************************************
*    FILE NAME:       sprite2d.h
*
*    DESCRIPTION:     2D sprite class
************************************************************************/

#ifndef __sprite_2d_h__
#define __sprite_2d_h__

// Physical component dependency
#include <common/object.h>

// Standard lib dependencies
#include <boost/noncopyable.hpp>

// Game lib dependencies
#include <2d/visualcomponent2d.h>
#include <2d/physicscomponent2d.h>
#include <2d/spritescriptcomponent2d.h>

// Forward declaration(s)
class CObjectData2D;

class CSprite2D : public CObject, boost::noncopyable
{
public:

    // Constructor
    CSprite2D( const CObjectData2D & objectData );

    // Destructor
    virtual ~CSprite2D();

    // Get the object data
    const CObjectData2D & GetObjectData() const;

    // Get the visual component
    CVisualComponent2d & GetVisualComponent();

    // Get the physics component
    CPhysicsComponent2d & GetPhysicsComponent();

    // Get the scripting component
    CSpriteScriptComponent2d & GetScriptComponent();

    // Update the sprite 
    void Update();

    // do the render
    void Render( const CMatrix & matrix );

private:

    // The object data
    const CObjectData2D & m_objectData;

    // The visual part of the 2d sprite
    CVisualComponent2d m_visualComponent;

    // The physics part of the 2d sprite
    CPhysicsComponent2d m_physicsComponent;

    // The script part of the 2d sprite
    CSpriteScriptComponent2d m_scriptComponent;

    // The projection matrix
    const CMatrix m_projectionMatrix;

};

#endif  // __sprite_2d_h__


