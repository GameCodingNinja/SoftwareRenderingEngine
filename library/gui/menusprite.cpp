
/************************************************************************
*    FILE NAME:       menusprite.cpp
*
*    DESCRIPTION:     Class template
************************************************************************/

// Physical component dependency
#include <gui/menusprite.h>

// Standard lib dependencies

// Boost lib dependencies

// Game lib dependencies
#include <objectdata/objectdata2d.h>
#include <utilities/xmlParser.h>

/************************************************************************
*    desc:  Constructer
************************************************************************/
CMenuSprite::CMenuSprite( const CObjectData2D & objectData )
    : CSprite2D( objectData )
{
}   // constructor


/************************************************************************
*    desc:  destructer                                                             
************************************************************************/
CMenuSprite::~CMenuSprite()
{
}   // destructer


/************************************************************************
*    desc:  Init the script functions
************************************************************************/
void CMenuSprite::InitScriptFunctions( const XMLNode & node )
{
    // Check for scripting - Add an empty string for scripts not defined
    XMLNode scriptNode = node.getChildNode( "script" );
    if( !scriptNode.isEmpty() )
    {
        XMLNode transNode = scriptNode.getChildNode( "transition" );
        if( !transNode.isEmpty() )
        {
            if( transNode.isAttributeSet( "in" ) )
                m_scriptFunction.insert( std::make_pair(NUIControl::ESSF_TRANS_IN, transNode.getAttribute( "in" )) );

            if( transNode.isAttributeSet( "out" ) )
                m_scriptFunction.insert( std::make_pair(NUIControl::ESSF_TRANS_OUT, transNode.getAttribute( "out" )) );
        }

        XMLNode stateNode = scriptNode.getChildNode( "state" );
        if( !stateNode.isEmpty() )
        {
            if( stateNode.isAttributeSet( "disabled" ) )
                m_scriptFunction.insert( std::make_pair(NUIControl::ESSF_DISABLED, stateNode.getAttribute( "disabled" )) );

            if( stateNode.isAttributeSet( "inactive" ) )
                m_scriptFunction.insert( std::make_pair(NUIControl::ESSF_INACTIVE, stateNode.getAttribute( "inactive" )) );

            if( stateNode.isAttributeSet( "active" ) )
                m_scriptFunction.insert( std::make_pair(NUIControl::ESSF_ACTIVE, stateNode.getAttribute( "active" )) );

            if( stateNode.isAttributeSet( "selected" ) )
                m_scriptFunction.insert( std::make_pair(NUIControl::ESSF_SELECTED, stateNode.getAttribute( "selected" )) );
        }
    }

}   // InitScriptFunctions


/************************************************************************
*    desc:  Prepare the script function to run
************************************************************************/
void CMenuSprite::Prepare( NUIControl::ESpriteScriptFunctions scriptFunc )
{
    auto iter = m_scriptFunction.find(scriptFunc);

    if( iter != m_scriptFunction.end() )
        GetScriptComponent().Prepare(iter->second);

}   // Prepare


