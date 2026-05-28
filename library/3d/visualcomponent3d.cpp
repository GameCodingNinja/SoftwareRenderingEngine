
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
#include <common/camera.h>

/************************************************************************
*    desc:  Constructer
************************************************************************/
CVisualComponent3d::CVisualComponent3d( const CObjectData3D & objectData )
    : m_visualData( objectData.getVisualData() ),
      m_vertexBufSize( sizeof(CVertex) ),
      m_uvOffset( sizeof(CPoint<float>) ),
      m_pTexture( m_visualData.getTexture() ),
      m_color( m_visualData.getColor() ),
      m_pVBO( m_visualData.getVBO() ),
      m_pIBO( m_visualData.getIBO() ),
      m_vertexCount( m_visualData.getVertexCount() ),
      m_IndexCount( m_visualData.getIndexCount() ),
      m_fixedFunction( m_visualData.getFixedFunction() ),
      m_uniqueVerts( m_visualData.getUniqueVerts() ),
      m_vertToUniqueVec( m_visualData.getVertToUniqueVec() )
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
void CVisualComponent3d::render( const CMatrix & modelMatrix, const CCamera & camera )
{
    if( isActive() )
    {
        // Increment our stat counter to keep track of what is going on.
        CStatCounter::Instance().incDisplayCounter();

        // MVP matrix: vertexScale * model * view * projection
        CMatrix mvpMatrix;
        mvpMatrix.setScale( m_visualData.getVertexScale() ); // Needed for 2d to pre-scale a 2d image to it's pixel size for a 3D quad
        mvpMatrix *= modelMatrix;
        mvpMatrix.mergeMatrix( camera.getFinalMatrix() );

        // World-space model matrix for normal transformation (no view/projection)
        CMatrix worldMatrix;
        worldMatrix.setScale( m_visualData.getVertexScale() ); // Needed for 2d to pre-scale a 2d image to it's pixel size for a 3D quad
        worldMatrix *= modelMatrix;

        if( m_fixedFunction )
            CSoftwareRender::Instance().renderFixedFunction3D( mvpMatrix, *this );
        else
            CSoftwareRender::Instance().render3D( mvpMatrix, worldMatrix, *this );
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


/************************************************************************
*    desc:  Get the texture
************************************************************************/
const CTexture * CVisualComponent3d::getTexture() const
{
    return m_pTexture;
}


/************************************************************************
*    desc:  Get the VBO
************************************************************************/
float * CVisualComponent3d::getVBO() const
{
    return m_pVBO;
}


/************************************************************************
*    desc:  Get the IBO
************************************************************************/
uint * CVisualComponent3d::getIBO() const
{
    return m_pIBO;
}


/************************************************************************
*    desc:  Get the vertex count
************************************************************************/
int CVisualComponent3d::getVertexCount() const
{
    return m_vertexCount;
}


/************************************************************************
*    desc:  Get the index count
************************************************************************/
int CVisualComponent3d::getIndexCount() const
{
    return m_IndexCount;
}


/************************************************************************
*    desc:  Get the unique verts
************************************************************************/
const std::vector<CPoint<float>*> & CVisualComponent3d::getUniqueVerts() const
{
    return m_uniqueVerts;
}


/************************************************************************
*    desc:  Get the vertex to unique position mapping
************************************************************************/
const std::vector<uint> & CVisualComponent3d::getVertToUniqueVec() const
{
    return m_vertToUniqueVec;
}


/************************************************************************
*    desc:  Get the shader function
************************************************************************/
FragmentShaderFunc CVisualComponent3d::getShader() const
{
    return m_visualData.getShader();
}


/************************************************************************
*    desc:  Is fixed function rendering
************************************************************************/
bool CVisualComponent3d::isFixedFunction() const
{
    return m_fixedFunction;
}
