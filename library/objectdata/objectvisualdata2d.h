
/************************************************************************
*    FILE NAME:       ojectvisualdata2d.h
*
*    DESCRIPTION:     Class containing the 2D object's visual data
************************************************************************/

#ifndef __object_visual_data_2d_h__
#define __object_visual_data_2d_h__

// Standard lib dependencies
#include <string>
#include <vector>

// Game lib dependencies
#include <common/size.h>
#include <common/color.h>
#include <common/rect.h>
#include <common/point.h>
#include <common/defs.h>
#include <common/scaledframe.h>
#include <utilities/xmlParser.h>
#include <softwareRender/renderdefs.h>

// Forward Declarations
struct XMLNode;

class CObjectVisualData2D
{
public:

    // Constructor/Destructor
    CObjectVisualData2D();
    ~CObjectVisualData2D();

    // Load thes object data from node
    void LoadFromNode( const XMLNode & objectNode );

    // Create the object from data
    void CreateFromData( const std::string & group, CSize<int> & rSize );

    // Get the gne type
    NDefs::EGenerationType GetGenerationType() const;

    // Get the texture ID
    uint GetTextureID( uint index = 0 ) const;

    // Get the color
    const CColor & GetColor() const;

    // Get the vertex scale
    const CPoint<float> & GetVertexScale() const;

    // Get the VBO
    uint GetVBO() const;

    // Get the IBO
    uint GetIBO() const;

    // Get the vertex count
    int GetVertexCount() const;

    // Get Index count
    int GetIndexCount() const;

    // Get the frame count
    uint GetFrameCount() const;

    // Get the shader function (nullptr = use default)
    FragmentShaderFunc GetShader() const;

private:

    // Loaded texture data
    std::vector<uint> m_textureIDVec;

    // VBO
    uint m_vbo;

    // IBO
    uint m_ibo;

    // VBO/IBO generation type
    NDefs::EGenerationType m_genType;

    // Initial color of the object
    CColor m_color;

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

    // Scaled frame
    CScaledFrame m_scaledFrame;

    // Fragment shader function pointer (nullptr = use default built-in)
    FragmentShaderFunc m_shader;
};

#endif  // __object_visual_data_2d_h__
