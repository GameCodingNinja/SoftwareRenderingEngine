
/************************************************************************
*    FILE NAME:       sprite.h
*
*    DESCRIPTION:     Sprite class
************************************************************************/

#ifndef __sprite_h__
#define __sprite_h__

// Physical component dependency
#include <common/object.h>

// Game lib dependencies
#include <objectdata/iobjectdata.h>

// Standard lib dependencies
#include <memory>

// Forward declaration(s)
class CMatrix;
class CCamera;
class iVisualComponent;

class CSprite : public CObject
{
public:

    // Constructor
    CSprite( const iObjectData & objectData );

    // Destructor
    virtual ~CSprite();

    CSprite(const CSprite&) = delete;
    CSprite& operator=(const CSprite&) = delete;

    // Get the object data
    const iObjectData & GetObjectData() const;

    // Get the visual component
    iVisualComponent * GetVisualComponent();

    // Update the sprite 
    void Update();

    // do the render
    void render( const CCamera & camera );

private:

    // The object data
    const iObjectData & m_objectData;

    // The visual part of the sprite
    std::unique_ptr<iVisualComponent> m_upVisualComponent;

    // The projection matrix
    const CMatrix m_projectionMatrix;

};

#endif
