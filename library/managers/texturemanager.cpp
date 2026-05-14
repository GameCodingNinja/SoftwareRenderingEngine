
/************************************************************************
*    FILE NAME:       texturemanager.h
*
*    DESCRIPTION:     texture class singleton
************************************************************************/ 

// Physical component dependency
#include <managers/texturemanager.h>

// Boost lib dependencies
#include <boost/format.hpp>

// SDL lib dependencies
#include <SDL.h>

// Game lib dependencies
#include <utilities/exceptionhandling.h>
#include <softwareRender/softwareRender.h>
#include <soil/SOIL.h>
#include <soil/image_helper.h>

/************************************************************************
*    desc:  Constructer
************************************************************************/
CTextureMgr::CTextureMgr()
    : m_currentTextureID(0)
{
}   // constructor


/************************************************************************
*    desc:  destructer                                                             
************************************************************************/
CTextureMgr::~CTextureMgr()
{
}   // destructer


/************************************************************************
*    desc:  Load the texture from file path
************************************************************************/
const CTexture & CTextureMgr::LoadFor2D( const std::string & group, const std::string & filePath, bool compressed )
{
    // Create the map group if it doesn't already exist
    auto mapMapIter = m_textureFor2DMapMap.find( group );
    if( mapMapIter == m_textureFor2DMapMap.end() )
            mapMapIter = m_textureFor2DMapMap.insert( std::make_pair(group, std::map<const std::string, CTexture>()) ).first;

    // See if this texture has already been loaded
    auto mapIter = mapMapIter->second.find( filePath );

    // If it's not found, load the texture and add it to the list
    if( mapIter == mapMapIter->second.end() )
    {
        CTexture texture;
        int channels(0);

        // Use soil to laad the texture
        uchar * pData = SOIL_load_image( filePath.c_str(), &texture.m_size.w, &texture.m_size.h, &channels, 4 );
        if( (pData == NULL) || (texture.m_size.w == 0) || (texture.m_size.h == 0) )
            throw NExcept::CCriticalException("Texture Load Error!",
                    boost::str( boost::format("Unable to load texture (%s).\n\n%s\nLine: %s") % filePath % __FUNCTION__ % __LINE__ ));

        // Swap the red and blue
        SwapRedandBlue( pData, texture.m_size.w, texture.m_size.h, 4 );

        // Pass the texture data to the software render. It now owns the pointer
        texture.m_id = CSoftwareRender::Instance().CreateTexture(
            pData,
            texture.m_size.w,
            texture.m_size.h );

        if( texture.GetID() == 0 )
        {
            throw NExcept::CCriticalException("Load Texture Error!", 
                    boost::str( boost::format("Error loading texture (%s).\n\n%s\nLine: %s") % filePath % __FUNCTION__ % __LINE__ ));
        }

        // Insert the new texture info
        mapIter = mapMapIter->second.insert( std::make_pair(filePath, texture) ).first;
    }

    return mapIter->second;

}   // LoadFor2D */


/************************************************************************
*    desc:  Delete a texture in a group
************************************************************************/
void CTextureMgr::DeleteTextureGroupFor2D( const std::string & group )
{
    // Free the texture group if it exists
    auto mapMapIter = m_textureFor2DMapMap.find( group );
    if( mapMapIter != m_textureFor2DMapMap.end() )
    {
        // Delete all the textures in this group
        for( auto mapIter = mapMapIter->second.begin();
                 mapIter != mapMapIter->second.end();
                 ++mapIter )
        {
            CSoftwareRender::Instance().DeleteTexture( mapIter->second.m_id );
        }

        // Erase this group
        m_textureFor2DMapMap.erase( mapMapIter );
    }

}   // DeleteGroupTexturesFor2D


/************************************************************************
*    desc:  Function call used to manage what texture is currently bound.
*           This insures that we don't keep rebinding the same texture
************************************************************************/
void CTextureMgr::BindTexture( GLuint textureID )
{
    /*if( m_currentTextureID != textureID )
    {
        // save the current binding
        m_currentTextureID = textureID;

        // Have OpenGL bind this texture now
        glBindTexture(GL_TEXTURE_2D, textureID);
    }*/

}   // BindTexture


