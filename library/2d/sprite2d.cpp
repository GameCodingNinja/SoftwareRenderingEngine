
/************************************************************************
*    FILE NAME:       sprite2d.cpp
*
*    DESCRIPTION:     2D sprite class
************************************************************************/

// Physical component dependency
#include <2d/sprite2d.h>

// Game lib dependencies
#include <objectdata/objectdata2d.h>

/************************************************************************
*    desc:  Constructer
************************************************************************/
CSprite2D::CSprite2D( const CObjectData2D & objectData )
    : m_objectData( objectData ),
      m_visualComponent( objectData )
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
*    desc:  Get the object data                                                            
************************************************************************/
const CObjectData2D & CSprite2D::GetObjectData() const
{
    return m_objectData;

}   // GetObjectData
