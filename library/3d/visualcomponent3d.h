
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
    void render( const CMatrix & matrix ) override;

    // Is this component active?
    bool isActive() override;

    // Set/Get the color
    void setColor( const CColor<float> & color ) override;
    const CColor<float> & getColor() const override;

    // Set the texture from index
    void setTexture( uint index ) override;

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
};

#endif
