
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
#include <utilities/deletefuncs.h>
#include <softwareRender/srtexture.h>
#include <soil/stb_image_aug.h>

/************************************************************************
*    desc:  Constructer
************************************************************************/
CTextureMgr::CTextureMgr() :
    m_textIdInc(0)
{
}

/************************************************************************
*    desc:  destructer                                                             
************************************************************************/
CTextureMgr::~CTextureMgr()
{
    NDelFunc::DeleteMapPointers(m_pTextureMap);

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

        // Create the texture. The pointer is now owned by this class
        texture.m_id = createTexture(
            pData,
            texture.m_size.w,
            texture.m_size.h );

        if( texture.GetID() == 0 )
        {
            throw NExcept::CCriticalException("Load Texture Error!", 
                    NGenFunc::FormatString("Error loading texture (%s).\n\n%s\nLine: %d", filePath, __FUNCTION__, __LINE__));
        }

        // Insert the new texture info
        mapIter = mapMapIter->second.insert( std::make_pair(filePath, texture) ).first;
    }

    return mapIter->second;
}

/***************************************************************************
*   desc:  Create a texture. The pointer is now owned by this class
****************************************************************************/
uint CTextureMgr::createTexture( uchar * pData, int w, int h )
{
    ++m_textIdInc;

    m_pTextureMap.insert( std::make_pair(m_textIdInc, new CSRTexture( w, h, pData )) );

    return m_textIdInc;
}

/************************************************************************
*    desc:  Delete a texture in a group
************************************************************************/
void CTextureMgr::deleteTextureGroup( const std::string & group )
{
    // Free the texture group if it exists
    auto mapMapIter = m_textureMapMap.find( group );
    if( mapMapIter != m_textureMapMap.end() )
    {
        // Delete all the textures in this group
        for( auto mapIter = mapMapIter->second.begin();
                 mapIter != mapMapIter->second.end();
                 ++mapIter )
        {
            deleteTexture( mapIter->second.m_id );
        }

        // Erase this group
        m_textureMapMap.erase( mapMapIter );
    }

}

/***************************************************************************
*   desc:  Delete the texture
****************************************************************************/
void CTextureMgr::deleteTexture( uint Id )
{
    // Delete the texture if it exists
    auto mapIter = m_pTextureMap.find( Id );
    if( mapIter != m_pTextureMap.end() )
    {
        NDelFunc::Delete( mapIter->second );
        m_pTextureMap.erase( mapIter );
    }
}

/***************************************************************************
*   desc:  Get the texture
****************************************************************************/
CSRTexture * CTextureMgr::getTexture( uint Id )
{
    // Find the texture if it exists
    auto mapIter = m_pTextureMap.find( Id );
    if( mapIter != m_pTextureMap.end() )
    {
        return mapIter->second;
    }
    else
    {
        throw NExcept::CCriticalException("Texture Find Error!",
            NGenFunc::FormatString("Unable to find texture Id (%d).\n\n%s\nLine: %d", Id, __FUNCTION__, __LINE__));
    }

    return nullptr;

}
