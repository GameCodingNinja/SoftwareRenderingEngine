
/************************************************************************
*    FILE NAME:       texturemanager.h
*
*    DESCRIPTION:     texture class singleton
************************************************************************/ 

#ifndef __texture_manager_h__
#define __texture_manager_h__

// SDL/OpenGL lib dependencies
#include <SDL_opengl.h>

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
    const CTexture & LoadFor2D( const std::string & group, const std::string & filePath, bool compressed = false );

    // Texture deleting
    void DeleteTextureGroupFor2D( const std::string & group );

    // Function call used to manage what texture is currently bound
    void BindTexture( GLuint textureID );

    // Unbind the texture and reset the flag
    void UnbindTexture();

private:

    // Constructor
    CTextureMgr();

    // Destructor
    ~CTextureMgr();

private:

    // Map containing a group of texture handles
    std::map< const std::string, std::map< const std::string, CTexture > > m_textureFor2DMapMap;

    // Current texture ID
    GLuint m_currentTextureID;

};

#endif  // __texture_manager_h__

