
/************************************************************************
*    FILE NAME:       spritescriptcomponent2d.cpp
*
*    DESCRIPTION:     Class for handling the scripting part of the sprite
************************************************************************/

// Physical component dependency
#include <2d/spritescriptcomponent2d.h>

// Standard lib dependencies
#include <assert.h>

// AngelScript lib dependencies
#include <angelscript.h>

// Boost lib dependencies
#include <boost/format.hpp>

// Game lib dependencies
#include <2d/sprite2d.h>
#include <2d/visualcomponent2d.h>
#include <objectdata/objectdata2d.h>
#include <script/scriptmanager.h>
#include <utilities/exceptionhandling.h>
#include <common/color.h>

/************************************************************************
*    desc:  Constructer
************************************************************************/
CSpriteScriptComponent2d::CSpriteScriptComponent2d( CSprite2D & sprite, const CObjectData2D & objectData )
    : m_sprite(sprite),
      m_objectData(objectData),
      m_visualData(m_objectData.GetVisualData()),
      m_visualComponent(sprite.GetVisualComponent())
{
}   // constructor


/************************************************************************
*    desc:  destructer                                                             
************************************************************************/
CSpriteScriptComponent2d::~CSpriteScriptComponent2d()
{
}   // destructer


/************************************************************************
*    desc:  Prepare the script function to run
************************************************************************/
void CSpriteScriptComponent2d::Prepare( const std::string & name )
{
    PrepareScript( name, m_objectData.GetGroup() );

    // Pass the parameters to the script function
    if( GetContext()->SetArgObject(0, this) < 0 )
    {
        throw NExcept::CCriticalException("Error Preparing Script!",
                boost::str( boost::format("There was an error setting the argument the script (%s).\n\n%s\nLine: %s") % name % __FUNCTION__ % __LINE__ ));
    }

}   // Prepare


/************************************************************************
*    desc:  Is this sprite visible?
************************************************************************/
void CSpriteScriptComponent2d::SetVisible( bool visible )
{
    m_sprite.SetVisible( visible );

}   // SetVisible


/************************************************************************
*    desc:  Set the color
************************************************************************/
void CSpriteScriptComponent2d::SetColor( CColor & color )
{
    m_visualComponent.SetColor( color );

}   // SetColor


/************************************************************************
*    desc:  Set the default color
************************************************************************/
void CSpriteScriptComponent2d::SetDefaultColor()
{
    m_visualComponent.SetColor( m_visualData.GetColor() );

}   // SetColor


/************************************************************************
*    desc:  Get the color
************************************************************************/
const CColor & CSpriteScriptComponent2d::GetColor() const
{
    return m_visualComponent.GetColor();

}   // GetColor


/************************************************************************
*    desc:  Get the default color
************************************************************************/
const CColor & CSpriteScriptComponent2d::GetDefaultColor() const
{
    return m_visualData.GetColor();

}   // GetDefaultColor


/************************************************************************
*    desc:  Set the Alpha
************************************************************************/
void CSpriteScriptComponent2d::SetAlpha( float alpha )
{
    if( alpha > 1.5 )
        alpha *= defs_RGB_TO_DEC;

    m_visualComponent.SetAlpha( alpha );

}   // SetAlpha


/************************************************************************
*    desc:  Get the Alpha
************************************************************************/
float CSpriteScriptComponent2d::GetAlpha() const
{
    return m_visualComponent.GetAlpha();

}   // GetAlpha


/************************************************************************
*    desc:  Get the default alpha
************************************************************************/
float CSpriteScriptComponent2d::GetDefaultAlpha() const
{
    return m_visualData.GetColor().GetA();

}   // GetDefaultAlpha


/************************************************************************
*    desc:  Get the frame count
************************************************************************/
uint CSpriteScriptComponent2d::GetFrameCount() const 
{
    return m_visualData.GetFrameCount();
        
}   // GetFrameCount


/************************************************************************
*    desc:  Set the texture ID from index
************************************************************************/
void CSpriteScriptComponent2d::SetFrame( uint index )
{
    m_visualComponent.SetTextureID( index );

}   // SetTextureID


/************************************************************************
*    desc:  Register the class with AngelScript
************************************************************************/
void CSpriteScriptComponent2d::Register( asIScriptEngine * pEngine )
{
    // Register CScriptComponent2d reference and methods
    pEngine->RegisterObjectType(  "CSpriteScript2d", 0, asOBJ_REF|asOBJ_NOCOUNT);
    pEngine->RegisterObjectMethod("CSpriteScript2d", "void SetVisible(bool visible)",    asMETHOD(CSpriteScriptComponent2d, SetVisible), asCALL_THISCALL);
    pEngine->RegisterObjectMethod("CSpriteScript2d", "void SetColor(CColor & in)",       asMETHOD(CSpriteScriptComponent2d, SetColor), asCALL_THISCALL);
    pEngine->RegisterObjectMethod("CSpriteScript2d", "void SetDefaultColor()",           asMETHOD(CSpriteScriptComponent2d, SetDefaultColor), asCALL_THISCALL);
    pEngine->RegisterObjectMethod("CSpriteScript2d", "const CColor & GetColor()",        asMETHOD(CSpriteScriptComponent2d, GetColor), asCALL_THISCALL);
    pEngine->RegisterObjectMethod("CSpriteScript2d", "const CColor & GetDefaultColor()", asMETHOD(CSpriteScriptComponent2d, GetDefaultColor), asCALL_THISCALL);
    pEngine->RegisterObjectMethod("CSpriteScript2d", "uint GetFrameCount()",             asMETHOD(CSpriteScriptComponent2d, GetFrameCount), asCALL_THISCALL);
    pEngine->RegisterObjectMethod("CSpriteScript2d", "void SetFrame(uint index)",        asMETHOD(CSpriteScriptComponent2d, SetFrame), asCALL_THISCALL);
    pEngine->RegisterObjectMethod("CSpriteScript2d", "void SetAlpha(float alpha)",       asMETHOD(CSpriteScriptComponent2d, SetAlpha), asCALL_THISCALL);
    pEngine->RegisterObjectMethod("CSpriteScript2d", "float GetAlpha()",                 asMETHOD(CSpriteScriptComponent2d, GetAlpha), asCALL_THISCALL);
    pEngine->RegisterObjectMethod("CSpriteScript2d", "float GetDefaultAlpha()",          asMETHOD(CSpriteScriptComponent2d, GetDefaultAlpha), asCALL_THISCALL);

}   // Register
