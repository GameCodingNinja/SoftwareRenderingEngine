
/************************************************************************
*    FILE NAME:       sprite.cpp
*
*    DESCRIPTION:     Sprite class
************************************************************************/

// Physical component dependency
#include <sprite/sprite.h>

// Game lib dependencies
#include <objectdata/iobjectdata.h>
#include <objectdata/objectdata2d.h>
#include <objectdata/objectdata3d.h>
#include <2d/visualcomponent2d.h>
#include <3d/visualcomponent3d.h>

/************************************************************************
*    desc:  Constructer
************************************************************************/
CSprite::CSprite( const iObjectData & objectData )
    : m_objectData( objectData )
{
    if( objectData.Is2D() )
    {
        m_upVisualComponent.reset( new CVisualComponent2d( static_cast<const CObjectData2D &>(objectData) ) );
    }
    else if( objectData.Is3D() )
    {
        m_upVisualComponent.reset( new CVisualComponent3d( static_cast<const CObjectData3D &>(objectData) ) );
    }

}   // constructor


/************************************************************************
*    desc:  destructer                                                             
************************************************************************/
CSprite::~CSprite()
{
}	// destructer


/************************************************************************
*    desc:  Update the sprite                                                           
************************************************************************/
void CSprite::Update()
{
}   // Update


/************************************************************************
*    desc:  do the render                                                            
************************************************************************/
void CSprite::Render( const CMatrix & matrix )
{
    if( isVisible() )
        m_upVisualComponent->Render( m_matrix * matrix );

}	// Render


/************************************************************************
*    desc:  Get the visual component                                                            
************************************************************************/
iVisualComponent * CSprite::GetVisualComponent()
{
    return m_upVisualComponent.get();

}   // GetVisualComponent


/************************************************************************
*    desc:  Get the object data                                                            
************************************************************************/
const iObjectData & CSprite::GetObjectData() const
{
    return m_objectData;

}   // GetObjectData
