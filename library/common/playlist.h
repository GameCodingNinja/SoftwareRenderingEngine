
/************************************************************************
*    FILE NAME:       playlist.h
*
*    DESCRIPTION:     Play list class
************************************************************************/

#ifndef __play_list_h__
#define __play_list_h__

// Physical component dependency

// Standard lib dependencies
#include <vector>

// Boost lib dependencies

// Game lib dependencies
#include <common/sound.h>
#include <common/defs.h>

// Game dependencies

// Forward declaration(s)


class CPlayList
{
public:

    enum EPlayListType
    {
        EST_NULL=0,
        EST_RANDOM,
        EST_SEQUENTIAL,
        EST_MAX,
    };

    // Constructor
    CPlayList( EPlayListType type );

    // Destructor
    virtual ~CPlayList();

    // Set the sound for the playlist
    void SetSound( const CSound & sound );
    CSound & GetSound();

    // Play the play list
    void Play( int loopCount = 0, int channel = -1 );

private:

    // Counter used sequential play
    uint m_counter;

    // Sound type - loaded or stream
    EPlayListType m_type;

    // vector of sounds
    // NOTE: This class doesn't own the sound pointer
    std::vector<CSound> m_soundVec;

};

#endif  // __play_list_h__


