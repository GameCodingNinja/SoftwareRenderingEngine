
/************************************************************************
*    FILE NAME:       objectvisualdata3d.cpp
*
*    DESCRIPTION:     Class containing the 3D object's visual data
************************************************************************/

// Physical component dependency
#include <objectdata/objectvisualdata3d.h>

// Game lib dependencies
#include <cstring>
#include <managers/texturemanager.h>
#include <managers/vertexbuffermanager.h>
#include <utilities/xmlparsehelper.h>
#include <utilities/exceptionhandling.h>
#include <utilities/genfunc.h>
#include <utilities/objmeshloader.h>
#include <common/defs.h>
#include <managers/shadermanager.h>

/************************************************************************
*    desc:  Constructer
************************************************************************/
CObjectVisualData3D::CObjectVisualData3D()
    : m_pVBO(nullptr),
      m_pIBO(nullptr),
      m_genType(NDefs::EGT_NULL),
      m_vertexCount(0),
      m_indexCount(0),
      m_vertexScale(1,1,1),
      m_shader(nullptr),
      m_blendAlpha(false),
      m_fixedFunction(false)
{
}


/************************************************************************
*    desc:  Destructer                                                             
************************************************************************/
CObjectVisualData3D::~CObjectVisualData3D()
{
    // NOTE: Nothing should ever be deleted here
}


/************************************************************************
*    desc:  Load the object data from node
************************************************************************/
void CObjectVisualData3D::loadFromNode( const XMLNode & objectNode )
{
    const XMLNode visualNode = objectNode.getChildNode( "visual" );

    if( !visualNode.isEmpty() )
    {
        // See if we have a texture list
        const XMLNode textureNode = visualNode.getChildNode("texture");
        if( !textureNode.isEmpty() )
        {
            // Clear any info from the default values
            m_textureFileVec.clear();

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

                else if( genTypeStr == "mesh" )
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

            const XMLNode fileNode = meshNode.getChildNode( "mesh" );
            if( !fileNode.isEmpty() )
            {
                m_meshFile = fileNode.getAttribute( "file" );
            }

            // Shader defined inside mesh node
            const XMLNode meshShaderNode = meshNode.getChildNode( "shader" );
            if( !meshShaderNode.isEmpty() && meshShaderNode.isAttributeSet("name") )
            {
                m_shader = CShaderMgr::Instance().get( meshShaderNode.getAttribute("name") );
            }
        }

        // Check for color
        m_color = NParseHelper::loadColor( visualNode, m_color );

        // Check for a named shader on the visual node (for quads)
        const XMLNode shaderNode = visualNode.getChildNode("shader");
        if( !shaderNode.isEmpty() && shaderNode.isAttributeSet("name") )
        {
            m_shader = CShaderMgr::Instance().get( shaderNode.getAttribute("name") );
        }

        // Check for fixed function rendering
        const XMLNode fixedFuncNode = visualNode.getChildNode("fixedFunction");
        if( !fixedFuncNode.isEmpty() )
        {
            if( fixedFuncNode.isAttributeSet("enable") )
                m_fixedFunction = (std::string(fixedFuncNode.getAttribute("enable")) == "true");

            if( fixedFuncNode.isAttributeSet("blendAlpha") )
                m_blendAlpha = (std::string(fixedFuncNode.getAttribute("blendAlpha")) == "true");
        }
    }
}


