
/************************************************************************
*    FILE NAME:       objectvisualdata3d.h
*
*    DESCRIPTION:     Class containing the 3D object's visual data
************************************************************************/

#ifndef __object_visual_data_3d_h__
#define __object_visual_data_3d_h__

// Standard lib dependencies
#include <string>
#include <vector>

// Game lib dependencies
#include <common/size.h>
#include <common/color.h>
#include <common/rect.h>
#include <common/point.h>
#include <common/defs.h>
#include <utilities/xmlParser.h>
#include <softwareRender/renderdefs.h>

// Forward Declarations
class CTexture;
struct XMLNode;

class CObjectVisualData3D
{
public:

    // Constructor/Destructor
    CObjectVisualData3D();
    ~CObjectVisualData3D();

    // Load thes object data from node
    void loadFromNode( const XMLNode & objectNode );

    // Create the object from data
    void createFromData( const std::string & group, CSize<int> & rSize );

    // Get the gne type
    NDefs::EGenerationType getGenerationType() const;

    // Get the texture
    const CTexture * getTexture( uint index = 0 ) const;

    // Get the color
    const CColor<float> & getColor() const;

    // Get the vertex scale
    const CPoint<float> & getVertexScale() const;

    // Get the VBO
    uint getVBO() const;

    // Get the IBO
    uint getIBO() const;

    // Get the vertex count
    int getVertexCount() const;

    // Get Index count
    int getIndexCount() const;

    // Get the shader function (nullptr = use default)
    FragmentShaderFunc getShader() const;

private:

    // Loaded texture data
    std::vector<const CTexture *> m_textureVec;

    // VBO
    uint m_vbo;

    // IBO
    uint m_ibo;

    // VBO/IBO generation type
    NDefs::EGenerationType m_genType;

    // Initial color of the object
    CColor<float> m_color;

    // texture file path vector
    std::vector<std::string> m_textureFileVec;

    // mesh file path
    std::string m_meshFile;

    // vertex count
    int m_vertexCount;

    // index count
    int m_indexCount;

    // Quad UV data
    CRect<float> m_uv;

    // The vertex scale of the object
    CPoint<float> m_vertexScale;

    // Fragment shader function pointer (nullptr = use default built-in)
    FragmentShaderFunc m_shader;
};

#endif
