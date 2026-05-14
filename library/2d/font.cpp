/************************************************************************
*    FILE NAME:       font.cpp
*
*    DESCRIPTION:     font class
************************************************************************/

// Physical component dependency
#include <2d/font.h>

// Boost lib dependencies
#include <boost/format.hpp>

// Game lib dependencies
#include <utilities/xmlParser.h>
#include <utilities/exceptionhandling.h>
#include <managers/texturemanager.h>
#include <common/texture.h>

/************************************************************************
*    desc:  Constructer                                                             
************************************************************************/
CFont::CFont()
    : m_lineHeight(0),
      m_baselineOffset(0),
      m_horzPadding(0),
      m_vertPadding(0)
{
}   // Constructer


/************************************************************************
*    desc:  Destructer                                                             
************************************************************************/
CFont::~CFont()
{
}   // Destructer


/************************************************************************
*    desc:  Load from XML file
*
*    param: String filePath - path to xml file
************************************************************************/
bool CFont::LoadFromXML( const std::string & group, const std::string & filePath )
{
    // load the texture - force power of two on this texture
    m_texture = CTextureMgr::Instance().LoadFor2D( group, filePath + ".png" );

    // open this file and parse
    XMLNode mainNode = XMLNode::openFileHelper( (filePath + ".fnt").c_str(), "font" );

    // Get the padding
    std::string padding = mainNode.getChildNode( "info" ).getAttribute("padding");
    m_horzPadding = atof(padding.substr(6,1).c_str());
    m_vertPadding = atof(padding.substr(0,1).c_str());

    // Get the common font info
    XMLNode commonNode = mainNode.getChildNode( "common" );

    // get the line height
    m_lineHeight = atof(commonNode.getAttribute("lineHeight"));

    // get the baseline offset
    m_baselineOffset = atof(commonNode.getAttribute("base"));

    // Get the list of character info
    XMLNode charLstNode = mainNode.getChildNode( "chars" );

    // Load in the individual character data
    for( int i = 0; i < charLstNode.nChildNode(); ++i )
    {
        XMLNode charNode = charLstNode.getChildNode( "char", i );
        
        CCharData charData;

        // Get the offset of the character
        charData.offset.w = std::atof(charNode.getAttribute( "xoffset" ));
        charData.offset.h = std::atof(charNode.getAttribute( "yoffset" ));

        // Get the x advance of the character
        charData.xAdvance = std::atof(charNode.getAttribute( "xadvance" ));

        // Get the rect of the character
        charData.rect.x1 = std::atof(charNode.getAttribute( "x" ));
        charData.rect.y1 = std::atof(charNode.getAttribute( "y" ));
        charData.rect.x2 = std::atof(charNode.getAttribute( "width" ));
        charData.rect.y2 = std::atof(charNode.getAttribute( "height" ));
        
        // Get the character ID which is the ascii value of the character.
        char id = std::atoi(charNode.getAttribute( "id" ));
        
        // Add the character to our list
        m_charDataMap.insert( std::make_pair(id, charData) );
    }

    return true;

}   // LoadFromXML


/************************************************************************
*    desc:  Get the data for this character 
************************************************************************/
const CCharData & CFont::GetCharData( char id ) const
{
    // See if this character is part of the map
    auto iter = m_charDataMap.find( id );

    if( iter == m_charDataMap.end() )
        throw NExcept::CCriticalException("Font character data Error!",
                boost::str( boost::format("Font character ID can't be found (%s).\n\n%s\nLine: %s") % id % __FUNCTION__ % __LINE__ ));

    return iter->second;

}   // GetCharData


/************************************************************************
*    desc:  Get the line height 
************************************************************************/
float CFont::GetLineHeight() const
{
    return m_lineHeight;

}   // GetLineHeight


/************************************************************************
*    desc:  Get the baseline offset 
************************************************************************/
float CFont::GetBaselineOffset() const
{
    return m_baselineOffset;

}   // GetBaselineOffset


/************************************************************************
*    desc:  Get the horzontal padding
************************************************************************/
float CFont::GetHorzPadding() const
{
    return m_horzPadding;

}   // GetHorzPadding


/************************************************************************
*    desc:  Get the vertical padding
************************************************************************/
float CFont::GetVertPadding() const
{
    return m_vertPadding;

}   // GetVertPadding


/************************************************************************
*    desc:  Get the texture size
************************************************************************/
const CSize<int> & CFont::GetTextureSize() const
{
    return m_texture.GetSize();

}   // GetTextureSize


/************************************************************************
*    desc:  Get the texture ID
************************************************************************/
uint CFont::GetTextureID() const
{
    return m_texture.GetID();

}   // GetTextureID
