
/************************************************************************
*    FILE NAME:       soundmanager.cpp
*
*    DESCRIPTION:     Sound Manager class singleton — XML-driven group
*                     loading, sound IDs, playlist IDs, bus routing
************************************************************************/

// Physical component dependency
#include <sound/soundmanager.h>

// Game lib dependencies
#include <sound/mixengine.h>
#include <utilities/xmlParser.h>
#include <utilities/exceptionhandling.h>
#include <utilities/genfunc.h>

// Standard lib dependencies
#include <cstring>


/************************************************************************
*    DESC:  Constructor
************************************************************************/
CSoundMgr::CSoundMgr() :
    m_initialized( false )
{
}


/************************************************************************
*    DESC:  Destructor
************************************************************************/
CSoundMgr::~CSoundMgr()
{
}


/************************************************************************
*    DESC:  Initialize the mix engine
************************************************************************/
void CSoundMgr::init()
{
    CMixEngine::Instance().init();
    m_initialized = true;
}


/************************************************************************
*    DESC:  Stop, free all sounds, and shut down the mix engine
************************************************************************/
void CSoundMgr::cleanup()
{
    if( m_initialized )
    {
        // Free all sounds in all groups
        for( auto & mapMapIter : m_soundMapMap )
        {
            for( auto & mapIter : mapMapIter.second )
            {
                mapIter.second.free();
            }
        }

        m_soundMapMap.clear();
        m_playListMapMap.clear();

        CMixEngine::Instance().shutdown();
        m_initialized = false;
    }
}


/************************************************************************
*    DESC:  Load all sounds of a specific group
************************************************************************/
void CSoundMgr::loadGroup( const std::string & group )
{
    if( !m_initialized )
        return;

    // Make sure the group has been defined in the list table file
    auto listTableIter = m_listTableMap.find( group );
    if( listTableIter == m_listTableMap.end() )
        throw NExcept::CCriticalException( "Sound List Load Group Data Error!",
            NGenFunc::FormatString( "Sound list group name can't be found (%s).\n\n%s\nLine: %d",
                group, __FUNCTION__, __LINE__ ) );

    // Load the group data if it doesn't already exist
    if( m_soundMapMap.find( group ) == m_soundMapMap.end() )
    {
        for( auto & iter : listTableIter->second )
            load( group, iter );
    }
    else
    {
        throw NExcept::CCriticalException( "Sound Data List Load Error!",
            NGenFunc::FormatString( "Sound data list group has already been loaded (%s).\n\n%s\nLine: %d",
                group, __FUNCTION__, __LINE__ ) );
    }
}


/************************************************************************
*    DESC:  Load sounds from an XML file for a group
************************************************************************/
void CSoundMgr::load( const std::string & group, const std::string & filePath )
{
    // Open and parse the XML file
    const XMLNode mainNode = XMLNode::openFileHelper( filePath.c_str(), "soundList" );

    // Create a new map inside of our map and get an iterator into it
    auto soundMapIter = m_soundMapMap.emplace( group, std::map<const std::string, CSound>() ).first;

    // Get the node to the sound files
    const XMLNode soundFilesNode = mainNode.getChildNode( "soundFiles" );

    // Parse both <load> and <stream> tags
    const char * typeTag[] = { "load", "stream" };
    const CSound::ESoundType typeEnum[] = { CSound::EST_LOADED, CSound::EST_STREAM };

    for( int t = 0; t < 2; ++t )
    {
        const int count = soundFilesNode.nChildNode( typeTag[t] );

        for( int j = 0; j < count; ++j )
        {
            // Get the node
            const XMLNode loadNode = soundFilesNode.getChildNode( typeTag[t], j );

            // Get the id
            const std::string id = loadNode.getAttribute( "id" );

            // Determine the bus (default: SFX for loaded, MUSIC for streamed)
            NDefs::EMixBus bus = (typeEnum[t] == CSound::EST_STREAM) ? NDefs::MUSIC : NDefs::SFX;
            if( loadNode.isAttributeSet( "bus" ) )
                bus = NDefs::StringToBus( loadNode.getAttribute( "bus" ) );

            // Add the sound data to the map
            auto iter = soundMapIter->second.emplace( id, CSound( typeEnum[t], bus ) );

            // Check for duplicate id names
            if( !iter.second )
            {
                throw NExcept::CCriticalException( "Sound Data Load Group Error!",
                    NGenFunc::FormatString( "Duplicate sound ID (%s - %s).\n\n%s\nLine: %d",
                        id, group, __FUNCTION__, __LINE__ ) );
            }

            // Load the sound file
            iter.first->second.loadFromNode( loadNode );
        }
    }

    // Get the node to the play lists
    const XMLNode playListSetNode = mainNode.getChildNode( "playlistSet" );
    if( !playListSetNode.isEmpty() )
    {
        // Create a new map inside of our map
        auto playListMapIter = m_playListMapMap.emplace(
            group, std::map<const std::string, CPlayList>() ).first;

        for( int i = 0; i < playListSetNode.nChildNode(); ++i )
        {
            // Get the play list node
            const XMLNode playListNode = playListSetNode.getChildNode( i );

            // Get the id
            const std::string id = playListNode.getAttribute( "id" );

            // Add the playlist data to the map
            auto iter = playListMapIter->second.emplace(
                id, std::string( playListNode.getAttribute( "playtype" ) ) );

            // Check for duplicate names
            if( !iter.second )
            {
                throw NExcept::CCriticalException( "Playlist Data Group Load Error!",
                    NGenFunc::FormatString( "Duplicate playlist name! (%s - %s).\n\n%s\nLine: %d",
                        id, group, __FUNCTION__, __LINE__ ) );
            }

            iter.first->second.loadFromNode( playListNode, group, soundMapIter->second );
        }
    }
}


