
/************************************************************************
*    FILE NAME:       ojectvisualdata2d.h
*
*    DESCRIPTION:     Class containing the 2D object's visual data
************************************************************************/

#ifndef __object_visual_data_2d_h__
#define __object_visual_data_2d_h__

// SDL/OpenGL lib dependencies
#include <SDL_opengl.h>

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
    GLuint GetTextureID( uint index = 0 ) const;

    // Get the name of the shader ID
    const std::string & GetShaderID() const;

    // Get the color
    const CColor & GetColor() const;

    // Get the vertex scale
    const CPoint<float> & GetVertexScale() const;

    // Get the VBO
    GLuint GetVBO() const;

    // Get the IBO
    GLuint GetIBO() const;

    // Get the vertex count
    int GetVertexCount() const;

    // Get Index count
    int GetIndexCount() const;

    // Get the frame count
    uint GetFrameCount() const;

private:

    // Loaded texture data
    std::vector<GLuint> m_textureIDVec;

    // VBO
    GLuint m_vbo;

    // IBO
    GLuint m_ibo;

    // VBO/IBO generation type
    NDefs::EGenerationType m_genType;

    // Name of the shader
    std::string m_shaderID;

    // Initial color of the object
    CColor m_color;

    // texture file path vector
    std::vector<std::string> m_textureFileVec;

    // Compressed flag
    bool m_compressed;

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
};

#endif  // __object_visual_data_2d_h__
