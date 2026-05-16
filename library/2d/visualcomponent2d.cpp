
/************************************************************************
*    FILE NAME:       visualcomponent2d.cpp
*
*    DESCRIPTION:     Class for handling the visual part of the sprite
************************************************************************/

// Physical component dependency
#include <2d/visualcomponent2d.h>

// Game lib dependencies
#include <objectdata/objectdata2d.h>
#include <objectdata/objectvisualdata2d.h>
#include <common/quad2d.h>
#include <utilities/statcounter.h>
#include <softwareRender/softwareRender.h>

/************************************************************************
*    desc:  Constructer
************************************************************************/
CVisualComponent2d::CVisualComponent2d( const CObjectData2D & objectData )
    : m_visualData( objectData.GetVisualData() ),
      m_vertexBufSize( sizeof(CVertex2D) ),
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
CVisualComponent2d::~CVisualComponent2d()
{
}   // destructer


/************************************************************************
*    desc:  do the render
************************************************************************/
void CVisualComponent2d::Render( const CMatrix & matrix )
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

            CSoftwareRender::Instance().Render( finalMatrix, m_vertexCount, m_IndexCount, m_textureID, m_vbo, m_ibo, m_color, m_visualData.IsAlphaBlend() );
        }
    }

}   // Render


/************************************************************************
*    desc:  Is this component active?
************************************************************************/
bool CVisualComponent2d::IsActive()
{
    return (m_visualData.GetGenerationType() != NDefs::EGT_NULL);

}   // IsActive


/************************************************************************
*    desc:  Set/Get the color 
************************************************************************/
void CVisualComponent2d::SetColor( const CColor & color )
{
    m_color = color;

}   // SetColor

const CColor & CVisualComponent2d::GetColor() const
{
    return m_color;

}   // GetColor


/************************************************************************
*    desc:  Set/Get the alpha 
************************************************************************/
void CVisualComponent2d::SetAlpha( float alpha )
{
    m_color.a = alpha;

}   // SetAlpha

float CVisualComponent2d::GetAlpha() const
{
    return m_color.a;

}   // GetAlpha


/************************************************************************
*    desc:  Set the texture ID from index
************************************************************************/
void CVisualComponent2d::SetTextureID( uint index )
{
    m_textureID = m_visualData.GetTextureID( index );

}   // SetTextureID
