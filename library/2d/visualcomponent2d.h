
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
#include <common/matrix.h>
#include <common/color.h>
#include <common/point.h>
#include <common/defs.h>

// Forward declaration(s)
class CObjectData2D;
class CObjectVisualData2D;

class CVisualComponent2d
{
public:

    // Constructor
    CVisualComponent2d( const CObjectData2D & objectData );

    // Destructor
    ~CVisualComponent2d();

    // do the render
    void Render( const CMatrix & matrix );

    // Is this component active?
    bool IsActive();

    // Set/Get the color
    void SetColor( const CColor & color );
    const CColor & GetColor() const;

    // Set/Get the alpha
    void SetAlpha( float alpha );
    float GetAlpha() const;

    // Set the texture ID from index
    void SetTextureID( uint index );

private:

    // Reference to object visual data
    const CObjectVisualData2D & m_visualData;

    // offset variables
    const int m_vertexBufSize;
    const int m_uvOffset;

    // Loaded texture data
    uint m_textureID;

    // Color
    CColor m_color;

    // VBO
    uint m_vbo;

    // IBO
    uint m_ibo;

    // Vertex count
    int m_vertexCount;

    // Index count
    int m_IndexCount;
};

#endif  // __visual_component_2d_h__
