
/************************************************************************
*    FILE NAME:       soundmanager.h
*
*    DESCRIPTION:     Sound Manager class singleton — XML-driven group
*                     loading, sound IDs, playlist IDs, bus routing
************************************************************************/

#ifndef __sound_manager_h__
#define __sound_manager_h__

// Game lib dependencies
#include <managers/managerbase.h>
#include <sound/sound.h>
#include <sound/playlist.h>

// Standard lib dependencies
#include <map>
#include <string>

class CSoundMgr : public CManagerBase
{
public:

    // Get the instance of the singleton class
    static CSoundMgr & Instance()
    {
        static CSoundMgr soundMgr;
        return soundMgr;
    }

    // Initialize the mix engine
    void init();

    // Stop, free all sounds, and shut down the mix engine
    void cleanup();

    // Load all sounds of a specific group
    void loadGroup( const std::string & group );

    // Free a sound group
    void freeGroup( const std::string & group );

    // Play a sound (checks playlists first, then individual sounds)
    void play( const std::string & group, const std::string & soundID, int loopCount = 0 );

    // Pause a sound
    void pause( const std::string & group, const std::string & soundID );

    // Resume a sound
    void resume( const std::string & group, const std::string & soundID );

    // Stop a sound
    void stop( const std::string & group, const std::string & soundID );

    // Set/Get volume for a sound
    void setVolume( const std::string & group, const std::string & soundID, int volume );
    int getVolume( const std::string & group, const std::string & soundID );

    // Is a sound playing?
    bool isPlaying( const std::string & group, const std::string & soundID );

    // Is a sound paused?
    bool isPaused( const std::string & group, const std::string & soundID );

    // Get a sound directly (checks playlists first)
    CSound & getSound( const std::string & group, const std::string & soundID );

    // Get a playlist directly
    CPlayList & getPlayList( const std::string & group, const std::string & playLstID );

    // Stop all playing sounds
    void stopAllSound();

    // Pause all playing sounds (for pause menus)
    void pauseAll();

    // Resume all sounds paused by pauseAll
    void resumeAll();

private:

    // Constructor
    CSoundMgr();

    // Destructor
    virtual ~CSoundMgr();

    // Load sounds from an XML file for a group
    void load( const std::string & group, const std::string & filePath );

private:

    // Map containing a group of sound ID's
    std::map< const std::string, std::map< const std::string, CSound > > m_soundMapMap;

    // Map containing a group of play list ID's
    std::map< const std::string, std::map< const std::string, CPlayList > > m_playListMapMap;

    // Null members for safe returns
    CPlayList m_null_playLst;
    CSound m_null_sound;

    // Flag to indicate the audio was initialized
    bool m_initialized;
};

#endif
