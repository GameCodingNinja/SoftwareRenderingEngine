
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
    : m_visualData( objectData.getVisualData() ),
      m_vertexBufSize( sizeof(CVertex) ),
      m_uvOffset( sizeof(CPoint<float>) ),
      m_pTexture( m_visualData.getTexture() ),
      m_color( m_visualData.getColor() ),
      m_vbo( m_visualData.getVBO() ),
      m_ibo( m_visualData.getIBO() ),
      m_vertexCount( m_visualData.getVertexCount() ),
      m_IndexCount( m_visualData.getIndexCount() )
{
}


/************************************************************************
*    desc:  destructer                                                             
************************************************************************/
CVisualComponent2d::~CVisualComponent2d()
{
}


/************************************************************************
*    desc:  do the render
************************************************************************/
void CVisualComponent2d::render( const CMatrix & matrix )
{
    if( isActive() )
    {
        // Increment our stat counter to keep track of what is going on.
        CStatCounter::Instance().incDisplayCounter();

        // If this is a quad, we need to take into account the vertex scale
        if( m_visualData.getGenerationType() == NDefs::EGT_QUAD )
        {
            // Calculate the final matrix
            CMatrix finalMatrix;
            finalMatrix.setScale( m_visualData.getVertexScale() );
            finalMatrix *= matrix;

            CSoftwareRender::Instance().render2D( finalMatrix, m_vertexCount, m_IndexCount, m_pTexture, m_vbo, m_ibo, m_color, m_visualData.getShader() );
        }
    }

}


/************************************************************************
*    desc:  Is this component active?
************************************************************************/
bool CVisualComponent2d::isActive()
{
    return (m_visualData.getGenerationType() != NDefs::EGT_NULL);

}


/************************************************************************
*    desc:  Set/Get the color 
************************************************************************/
void CVisualComponent2d::setColor( const CColor<float> & color )
{
    m_color = color;

}

const CColor<float> & CVisualComponent2d::getColor() const
{
    return m_color;

}


/************************************************************************
*    desc:  Set the texture ID from index
************************************************************************/
void CVisualComponent2d::setTexture( uint index )
{
    m_pTexture = m_visualData.getTexture( index );

}
