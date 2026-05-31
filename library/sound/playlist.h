
/************************************************************************
*    FILE NAME:       playlist.h
*
*    DESCRIPTION:     Play list class — sequential or random playback
*                     of a collection of sounds with anti-repeat shuffle
************************************************************************/

#ifndef __playlist_h__
#define __playlist_h__

// Game lib dependencies
#include <sound/sound.h>

// Standard lib dependencies
#include <vector>
#include <map>
#include <string>
#include <cstdint>

// Forward declaration(s)
struct XMLNode;

class CPlayList
{
public:

    enum EPlayListType
    {
        EST_NULL = 0,
        EST_RANDOM,
        EST_SEQUENTIAL
    };

    CPlayList( const std::string strType );
    CPlayList( const CPlayList & playLst );
    CPlayList();
    ~CPlayList();

    // Load the playlist from an XML node
    void loadFromNode(
        const XMLNode & node,
        const std::string & group,
        std::map< const std::string, CSound > & soundMap );

    // Get the next sound in the playlist
    CSound & getSound();

    // Play the next sound in the playlist
    void play( int loopCount = 0 );

    // Stop the current sound
    void stop();

    // Pause the current sound
    void pause();

    // Resume the current sound
    void resume();

    // Set/Get volume on the current sound
    void setVolume( int volume );
    int getVolume();

    // Is the current sound playing?
    bool isPlaying();

    // Is the current sound paused?
    bool isPaused();

private:

    // Shuffle the playlist with anti-repeat logic
    void shuffle();

private:

    // Counter for tracking position in the list
    int16_t m_counter;

    // Current sound index
    int16_t m_current;

    // Playlist type — random or sequential
    EPlayListType m_type;

    // Vector of sounds (copies from the sound map)
    std::vector<CSound> m_soundVec;

    // Null sound for empty playlists
    static CSound m_null_Sound;
};

#endif
