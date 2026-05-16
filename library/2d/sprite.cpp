
/************************************************************************
*    FILE NAME:       sprite.cpp
*
*    DESCRIPTION:     2D sprite class
************************************************************************/

// Physical component dependency
#include <2d/sprite.h>

// Game lib dependencies
#include <objectdata/objectdata2d.h>

/************************************************************************
*    desc:  Constructer
************************************************************************/
CSprite::CSprite( const CObjectData2D & objectData )
    : m_objectData( objectData ),
      m_visualComponent( objectData )
{
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
    if( m_visible )
        m_visualComponent.Render( m_trans_matrix * matrix );

}	// Render


/************************************************************************
*    desc:  Get the visual component                                                            
************************************************************************/
CVisualComponent2d & CSprite::GetVisualComponent()
{
    return m_visualComponent;

}   // GetVisualComponent


/************************************************************************
*    desc:  Get the object data                                                            
************************************************************************/
const CObjectData2D & CSprite::GetObjectData() const
{
    return m_objectData;

}   // GetObjectData
