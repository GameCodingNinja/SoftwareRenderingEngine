
/************************************************************************
*    FILE NAME:       visualcomponent3d.h
*
*    DESCRIPTION:     Class for handling the visual part of the sprite
************************************************************************/

#ifndef __visual_component_3d_h__
#define __visual_component_3d_h__

// Standard lib dependencies
#include <string>

// Game lib dependencies
#include <common/ivisualcomponent.h>
#include <common/matrix.h>
#include <common/color.h>
#include <common/point.h>
#include <common/defs.h>
#include <softwareRender/renderdefs.h>

// Forward declaration(s)
class CTexture;
class CObjectData3D;
class CObjectVisualData3D;

class CVisualComponent3d : public iVisualComponent
{
public:

    // Constructor
    CVisualComponent3d( const CObjectData3D & objectData );

    // Destructor
    virtual ~CVisualComponent3d();

    // do the render
    void render( const CMatrix & modelMatrix, const CCamera & camera ) override;

    // Is this component active?
    bool isActive() override;

    // Set/Get the color
    void setColor( const CColor<float> & color ) override;
    const CColor<float> & getColor() const override;

    // Set the texture from index
    void setTexture( uint index ) override;

    // Get the texture
    const CTexture * getTexture() const;

    // Get the VBO
    float * getVBO() const;

    // Get the IBO
    uint * getIBO() const;

    // Get the vertex count
    int getVertexCount() const;

    // Get the index count
    int getIndexCount() const;

    // Get the unique verts
    const std::vector<CPoint<float>*> & getUniqueVerts() const;

    // Get the vertex to unique position mapping
    const std::vector<uint> & getVertToUniqueVec() const;

    // Get the shader function
    FragmentShaderFunc getShader() const;

    // Is fixed function rendering
    bool isFixedFunction() const;

    // Set the active lights
    void setLights( const std::vector<CLight> & lights );

    // Get the active lights
    const std::vector<CLight> * getLights() const;

private:

    // Reference to object visual data
    const CObjectVisualData3D & m_visualData;

    // offset variables
    const int m_vertexBufSize;
    const int m_uvOffset;

    // Loaded texture data
    const CTexture * m_pTexture;

    // Color
    CColor<float> m_color;

    // VBO
    float * m_pVBO;

    // IBO
    uint * m_pIBO;

    // Vertex count
    int m_vertexCount;

    // Index count
    int m_IndexCount;

    // Use fixed-function rendering
    bool m_fixedFunction;

    // Unique vertex data for optimized transforms
    const std::vector<CPoint<float>*> & m_uniqueVerts;
    const std::vector<uint> & m_vertToUniqueVec;

    // Active lights (nullptr = use renderer defaults)
    const std::vector<CLight> * m_pLights;
};

#endif
