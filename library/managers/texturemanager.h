
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
#include <common/defs.h>

// Forward declaration(s)
class CSRTexture;

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

    // Texture deleting
    void deleteTextureGroup( const std::string & group );

    // Create a texture from raw pixel data. The pointer is now owned by this class
    uint createTexture( uchar * pData, int w, int h );

    // Delete a texture by ID
    void deleteTexture( uint Id );

    // Get the texture by ID
    CSRTexture * getTexture( uint Id );

private:

    // Constructor
    CTextureMgr();

    // Destructor
    ~CTextureMgr();

private:

    // Map containing a group of texture handles
    std::map< const std::string, std::map< const std::string, CTexture > > m_textureMapMap;

    // map of allocated textures
    std::map<uint, CSRTexture *> m_pTextureMap;

    // Texture ID incrementor
    uint m_textIdInc;

};

#endif  // __texture_manager_h__
