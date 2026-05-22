
/************************************************************************
*    FILE NAME:       texturemanager.h
*
*    DESCRIPTION:     texture class singleton
************************************************************************/ 

#ifndef __texture_manager_h__
#define __texture_manager_h__

// Standard lib dependencies
#include <string>
#include <map>

// Game lib dependencies
#include <common/texture.h>

class CTextureMgr
{
public:

    // Get the instance of the singleton class
    static CTextureMgr & Instance()
    {
        static CTextureMgr textMgr;
        return textMgr;
    }

    // Load the texture from file path
    const CTexture & load( const std::string & group, const std::string & filePath );

    // Delete a texture group
    void deleteTextureGroup( const std::string & group );

private:

    // Constructor
    CTextureMgr();

    // Destructor
    ~CTextureMgr();

private:

    // Map containing a group of textures keyed by file path
    std::map< const std::string, std::map< const std::string, CTexture > > m_textureMapMap;

};

#endif
