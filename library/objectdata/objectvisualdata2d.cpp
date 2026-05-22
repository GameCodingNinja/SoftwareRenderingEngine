
/************************************************************************
*    FILE NAME:       ojectvisualdata2d.cpp
*
*    DESCRIPTION:     Class containing the 3D object's visual data
************************************************************************/

// Physical component dependency
#include <objectdata/objectvisualdata2d.h>

// Game lib dependencies
#include <managers/texturemanager.h>
#include <managers/vertexbuffermanager.h>
#include <utilities/xmlparsehelper.h>
#include <utilities/exceptionhandling.h>
#include <utilities/genfunc.h>
#include <common/defs.h>
#include <managers/shadermanager.h>

/************************************************************************
*    desc:  Constructer
************************************************************************/
CObjectVisualData2D::CObjectVisualData2D()
    : m_vbo(0),
      m_ibo(0),
      m_genType(NDefs::EGT_NULL),
      m_vertexCount(0),
      m_indexCount(0),
      m_vertexScale(1,1,1),
      m_shader(nullptr)
{
}


/************************************************************************
*    desc:  Destructer                                                             
************************************************************************/
CObjectVisualData2D::~CObjectVisualData2D()
{
    // NOTE: Nothing should ever be deleted here
}


/************************************************************************
*    desc:  Load the object data from node
************************************************************************/
void CObjectVisualData2D::loadFromNode( const XMLNode & objectNode )
{
    const XMLNode visualNode = objectNode.getChildNode( "visual" );

    if( !visualNode.isEmpty() )
    {
        // See if we have a texture list
        const XMLNode textureNode = visualNode.getChildNode("texture");
        if( !textureNode.isEmpty() )
        {
            if( textureNode.isAttributeSet("count") )
            {
                const uint count = std::atoi( textureNode.getAttribute( "count" ) );
                const std::string file( textureNode.getAttribute( "file" ) );

                m_textureFileVec.reserve( count );

                for( uint i = 0; i < count; ++i )
                    m_textureFileVec.push_back( NGenFunc::FormatString(file.c_str(), i) );
            }
            else
            {
                m_textureFileVec.push_back( textureNode.getAttribute( "file" ) );
            }

        }

        // Get the mesh node
        const XMLNode meshNode = visualNode.getChildNode( "mesh" );
        if( !meshNode.isEmpty() )
        {
            if( meshNode.isAttributeSet("genType") )
            {
                std::string genTypeStr = meshNode.getAttribute( "genType" );

                if( genTypeStr == "quad" )
                    m_genType = NDefs::EGT_QUAD;

                else if( genTypeStr == "scaled_frame" )
                    m_genType = NDefs::EGT_SCALED_FRAME;

                else if( genTypeStr == "file" )
                    m_genType = NDefs::EGT_MESH_FILE;

                else if( genTypeStr == "font" )
                    m_genType = NDefs::EGT_FONT;
            }

            const XMLNode quadNode = meshNode.getChildNode( "quad" );
            if( !quadNode.isEmpty() )
            {
                m_uv.x1 = std::atof(quadNode.getAttribute( "uv.x1" ));
                m_uv.y1 = std::atof(quadNode.getAttribute( "uv.y1" ));
                m_uv.x2 = std::atof(quadNode.getAttribute( "uv.x2" ));
                m_uv.y2 = std::atof(quadNode.getAttribute( "uv.y2" ));
            }

            const XMLNode scaledFrameNode = meshNode.getChildNode( "scaledFrame" );
            if( !scaledFrameNode.isEmpty() )
            {
                m_scaledFrame.m_frame.w = std::atof(scaledFrameNode.getAttribute( "thicknessWidth" ));
                m_scaledFrame.m_frame.h = std::atof(scaledFrameNode.getAttribute( "thicknessHeight" ));

                if( scaledFrameNode.isAttributeSet("centerQuad") )
                    m_scaledFrame.m_centerQuad = (std::string(scaledFrameNode.getAttribute( "centerQuad" )) != "false");
            }

            const XMLNode fileNode = meshNode.getChildNode( "file" );
            if( !fileNode.isEmpty() )
            {
                m_meshFile = fileNode.getAttribute( "name" );
            }
        }

        // Check for color
        m_color = NParseHelper::loadColor( visualNode, m_color );

        // Check for a named shader
        const XMLNode shaderNode = visualNode.getChildNode("shader");
        if( !shaderNode.isEmpty() && shaderNode.isAttributeSet("name") )
        {
            m_shader = CShaderMgr::Instance().get( shaderNode.getAttribute("name") );
        }
    }

}


