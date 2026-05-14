
/************************************************************************
*    FILE NAME:       fontmanager.h
*
*    DESCRIPTION:     font manager class singleton
************************************************************************/  

#ifndef __font_manager_h__
#define __font_manager_h__

// Standard lib dependencies
#include <string>
#include <map>

// Game lib dependencies
#include <2d/font.h>

class CFontMgr
{
public:

    // Get the instance of the singleton class
    static CFontMgr & Instance()
    {
        static CFontMgr fontMgr;
        return fontMgr;
    }

    // Load the material from file path
    void LoadFromXML( const std::string & filePath );

    // Get the font
    const CFont & GetFont( const std::string & name ) const;

    // Is the font in the map
    bool IsFont( const std::string & name ) const;

    // Delete a specific mesh
    void DeleteTheFont( const std::string & key );

    // Get the group name
    const std::string & GetGroup() const;

private:

    CFontMgr();
    ~CFontMgr();

private:

    // map list of fonts
    std::map<const std::string, CFont> m_fontMap;

    // Group name
    std::string m_group;

};

#endif  // __font_manager_h__