/************************************************************************
*    DESC:  Free a sound group
************************************************************************/
void CSoundMgr::freeGroup( const std::string & group )
{
    // Free the sound group if it exists
    auto soundMapIter = m_soundMapMap.find( group );
    if( soundMapIter != m_soundMapMap.end() )
    {
        // Free all the sounds in this group
        for( auto & mapIter : soundMapIter->second )
            mapIter.second.free();

        m_soundMapMap.erase( soundMapIter );
    }

    // Free the playlist group if it exists
    auto playLstMapIter = m_playListMapMap.find( group );
    if( playLstMapIter != m_playListMapMap.end() )
        m_playListMapMap.erase( playLstMapIter );
}


/************************************************************************
*    DESC:  Get a sound — checks playlists first, then individual sounds.
*           This allows a playlist ID to be used interchangeably with
*           a sound ID — callers don't need to know the difference.
************************************************************************/
CSound & CSoundMgr::getSound( const std::string & group, const std::string & soundID )
{
    if( !m_initialized )
        return m_null_sound;

    // Check if this is a playlist sound ID
    auto playListMapIter = m_playListMapMap.find( group );
    if( playListMapIter != m_playListMapMap.end() )
    {
        auto iter = playListMapIter->second.find( soundID );
        if( iter != playListMapIter->second.end() )
            return iter->second.getSound();
    }

    // Fall back to individual sound lookup
    auto soundMapIter = m_soundMapMap.find( group );
    if( soundMapIter == m_soundMapMap.end() )
    {
        NGenFunc::PostDebugMsg( NGenFunc::FormatString(
            "Sound group can't be found (%s).", group ) );
        return m_null_sound;
    }

    auto iter = soundMapIter->second.find( soundID );
    if( iter == soundMapIter->second.end() )
    {
        NGenFunc::PostDebugMsg( NGenFunc::FormatString(
            "Sound ID can't be found (%s - %s).", group, soundID ) );
        return m_null_sound;
    }

    return iter->second;
}


/************************************************************************
*    DESC:  Get a playlist directly
************************************************************************/
CPlayList & CSoundMgr::getPlayList( const std::string & group, const std::string & playLstID )
{
    auto playListMapIter = m_playListMapMap.find( group );
    if( playListMapIter != m_playListMapMap.end() )
    {
        auto iter = playListMapIter->second.find( playLstID );
        if( iter != playListMapIter->second.end() )
            return iter->second;
    }

    return m_null_playLst;
}


/************************************************************************
*    DESC:  Play a sound
************************************************************************/
void CSoundMgr::play( const std::string & group, const std::string & soundID, int loopCount )
{
    getSound( group, soundID ).play( loopCount );
}


/************************************************************************
*    DESC:  Pause a sound
************************************************************************/
void CSoundMgr::pause( const std::string & group, const std::string & soundID )
{
    getSound( group, soundID ).pause();
}


/************************************************************************
*    DESC:  Resume a sound
************************************************************************/
void CSoundMgr::resume( const std::string & group, const std::string & soundID )
{
    getSound( group, soundID ).resume();
}


/************************************************************************
*    DESC:  Stop a sound
************************************************************************/
void CSoundMgr::stop( const std::string & group, const std::string & soundID )
{
    getSound( group, soundID ).stop();
}


/************************************************************************
*    DESC:  Set/Get volume for a sound
************************************************************************/
void CSoundMgr::setVolume( const std::string & group, const std::string & soundID, int volume )
{
    getSound( group, soundID ).setVolume( volume );
}

int CSoundMgr::getVolume( const std::string & group, const std::string & soundID )
{
    return getSound( group, soundID ).getVolume();
}


/************************************************************************
*    DESC:  Is a sound playing?
************************************************************************/
bool CSoundMgr::isPlaying( const std::string & group, const std::string & soundID )
{
    return getSound( group, soundID ).isPlaying();
}


/************************************************************************
*    DESC:  Is a sound paused?
************************************************************************/
bool CSoundMgr::isPaused( const std::string & group, const std::string & soundID )
{
    return getSound( group, soundID ).isPaused();
}


/************************************************************************
*    DESC:  Stop all playing sounds
************************************************************************/
void CSoundMgr::stopAllSound()
{
    CMixEngine::Instance().stopAll();
}


/************************************************************************
*    DESC:  Pause all playing sounds
************************************************************************/
void CSoundMgr::pauseAll()
{
    CMixEngine::Instance().pauseAll();
}


/************************************************************************
*    DESC:  Resume all sounds paused by pauseAll
************************************************************************/
void CSoundMgr::resumeAll()
{
    CMixEngine::Instance().resumeAll();
}
