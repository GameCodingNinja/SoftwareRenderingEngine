/************************************************************************
*    FILE NAME:       font.h
*
*    DESCRIPTION:     Font class
************************************************************************/  

#ifndef __font_h__
#define __font_h__

// Standard lib dependencies
#include <string>
#include <map>

// Game lib dependencies
#include <common/size.h>
#include <common/defs.h>
#include <common/rect.h>
#include <common/texture.h>

class CCharData
{
public:
    CCharData() : xAdvance(0) {}
    ~CCharData() {}

    // Character offsets
    CSize<float> offset;

    // Character rect
    CRect<float> rect;

    // Amount to advance
    float xAdvance;
};


class CFont
{
public:

    CFont();
    ~CFont();

    // Load from XML file
    bool LoadFromXML( const std::string & group, const std::string & filePath );

    // Get the data for this character
    const CCharData & GetCharData( char id ) const;

    // Get the line height
    float GetLineHeight() const;

    // Get the baseline offset
    float GetBaselineOffset() const;

    // Get the horzontal padding
    float GetHorzPadding() const;

    // Get the vertical padding
    float GetVertPadding() const;

    // Get the texture size
    const CSize<int> & GetTextureSize() const;

    // Get the texture ID
    uint GetTextureID() const;

private:

    // map list of character data
    std::map<char, CCharData> m_charDataMap;

    // Line height
    float m_lineHeight;

    // base line offset
    float m_baselineOffset;

    // Horizontal padding
    float m_horzPadding;

    // Vertival padding
    float m_vertPadding;

    // The texture
    CTexture m_texture;
};

#endif  // __font_h__
