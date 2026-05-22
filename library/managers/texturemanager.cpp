
/************************************************************************
*    FILE NAME:       texturemanager.cpp
*
*    DESCRIPTION:     texture class singleton
************************************************************************/ 

// Physical component dependency
#include <managers/texturemanager.h>

// Game lib dependencies
#include <utilities/exceptionhandling.h>
#include <utilities/genfunc.h>
#include <soil/stb_image_aug.h>

/************************************************************************
*    desc:  Constructer
************************************************************************/
CTextureMgr::CTextureMgr()
{
}


/************************************************************************
*    desc:  destructer                                                             
************************************************************************/
CTextureMgr::~CTextureMgr()
{
    for( auto & mapMapIter : m_textureMapMap )
        for( auto & mapIter : mapMapIter.second )
            if( mapIter.second.m_pData != nullptr )
                stbi_image_free( mapIter.second.m_pData );

}


/************************************************************************
*    desc:  Load the texture from file path
************************************************************************/
const CTexture & CTextureMgr::load( const std::string & group, const std::string & filePath )
{
    // Create the map group if it doesn't already exist
    auto mapMapIter = m_textureMapMap.find( group );
    if( mapMapIter == m_textureMapMap.end() )
        mapMapIter = m_textureMapMap.insert( std::make_pair(group, std::map<const std::string, CTexture>()) ).first;

    // See if this texture has already been loaded
    auto mapIter = mapMapIter->second.find( filePath );

    // If it's not found, load the texture and add it to the list
    if( mapIter == mapMapIter->second.end() )
    {
        CTexture texture;
        int channels(0);

        // Use stb_image to load the texture
        uchar * pData = stbi_load( filePath.c_str(), &texture.m_size.w, &texture.m_size.h, &channels, 4 );
        if( (pData == NULL) || (texture.m_size.w == 0) || (texture.m_size.h == 0) )
            throw NExcept::CCriticalException("Texture Load Error!",
                    NGenFunc::FormatString("Unable to load texture (%s).\n\n%s\nLine: %d", filePath, __FUNCTION__, __LINE__));

        // Swap RGBA to BGRA for native pixel format
        int pixelCount = texture.m_size.w * texture.m_size.h;
        for( int i = 0; i < pixelCount; ++i )
        {
            int idx = i * 4;
            uchar tmp = pData[idx];
            pData[idx] = pData[idx + 2];
            pData[idx + 2] = tmp;
        }

        texture.m_pData = pData;

        // Insert the new texture info
        mapIter = mapMapIter->second.insert( std::make_pair(filePath, texture) ).first;
    }

    return mapIter->second;

}


/************************************************************************
*    desc:  Delete a texture group
************************************************************************/
void CTextureMgr::deleteTextureGroup( const std::string & group )
{
    // Free the texture group if it exists
    auto mapMapIter = m_textureMapMap.find( group );
    if( mapMapIter != m_textureMapMap.end() )
    {
        // Free all the texture data in this group
        for( auto & mapIter : mapMapIter->second )
            if( mapIter.second.m_pData != nullptr )
                stbi_image_free( mapIter.second.m_pData );

        // Erase this group
        m_textureMapMap.erase( mapMapIter );
    }

}