/************************************************************************
*    desc:  Unbind the texture and reset the flag
************************************************************************/
void CTextureMgr::UnbindTexture()
{
    //m_currentTextureID = 0;
    //glBindTexture(GL_TEXTURE_2D, 0);

}   // UnbindTexture



/*GLuint CTextureMgr::LoadFor2D( const std::string & group, const std::string & filePath )
{
    // Create the map group if it doesn't already exist
    auto mapMapIter = m_textureFor2DMapMap.find( group );
    if( mapMapIter == m_textureFor2DMapMap.end() )
    {
            m_textureFor2DMapMap.insert( std::make_pair(group, std::map<const std::string, GLuint>()) );
            mapMapIter = m_textureFor2DMapMap.find( group );
    }

    // See if this texture has already been loaded
    auto mapIter = mapMapIter->second.find( filePath );

    // If it's not found, load the texture and add it to the list
    if( mapIter == mapMapIter->second.end() )
    {
        int width(0), height(0), channels(0);
        unsigned char*pImage = SOIL_load_image(filePath.c_str(), &width, &height, &channels, SOIL_LOAD_AUTO);
        if( pImage == nullptr )
        {
            throw NExcept::CCriticalException("Load Texture Error!", 
                boost::str( boost::format("Error loading texture (%s).\n\n%s\nLine: %s") % filePath % __FUNCTION__ % __LINE__ ));
        }

        GLuint textureID(0);
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        GLint mode = GL_RGB;
        if(channels == 4)
            mode = GL_RGBA;

        // Load the data into video memory
        glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, mode, GL_UNSIGNED_BYTE, pImage );

        // This needs to be done here otherwise the texture will render black
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // We're done with this texture so unbind it
        glBindTexture(GL_TEXTURE_2D, 0);

        // Insert the new texture info
        mapMapIter->second.insert( std::make_pair(filePath, textureID) );

        // Find the new texture for the return value
        mapIter = mapMapIter->second.find( filePath );

        // Free the memory since this is our only use for it
        SOIL_free_image_data( pImage );
    }
 
    return mapIter->second;

}   // LoadFor2D */


/************************************************************************
*    desc:  Load the texture from file path
************************************************************************/
// Commented out version using SDL2 Image
/*GLuint CTextureMgr::LoadFor2D( const std::string & group, const std::string & filePath )
{
    // Create the map group if it doesn't already exist
    auto mapMapIter = m_textureFor2DMapMap.find( group );
    if( mapMapIter == m_textureFor2DMapMap.end() )
    {
        m_textureFor2DMapMap.insert( std::make_pair(group, std::map<const std::string, GLuint>()) );
        mapMapIter = m_textureFor2DMapMap.find( group );
    }

    // See if this texture has already been loaded
    auto mapIter = mapMapIter->second.find( filePath );

    // If it's not found, load the texture and add it to the list
    if( mapIter == mapMapIter->second.end() )
    {
        // Load the image - Note: currently only using PNG's right now
        SDL_Surface* pSurface = SDL_LoadBMP( filePath.c_str() );
        if( pSurface == nullptr )
        {
            throw NExcept::CCriticalException("Load Texture Error!", 
                boost::str( boost::format("Error loading texture (%s). %s\n\n%s\nLine: %s") % filePath % SDL_GetError() % __FUNCTION__ % __LINE__ ));
        }

        GLuint textureID = 0;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        GLint mode = GL_RGB;
        if(pSurface->format->BytesPerPixel == 4)
            mode = GL_RGBA;

        // Load the data into video memory
        glTexImage2D( GL_TEXTURE_2D, 0, mode, pSurface->w, pSurface->h, 0, mode, GL_UNSIGNED_BYTE, pSurface->pixels );

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // We're done with this texture so unbind it
        glBindTexture(GL_TEXTURE_2D, 0);

        // Insert the new texture info
        mapMapIter->second.insert( std::make_pair(filePath, textureID) );

        // Find the new texture for the return value
        mapIter = mapMapIter->second.find( filePath );

        // Free the surface since this is our only use for it
        SDL_FreeSurface( pSurface );
    }

    return mapIter->second;

}   // LoadFor2D */
