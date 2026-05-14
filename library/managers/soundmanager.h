
/************************************************************************
*    FILE NAME:       soundmanager.h
*
*    DESCRIPTION:     Class template
************************************************************************/

#ifndef __sound_manager_h__
#define __sound_manager_h__

// Physical component dependency
#include <managers/managerbase.h>

// Boost lib dependencies

// Game lib dependencies
#include <common/sound.h>
#include <common/playlist.h>

// Forward declaration(s)


class CSoundMgr : public CManagerBase
{
public:

    // Get the instance of the singleton class
    static CSoundMgr & Instance()
    {
        static CSoundMgr soundMgr;
        return soundMgr;
    }

    // Load all of the sounds of a specific group
    void LoadGroup( const std::string & group );

    // Play a sound
    void Play( const std::string & group, const std::string & soundID );

    // Pause a sound
    void Pause( const std::string & group, const std::string & soundID );

    // Resume a sound
    void Resume( const std::string & group, const std::string & soundID );

    // Resume a sound
    void Stop( const std::string & group, const std::string & soundID );

    // Get the sound
    CSound & GetSound( const std::string & group, const std::string & soundID );

    // Delete a sound group
    void DeleteSoundGroup( const std::string & group );

protected:

    // Load custom data from node
    virtual void LoadFromNode( const XMLNode & node );

private:

    // Constructor
    CSoundMgr();

    // Destructor
    virtual ~CSoundMgr();

    // Load all object information from an xml
    void LoadFromXML( const std::string & group, const std::string & filePath );

private:

    // Map containing a group of sound ID's
    std::map< const std::string, std::map< const std::string, CSound > > m_soundMapMap;

    // Map containing a group of play list ID's
    // Do not free the sounds copied to the play list
    std::map< const std::string, std::map< const std::string, CPlayList > > m_playListMapMap;

    // sound channel for the next sound
    int m_soundChannel;

    // max sound channels
    int m_maxSoundChannels;

};

#endif  // __sound_manager_h__
