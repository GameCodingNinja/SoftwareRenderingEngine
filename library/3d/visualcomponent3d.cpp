
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
    : m_visualData( objectData.getVisualData() ),
      m_vertexBufSize( sizeof(CVertex3d) ),
      m_uvOffset( sizeof(CPoint<float>) ),
      m_pTexture( m_visualData.getTexture() ),
      m_color( m_visualData.getColor() ),
      m_pVBO( m_visualData.getVBO() ),
      m_pIBO( m_visualData.getIBO() ),
      m_vertexCount( m_visualData.getVertexCount() ),
      m_IndexCount( m_visualData.getIndexCount() ),
      m_fixedFunction( m_visualData.getFixedFunction() )
{
}

/************************************************************************
*    desc:  destructer                                                             
************************************************************************/
CVisualComponent3d::~CVisualComponent3d()
{
}

/************************************************************************
*    desc:  do the render
************************************************************************/
void CVisualComponent3d::render( const CMatrix & matrix )
{
    if( isActive() )
    {
        // Increment our stat counter to keep track of what is going on.
        CStatCounter::Instance().incDisplayCounter();

        // Apply vertex scale (quads use it for aspect ratio, mesh files default to 1,1,1)
        CMatrix finalMatrix;
        finalMatrix.setScale( m_visualData.getVertexScale() );
        finalMatrix *= matrix;

        if( m_fixedFunction )
                CSoftwareRender::Instance().renderFixedFunction3D( finalMatrix, m_vertexCount, m_IndexCount, m_pTexture, m_pVBO, m_pIBO, m_color );
        else
                CSoftwareRender::Instance().render3D( finalMatrix, m_vertexCount, m_IndexCount, m_pTexture, m_pVBO, m_pIBO, m_color, m_visualData.getShader() );
    }
}

/************************************************************************
*    desc:  Is this component active?
************************************************************************/
bool CVisualComponent3d::isActive()
{
    return (m_visualData.getGenerationType() != NDefs::EGT_NULL);
}

/************************************************************************
*    desc:  Set/Get the color 
************************************************************************/
void CVisualComponent3d::setColor( const CColor<float> & color )
{
    m_color = color;
}

const CColor<float> & CVisualComponent3d::getColor() const
{
    return m_color;
}

/************************************************************************
*    desc:  Set the texture ID from index
************************************************************************/
void CVisualComponent3d::setTexture( uint index )
{
    m_pTexture = m_visualData.getTexture( index );
}
