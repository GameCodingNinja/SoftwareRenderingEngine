
/************************************************************************
*    FILE NAME:       spritescriptcomponent2d.h
*
*    DESCRIPTION:     Class for handling the scripting part of the sprite
************************************************************************/

#ifndef __sprite_script_component_2d_h__
#define __sprite_script_component_2d_h__

// Physical component dependency
#include <script/scriptcomponent.h>

// Standard lib dependencies
#include <string>
#include <vector>

// Game lib dependencies
#include <common/defs.h>

// Forward declaration(s)
class CObjectData2D;
class CObjectVisualData2D;
class CSprite2D;
class CVisualComponent2d;
class CColor;
class asIScriptEngine;

class CSpriteScriptComponent2d : public CScriptComponent
{
public:

    // Constructor
    CSpriteScriptComponent2d( CSprite2D & sprite, const CObjectData2D & objectData );

    // Destructor
    ~CSpriteScriptComponent2d();

    // Prepare the script function to run
    void Prepare( const std::string & name );

    //////////////////////////////////////////////////////
    // Register CScriptComponent2d Object with AngelScript
    //////////////////////////////////////////////////////
    static void Register( asIScriptEngine * pEngine );

    //////////////////////////////////
    // Functions accessed via script
    //////////////////////////////////

    // Is this sprite visible
    void SetVisible( bool visible );

    // Set/Get the color
    void SetColor( CColor & color );
    void SetDefaultColor();
    const CColor & GetColor() const;
    const CColor & GetDefaultColor() const;

    // Set/Get the alpha
    void SetAlpha( float alpha );
    float GetAlpha() const;
    float GetDefaultAlpha() const;

    // Get the frame count
    uint GetFrameCount() const;

    // Set the texture ID from index
    void SetFrame( uint index );

private:

    // The sprite the script will control
    CSprite2D & m_sprite;

    // Local reference of object data
    const CObjectData2D & m_objectData;

    // Local reference of object data
    const CObjectVisualData2D & m_visualData;

    // The visual component the script will control
    CVisualComponent2d & m_visualComponent;

};

#endif  // __sprite_script_component_2d_h__