/************************************************************************
*    desc:  Create the object from data
************************************************************************/
void CObjectVisualData3D::createFromData( const std::string & group, CSize<int> & rSize )
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

        m_pVBO = CVertBufMgr::Instance().createQuadVBO( group, vboName, m_uv );
        m_pIBO = CVertBufMgr::Instance().createIBO( group, "quad_0123", indexData, sizeof(indexData) );

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
    else if( m_genType == NDefs::EGT_MESH_FILE && !m_meshFile.empty() )
    {
        // Load the OBJ mesh file
        CObjMeshLoader meshLoader;
        meshLoader.load( m_meshFile );

        m_vertexCount = static_cast<int>(meshLoader.m_vertices.size());
        m_indexCount = static_cast<int>(meshLoader.m_indices.size());

        // Store VBO/IBO in the vertex buffer manager using the file path as the name
        m_pVBO = CVertBufMgr::Instance().createMeshVBO( group, m_meshFile,
            (float *)meshLoader.m_vertices.data(), sizeof(CVertex) * m_vertexCount );

        m_pIBO = CVertBufMgr::Instance().createIBO( group, m_meshFile,
            meshLoader.m_indices.data(), sizeof(uint) * m_indexCount );

        // Build unique vertex position pointers into the VBO
        m_vertToUniqueVec = std::move( meshLoader.m_vertToUniqueVec );

        CVertex * pVertBase = (CVertex *)m_pVBO;
        m_uniqueVerts.resize( meshLoader.m_uniqueVerts.size() );

        // For each unique position, find the first vertex that maps to it 
        // and point into the VBO copy
        for( uint i = 0; i < m_vertToUniqueVec.size(); ++i )
        {
            uint uniqueIdx = m_vertToUniqueVec[i];
            // Only set the pointer once (first vertex with this unique index)
            if( m_uniqueVerts[uniqueIdx] == nullptr )
                m_uniqueVerts[uniqueIdx] = &pVertBase[i].vert;
        }
    }

}


/************************************************************************
*    desc:  Get the gne type
************************************************************************/
NDefs::EGenerationType CObjectVisualData3D::getGenerationType() const 
{
    return m_genType;
}


/************************************************************************
*    desc:  Get the texture ID
************************************************************************/
const CTexture * CObjectVisualData3D::getTexture( uint index ) const 
{
    if( m_textureVec.empty() )
        return nullptr;
    else
        return m_textureVec[index];
}


/************************************************************************
*    desc:  Get the color
************************************************************************/
const CColor<float> & CObjectVisualData3D::getColor() const 
{
    return m_color;
}


/************************************************************************
*    desc:  Get the VBO
************************************************************************/
float * CObjectVisualData3D::getVBO() const 
{
    return m_pVBO;
}


/************************************************************************
*    desc:  Get the IBO
************************************************************************/
uint * CObjectVisualData3D::getIBO() const 
{
    return m_pIBO;
}


/************************************************************************
*    desc:  Get the vertex count
************************************************************************/
int CObjectVisualData3D::getVertexCount() const 
{
    return m_vertexCount;
}


/************************************************************************
*    desc:  Get the index count
************************************************************************/
int CObjectVisualData3D::getIndexCount() const 
{
    return m_indexCount;
}


/************************************************************************
*    desc:  Get the vertex scale
************************************************************************/
const CPoint<float> & CObjectVisualData3D::getVertexScale() const 
{
    return m_vertexScale;
}

/************************************************************************
*    desc:  Get the shader function
************************************************************************/
FragmentShaderFunc CObjectVisualData3D::getShader() const
{
    return m_shader;
}


/************************************************************************
*    desc:  Get the blend alpha flag
************************************************************************/
bool CObjectVisualData3D::getBlendAlpha() const
{
    return m_blendAlpha;
}


/************************************************************************
*    desc:  Get the fixed function flag
************************************************************************/
bool CObjectVisualData3D::getFixedFunction() const
{
    return m_fixedFunction;
}


/************************************************************************
*    desc:  Get the unique verts
************************************************************************/
const std::vector<CPoint<float>*> & CObjectVisualData3D::getUniqueVerts() const
{
    return m_uniqueVerts;
}


/************************************************************************
*    desc:  Get the vertex to unique position mapping
************************************************************************/
const std::vector<uint> & CObjectVisualData3D::getVertToUniqueVec() const
{
    return m_vertToUniqueVec;
}