/************************************************************************
*    desc:  Create the object from data
************************************************************************/
void CObjectVisualData2D::createFromData( const std::string & group, CSize<int> & rSize )
{
    if( !m_textureFileVec.empty() )
    {
        for( size_t i = 0; i < m_textureFileVec.size(); ++i )
        {
            const CTexture & texture = CTextureMgr::Instance().load( group, m_textureFileVec[i] );
            m_textureVec.push_back( &texture );
        }

        // If the passed in size reference is empty, set it to the texture size
        if( rSize.isEmpty() )
            rSize = m_textureVec.back()->getSize();
    }

    if( m_genType == NDefs::EGT_QUAD )
    {
        uint indexData[] = {0, 1, 2, 0, 2, 3};

        std::string vboName = NGenFunc::FormatString("quad_%g_%g_%g_%g", m_uv.x1, m_uv.y1, m_uv.x2, m_uv.y2);

        m_vbo = CVertBufMgr::Instance().createQuadVBO( group, vboName, m_uv );
        m_ibo = CVertBufMgr::Instance().createIBO( group, "quad_0123", indexData, sizeof(indexData) );

        // For this generation type, the image size is the default scale
        m_vertexScale.x = rSize.w;
        m_vertexScale.y = rSize.h;

        // A quad has 4 verts
        m_vertexCount = 4;

        // A quad has 6 indexes
        m_indexCount = 6;
    }
    else if( m_genType == NDefs::EGT_SCALED_FRAME )
    {
        const CTexture * pLastTex = m_textureVec.back();
        std::string vboName = NGenFunc::FormatString("scaled_frame_%d_%d_%d_%d_%d_%d", (int)rSize.w, (int)rSize.h, (int)m_scaledFrame.m_frame.w, (int)m_scaledFrame.m_frame.h, (int)pLastTex->m_size.w, (int)pLastTex->m_size.h);

        m_vbo = CVertBufMgr::Instance().createScaledFrame(
            group, vboName,m_scaledFrame, pLastTex->getSize(), rSize );

        uint indexData[] = {0,1,2,     0,3,1,
                               2,4,5,     2,1,4,
                               1,6,4,     1,7,6,
                               7,8,6,     7,9,8,
                               10,9,7,    10,11,9,
                               12,11,10,  12,13,11,
                               14,10,3,   14,12,10,
                               15,3,0,    15,14,3,
                               3,7,1,     3,10,7};

        // Create the reusable IBO buffer
        m_ibo = CVertBufMgr::Instance().createIBO( group, "scaled_frame", indexData, sizeof(indexData) );

        // Set the vert count depending on the number of quads being rendered
        // If the center quad is not used, just adjust the vertex count because
        // the center quat is just reused verts anyways and is that last 6 in the IBO
        m_vertexCount = 16;
        m_indexCount = 6 * 8;
        if( m_scaledFrame.m_centerQuad )
            m_indexCount += 6;
    }

}


/************************************************************************
*    desc:  Get the gne type
************************************************************************/
NDefs::EGenerationType CObjectVisualData2D::getGenerationType() const 
{
    return m_genType;
}


/************************************************************************
*    desc:  Get the texture ID
************************************************************************/
const CTexture * CObjectVisualData2D::getTexture( uint index ) const 
{
    if( m_textureVec.empty() )
        return nullptr;
    else
        return m_textureVec[index];
}


/************************************************************************
*    desc:  Get the color
************************************************************************/
const CColor<float> & CObjectVisualData2D::getColor() const 
{
    return m_color;
}


/************************************************************************
*    desc:  Get the VBO
************************************************************************/
uint CObjectVisualData2D::getVBO() const 
{
    return m_vbo;
}


/************************************************************************
*    desc:  Get the IBO
************************************************************************/
uint CObjectVisualData2D::getIBO() const 
{
    return m_ibo;
}


/************************************************************************
*    desc:  Get the vertex count
************************************************************************/
int CObjectVisualData2D::getVertexCount() const 
{
    return m_vertexCount;
}


/************************************************************************
*    desc:  Get the index count
************************************************************************/
int CObjectVisualData2D::getIndexCount() const 
{
    return m_indexCount;
}


/************************************************************************
*    desc:  Get the frame count
************************************************************************/
uint CObjectVisualData2D::getFrameCount() const 
{
    return m_textureVec.size();
}


/************************************************************************
*    desc:  Get the vertex scale
************************************************************************/
const CPoint<float> & CObjectVisualData2D::getVertexScale() const 
{
    return m_vertexScale;
}


/************************************************************************
*    desc:  Get the shader function
************************************************************************/
FragmentShaderFunc CObjectVisualData2D::getShader() const
{
    return m_shader;
}

