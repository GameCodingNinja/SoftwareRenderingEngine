
/************************************************************************
*    FILE NAME:       visualcomponent3d.cpp
*
*    DESCRIPTION:     Class for handling the visual part of the sprite
************************************************************************/

// Physical component dependency
#include <3d/visualcomponent3d.h>

// Game lib dependencies
#include <objectdata/objectdata3d.h>
#include <objectdata/objectvisualdata3d.h>
#include <common/quad2d.h>
#include <utilities/statcounter.h>
#include <softwareRender/softwareRender.h>

/************************************************************************
*    desc:  Constructer
************************************************************************/
CVisualComponent3d::CVisualComponent3d( const CObjectData3D & objectData )
    : m_visualData( objectData.GetVisualData() ),
      m_vertexBufSize( sizeof(CVertex) ),
      m_uvOffset( sizeof(CPoint<float>) ),
      m_textureID( m_visualData.GetTextureID() ),
      m_color( m_visualData.GetColor() ),
      m_vbo( m_visualData.GetVBO() ),
      m_ibo( m_visualData.GetIBO() ),
      m_vertexCount( m_visualData.GetVertexCount() ),
      m_IndexCount( m_visualData.GetIndexCount() )
{
}   // constructor


/************************************************************************
*    desc:  destructer                                                             
************************************************************************/
CVisualComponent3d::~CVisualComponent3d()
{
}   // destructer


/************************************************************************
*    desc:  do the render
************************************************************************/
void CVisualComponent3d::Render( const CMatrix & matrix )
{
    if( IsActive() )
    {
        // Increment our stat counter to keep track of what is going on.
        CStatCounter::Instance().IncDisplayCounter();

        // If this is a quad, we need to take into account the vertex scale
        if( m_visualData.GetGenerationType() == NDefs::EGT_QUAD )
        {
            // Calculate the final matrix
            CMatrix finalMatrix;
            finalMatrix.Scale( m_visualData.GetVertexScale() );
            finalMatrix *= matrix;

            CSoftwareRender::Instance().Render3D( finalMatrix, m_vertexCount, m_IndexCount, m_textureID, m_vbo, m_ibo, m_color );
        }
    }

}   // Render


/************************************************************************
*    desc:  Is this component active?
************************************************************************/
bool CVisualComponent3d::IsActive()
{
    return (m_visualData.GetGenerationType() != NDefs::EGT_NULL);

}   // IsActive


/************************************************************************
*    desc:  Set/Get the color 
************************************************************************/
void CVisualComponent3d::SetColor( const CColor & color )
{
    m_color = color;

}   // SetColor

const CColor & CVisualComponent3d::GetColor() const
{
    return m_color;

}   // GetColor


/************************************************************************
*    desc:  Set the texture ID from index
************************************************************************/
void CVisualComponent3d::SetTextureID( uint index )
{
    m_textureID = m_visualData.GetTextureID( index );

}   // SetTextureID
