
/************************************************************************
*    FILE NAME:       objectvisualdata3d.cpp
*
*    DESCRIPTION:     Class containing the 3D object's visual data
************************************************************************/

// Physical component dependency
#include <objectdata/objectvisualdata3d.h>

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
CObjectVisualData3D::CObjectVisualData3D()
    : m_vbo(0),
      m_ibo(0),
      m_genType(NDefs::EGT_NULL),
      m_vertexCount(0),
      m_indexCount(0),
      m_vertexScale(1,1,1),
      m_shader(nullptr)
{
}   // constructor


/************************************************************************
*    desc:  Destructer                                                             
************************************************************************/
CObjectVisualData3D::~CObjectVisualData3D()
{
    // NOTE: Nothing should ever be deleted here
}   // Destructer


/************************************************************************
*    desc:  Load the object data from node
************************************************************************/
void CObjectVisualData3D::LoadFromNode( const XMLNode & objectNode )
{
    const XMLNode visualNode = objectNode.getChildNode( "visual" );

    if( !visualNode.isEmpty() )
    {
        // See if we have a texture list
        const XMLNode textureNode = visualNode.getChildNode("texture");
        if( !textureNode.isEmpty() )
        {
            m_textureFileVec.push_back( textureNode.getAttribute( "file" ) );
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

                else if( genTypeStr == "file" )
                    m_genType = NDefs::EGT_MESH_FILE;
            }

            const XMLNode quadNode = meshNode.getChildNode( "quad" );
            if( !quadNode.isEmpty() )
            {
                m_uv.x1 = std::atof(quadNode.getAttribute( "uv.x1" ));
                m_uv.y1 = std::atof(quadNode.getAttribute( "uv.y1" ));
                m_uv.x2 = std::atof(quadNode.getAttribute( "uv.x2" ));
                m_uv.y2 = std::atof(quadNode.getAttribute( "uv.y2" ));
            }

            const XMLNode fileNode = meshNode.getChildNode( "file" );
            if( !fileNode.isEmpty() )
            {
                m_meshFile = fileNode.getAttribute( "name" );
            }
        }

        // Check for color
        m_color = NParseHelper::LoadColor( visualNode, m_color );

        // Check for a named shader
        const XMLNode shaderNode = visualNode.getChildNode("shader");
        if( !shaderNode.isEmpty() && shaderNode.isAttributeSet("name") )
        {
            m_shader = CShaderMgr::Instance().get( shaderNode.getAttribute("name") );
        }
    }

}   // LoadFromNode


/************************************************************************
*    desc:  Create the object from data
************************************************************************/
void CObjectVisualData3D::CreateFromData( const std::string & group, CSize<int> & rSize )
{
    CTexture texture;

    if( !m_textureFileVec.empty() )
    {
        for( size_t i = 0; i < m_textureFileVec.size(); ++i )
        {
            texture = CTextureMgr::Instance().LoadFor2D( group, m_textureFileVec[i] );
            m_textureIDVec.push_back( texture.GetID() );
        }

        // If the passed in size reference is empty, set it to the texture size
        if( rSize.isEmpty() )
            rSize = texture.GetSize();
    }

    if( m_genType == NDefs::EGT_QUAD )
    {
        uint indexData[] = {0, 1, 2, 0, 2, 3};

        std::string vboName = NGenFunc::FormatString("quad_%g_%g_%g_%g", m_uv.x1, m_uv.y1, m_uv.x2, m_uv.y2);

        m_vbo = CVertBufMgr::Instance().CreateQuadVBO( group, vboName, m_uv );
        m_ibo = CVertBufMgr::Instance().CreateIBO( group, "quad_0123", indexData, sizeof(indexData) );

        // Scale the quad to match the texture's aspect ratio
        // so it doesn't appear stretched
        if( !rSize.isEmpty() )
        {
            float aspect = (float)rSize.w / (float)rSize.h;
            m_vertexScale.x = aspect;
            m_vertexScale.y = 1.0f;
        }

        // A quad has 4 verts
        m_vertexCount = 4;

        // A quad has 6 indexes
        m_indexCount = 6;
    }

}   // CreateFromData


/************************************************************************
*    desc:  Get the gne type
************************************************************************/
NDefs::EGenerationType CObjectVisualData3D::GetGenerationType() const 
{
    return m_genType;
}


/************************************************************************
*    desc:  Get the texture ID
************************************************************************/
uint CObjectVisualData3D::GetTextureID( uint index ) const 
{
    if( m_textureIDVec.empty() )
        return 0;
    else
        return m_textureIDVec[index];
}


/************************************************************************
*    desc:  Get the color
************************************************************************/
const CColor & CObjectVisualData3D::GetColor() const 
{
    return m_color;
}


/************************************************************************
*    desc:  Get the VBO
************************************************************************/
uint CObjectVisualData3D::GetVBO() const 
{
    return m_vbo;
}


/************************************************************************
*    desc:  Get the IBO
************************************************************************/
uint CObjectVisualData3D::GetIBO() const 
{
    return m_ibo;
}


/************************************************************************
*    desc:  Get the vertex count
************************************************************************/
int CObjectVisualData3D::GetVertexCount() const 
{
    return m_vertexCount;
}


/************************************************************************
*    desc:  Get the index count
************************************************************************/
int CObjectVisualData3D::GetIndexCount() const 
{
    return m_indexCount;
}


/************************************************************************
*    desc:  Get the vertex scale
************************************************************************/
const CPoint<float> & CObjectVisualData3D::GetVertexScale() const 
{
    return m_vertexScale;
}

/************************************************************************
*    desc:  Get the shader function
************************************************************************/
FragmentShaderFunc CObjectVisualData3D::GetShader() const
{
    return m_shader;
}
