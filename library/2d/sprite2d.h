
/************************************************************************
*    FILE NAME:       sprite2d.h
*
*    DESCRIPTION:     2D sprite class
************************************************************************/

#ifndef __sprite_2d_h__
#define __sprite_2d_h__

// Physical component dependency
#include <common/object.h>

// Game lib dependencies
#include <2d/visualcomponent2d.h>

// Forward declaration(s)
class CObjectData2D;

class CSprite2D : public CObject
{
public:

    // Constructor
    CSprite2D( const CObjectData2D & objectData );

    // Destructor
    virtual ~CSprite2D();

    CSprite2D(const CSprite2D&) = delete;
    CSprite2D& operator=(const CSprite2D&) = delete;

    // Get the object data
    const CObjectData2D & GetObjectData() const;

    // Get the visual component
    CVisualComponent2d & GetVisualComponent();

    // Update the sprite 
    void Update();

    // do the render
    void Render( const CMatrix & matrix );

private:

    // The object data
    const CObjectData2D & m_objectData;

    // The visual part of the 2d sprite
    CVisualComponent2d m_visualComponent;

    // The projection matrix
    const CMatrix m_projectionMatrix;

};

#endif  // __sprite_2d_h__
