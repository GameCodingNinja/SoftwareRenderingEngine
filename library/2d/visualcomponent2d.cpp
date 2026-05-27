
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
#include <common/camera.h>

/************************************************************************
*    desc:  Constructer
************************************************************************/
CVisualComponent2d::CVisualComponent2d( const CObjectData2D & objectData )
    : m_visualData( objectData.getVisualData() ),
      m_vertexBufSize( sizeof(CVertex) ),
      m_uvOffset( sizeof(CPoint<float>) ),
      m_pTexture( m_visualData.getTexture() ),
      m_color( m_visualData.getColor() ),
      m_pVBO( m_visualData.getVBO() ),
      m_pIBO( m_visualData.getIBO() ),
      m_vertexCount( m_visualData.getVertexCount() ),
      m_IndexCount( m_visualData.getIndexCount() ),
      m_blendAlpha( m_visualData.getBlendAlpha() ),
      m_fixedFunction( m_visualData.getFixedFunction() )
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
void CVisualComponent2d::render( const CMatrix & modelMatrix, const CCamera & camera )
{
    if( isActive() )
    {
        // Increment our stat counter to keep track of what is going on.
        CStatCounter::Instance().incDisplayCounter();

        // If this is a quad, we need to take into account the vertex scale
        if( m_visualData.getGenerationType() == NDefs::EGT_QUAD )
        {
            // Calculate the final matrix: vertexScale * model * view * projection
            CMatrix finalMatrix;
            finalMatrix.setScale( m_visualData.getVertexScale() ); // Needed for 2d to pre-scale a 2d image to it's pixel size
            finalMatrix *= modelMatrix;
            finalMatrix.mergeMatrix( camera.getFinalMatrix() );

            if( m_fixedFunction )
                CSoftwareRender::Instance().renderFixedFunction2D( finalMatrix, *this );
            else
                CSoftwareRender::Instance().render2D( finalMatrix, *this );
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


/************************************************************************
*    desc:  Get the texture
************************************************************************/
const CTexture * CVisualComponent2d::getTexture() const
{
    return m_pTexture;
}


/************************************************************************
*    desc:  Get the VBO
************************************************************************/
float * CVisualComponent2d::getVBO() const
{
    return m_pVBO;
}


/************************************************************************
*    desc:  Get the IBO
************************************************************************/
uint * CVisualComponent2d::getIBO() const
{
    return m_pIBO;
}


/************************************************************************
*    desc:  Get the vertex count
************************************************************************/
int CVisualComponent2d::getVertexCount() const
{
    return m_vertexCount;
}


/************************************************************************
*    desc:  Get the index count
************************************************************************/
int CVisualComponent2d::getIndexCount() const
{
    return m_IndexCount;
}


/************************************************************************
*    desc:  Get the shader function
************************************************************************/
FragmentShaderFunc CVisualComponent2d::getShader() const
{
    return m_visualData.getShader();
}


/************************************************************************
*    desc:  Get blend alpha flag
************************************************************************/
bool CVisualComponent2d::getBlendAlpha() const
{
    return m_blendAlpha;
}


/************************************************************************
*    desc:  Is fixed function rendering
************************************************************************/
bool CVisualComponent2d::isFixedFunction() const
{
    return m_fixedFunction;
}
