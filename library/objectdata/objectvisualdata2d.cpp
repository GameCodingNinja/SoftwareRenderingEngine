
/************************************************************************
*    FILE NAME:       ojectvisualdata2d.cpp
*
*    DESCRIPTION:     Class containing the 3D object's visual data
************************************************************************/

// Glew dependencies
#include <GL/glew.h>

// Physical component dependency
#include <objectdata/objectvisualdata2d.h>

// Boost lib dependencies
#include <boost/format.hpp>

// Game lib dependencies
#include <managers/texturemanager.h>
#include <managers/vertexbuffermanager.h>
#include <utilities/xmlparsehelper.h>
#include <utilities/exceptionhandling.h>
#include <common/defs.h>

/************************************************************************
*    desc:  Constructer
************************************************************************/
CObjectVisualData2D::CObjectVisualData2D()
    : m_vbo(0),
      m_ibo(0),
      m_genType(NDefs::EGT_NULL),
      m_compressed(false),
      m_vertexCount(0),
      m_indexCount(0),
      m_vertexScale(1,1,1)
{
}   // constructor


/************************************************************************
*    desc:  Destructer                                                             
************************************************************************/
CObjectVisualData2D::~CObjectVisualData2D()
{
    // NOTE: Nothing should ever be deleted here
}   // Destructer


/************************************************************************
*    desc:  Load the object data from node
************************************************************************/
void CObjectVisualData2D::LoadFromNode( const XMLNode & objectNode )
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
                    m_textureFileVec.push_back( boost::str( boost::format(file) % i ) );
            }
            else
            {
                m_textureFileVec.push_back( textureNode.getAttribute( "file" ) );
            }

            // Is this a compressed texture?
            if( textureNode.isAttributeSet("compressed") )
                m_compressed = (std::string(textureNode.getAttribute( "compressed" )) == "true");
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
        m_color = NParseHelper::LoadColor( visualNode, m_color );

        // The shader node determines which shader to use
        const XMLNode shaderNode = visualNode.getChildNode( "shader" );
        if( !shaderNode.isEmpty() )
        {
            m_shaderID = shaderNode.getAttribute( "id" );
        }

        // Raise an exception if there's a vbo but no shader
        if( (m_genType != NDefs::EGT_NULL) && m_shaderID.empty() )
        {
            throw NExcept::CCriticalException("Shader effect or techique not set!",
                boost::str( boost::format("Shader object data missing.\n\n%s\nLine: %s") % __FUNCTION__ % __LINE__ ));
        }
    }

}   // LoadFromNode


/************************************************************************
*    desc:  Create the object from data
************************************************************************/
void CObjectVisualData2D::CreateFromData( const std::string & group, CSize<int> & rSize )
{
    CTexture texture;

    if( !m_textureFileVec.empty() )
    {
        for( size_t i = 0; i < m_textureFileVec.size(); ++i )
        {
            texture = CTextureMgr::Instance().LoadFor2D( group, m_textureFileVec[i], m_compressed );
            m_textureIDVec.push_back( texture.GetID() );
        }

        // If the passed in size reference is empty, set it to the texture size
        if( rSize.IsEmpty() )
            rSize = texture.GetSize();
    }

    if( m_genType == NDefs::EGT_QUAD )
    {
        uint indexData[] = {0, 1, 2, 0, 2, 3};

        std::string vboName = boost::str( boost::format("quad_%s_%s_%s_%s") % m_uv.x1 % m_uv.y1 % m_uv.x2 % m_uv.y2 );

        m_vbo = CVertBufMgr::Instance().CreateQuadVBO( group, vboName, m_uv );
        m_ibo = CVertBufMgr::Instance().CreateIBO( group, "quad_0123", indexData, sizeof(indexData) );

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
        std::string vboName = boost::str( boost::format("scaled_frame_%d_%d_%d_%d_%d_%d") 
            % rSize.w % rSize.h % m_scaledFrame.m_frame.w % m_scaledFrame.m_frame.h % texture.m_size.w % texture.m_size.h );

        m_vbo = CVertBufMgr::Instance().CreateScaledFrame(
            group, vboName,m_scaledFrame, texture.GetSize(), rSize );

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
        m_ibo = CVertBufMgr::Instance().CreateIBO( group, "scaled_frame", indexData, sizeof(indexData) );

        // Set the vert count depending on the number of quads being rendered
        // If the center quad is not used, just adjust the vertex count because
        // the center quat is just reused verts anyways and is that last 6 in the IBO
        m_vertexCount = 16;
        m_indexCount = 6 * 8;
        if( m_scaledFrame.m_centerQuad )
            m_indexCount += 6;
    }

}   // CreateFromData


/************************************************************************
*    desc:  Get the gne type
************************************************************************/
NDefs::EGenerationType CObjectVisualData2D::GetGenerationType() const 
{
    return m_genType;
}


/************************************************************************
*    desc:  Get the texture ID
************************************************************************/
GLuint CObjectVisualData2D::GetTextureID( uint index ) const 
{
    if( m_textureIDVec.empty() )
        return 0;
    else
        return m_textureIDVec[index];
}


/************************************************************************
*    desc:  Get the name of the shader ID
************************************************************************/
const std::string & CObjectVisualData2D::GetShaderID() const
{
    return m_shaderID;
}


/************************************************************************
*    desc:  Get the color
************************************************************************/
const CColor & CObjectVisualData2D::GetColor() const 
{
    return m_color;
}


/************************************************************************
*    desc:  Get the VBO
************************************************************************/
GLuint CObjectVisualData2D::GetVBO() const 
{
    return m_vbo;
}


/************************************************************************
*    desc:  Get the IBO
************************************************************************/
GLuint CObjectVisualData2D::GetIBO() const 
{
    return m_ibo;
}


/************************************************************************
*    desc:  Get the vertex count
************************************************************************/
int CObjectVisualData2D::GetVertexCount() const 
{
    return m_vertexCount;
}


/************************************************************************
*    desc:  Get the index count
************************************************************************/
int CObjectVisualData2D::GetIndexCount() const 
{
    return m_indexCount;
}


/************************************************************************
*    desc:  Get the frame count
************************************************************************/
uint CObjectVisualData2D::GetFrameCount() const 
{
    return m_textureIDVec.size();
}


/************************************************************************
*    desc:  Get the vertex scale
************************************************************************/
const CPoint<float> & CObjectVisualData2D::GetVertexScale() const 
{
    return m_vertexScale;
}

