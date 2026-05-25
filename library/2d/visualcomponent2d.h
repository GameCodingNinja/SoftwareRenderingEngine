
/************************************************************************
*    FILE NAME:       visualcomponent2d.h
*
*    DESCRIPTION:     Class for handling the visual part of the sprite
************************************************************************/

#ifndef __visual_component_2d_h__
#define __visual_component_2d_h__

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
class CObjectData2D;
class CObjectVisualData2D;

class CVisualComponent2d : public iVisualComponent
{
public:

    // Constructor
    CVisualComponent2d( const CObjectData2D & objectData );

    // Destructor
    virtual ~CVisualComponent2d();

    // do the render
    void render( const CMatrix & matrix ) override;

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

    // Get the shader function
    FragmentShaderFunc getShader() const;

    // Get blend alpha flag
    bool getBlendAlpha() const;

    // Is fixed function rendering
    bool isFixedFunction() const;

private:

    // Reference to object visual data
    const CObjectVisualData2D & m_visualData;

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

    // Blend alpha flag
    bool m_blendAlpha;

    // Use fixed-function rendering
    bool m_fixedFunction;
};

#endif
