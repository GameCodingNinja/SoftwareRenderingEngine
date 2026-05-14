
/************************************************************************
*    FILE NAME:       sprite2d.cpp
*
*    DESCRIPTION:     2D sprite class
************************************************************************/

// Physical component dependency
#include <2d/sprite2d.h>

// Game lib dependencies
#include <objectdata/objectdata2d.h>
#include <objectdata/objectphysicsdata2d.h>

// disable warning about 'this' used in base member initializer list
// It's "ok" as long as it's not being used in the member constructor
// because the class isn't finished being initialzed yet
#pragma warning(disable : 4355)

/************************************************************************
*    desc:  Constructer
************************************************************************/
CSprite2D::CSprite2D( const CObjectData2D & objectData )
    : m_objectData( objectData ),
      m_visualComponent( objectData ),
      m_physicsComponent(*this, objectData),
      m_scriptComponent(*this, objectData)
{
}   // constructor


/************************************************************************
*    desc:  destructer                                                             
************************************************************************/
CSprite2D::~CSprite2D()
{
}	// destructer


/************************************************************************
*    desc:  Update the sprite                                                           
************************************************************************/
void CSprite2D::Update()
{
    m_physicsComponent.Update();

    m_scriptComponent.Update();

}   // Update


/************************************************************************
*    desc:  do the render                                                            
************************************************************************/
void CSprite2D::Render( const CMatrix & matrix )
{
    if( m_visible )
        m_visualComponent.Render( m_trans_matrix * matrix );

}	// Render


/************************************************************************
*    desc:  Get the visual component                                                            
************************************************************************/
CVisualComponent2d & CSprite2D::GetVisualComponent()
{
    return m_visualComponent;

}   // GetVisualComponent


/************************************************************************
*    desc:  Get the physics component                                                            
************************************************************************/
CPhysicsComponent2d & CSprite2D::GetPhysicsComponent()
{
    return m_physicsComponent;

}   // GetPhysicsComponent


/************************************************************************
*    desc:  Get the scripting component                                                            
************************************************************************/
CSpriteScriptComponent2d & CSprite2D::GetScriptComponent()
{
    return m_scriptComponent;

}   // GetScriptingComponent


/************************************************************************
*    desc:  Get the object data                                                            
************************************************************************/
const CObjectData2D & CSprite2D::GetObjectData() const
{
    return m_objectData;

}   // GetObjectData
