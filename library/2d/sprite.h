
/************************************************************************
*    FILE NAME:       sprite.h
*
*    DESCRIPTION:     2D sprite class
************************************************************************/

#ifndef __sprite_h__
#define __sprite_h__

// Physical component dependency
#include <common/object.h>

// Game lib dependencies
#include <2d/visualcomponent2d.h>

// Forward declaration(s)
class CObjectData2D;

class CSprite : public CObject
{
public:

    // Constructor
    CSprite( const CObjectData2D & objectData );

    // Destructor
    virtual ~CSprite();

    CSprite(const CSprite&) = delete;
    CSprite& operator=(const CSprite&) = delete;

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

#endif  // __sprite_h__
