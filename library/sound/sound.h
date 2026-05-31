
/************************************************************************
*    FILE NAME:       sound.h
*
*    DESCRIPTION:     Class to hold the sound data and playback state
************************************************************************/

#ifndef __sound_h__
#define __sound_h__

// Game lib dependencies
#include <sound/sounddefs.h>

// Standard lib dependencies
#include <string>
#include <memory>
#include <cstdint>

// Forward declaration(s)
struct XMLNode;
struct SWavData;

class CSound
{
public:

    enum ESoundType
    {
        EST_NULL = 0,
        EST_LOADED,
        EST_STREAM,
    };

    CSound( ESoundType type, NDefs::EMixBus bus = NDefs::SFX );
    CSound( const CSound & sound );
    CSound();
    ~CSound();

    // Load the sound from an XML node
    void loadFromNode( const XMLNode & node );

    // Play the sound
    void play( int loopCount = 0 );

    // Stop the sound
    void stop();

    // Pause the sound
    void pause();

    // Resume the sound
    void resume();

    // Set/Get the volume (0–128)
    void setVolume( int volume );
    int getVolume() const;

    // Is the sound playing?
    bool isPlaying() const;

    // Is the sound paused?
    bool isPaused() const;

    // Free the sound data and stop playback
    void free();

    // Get the bus this sound plays on
    NDefs::EMixBus getBus() const { return m_bus; }

    // Equality/inequality operators (compare by shared data pointer)
    bool operator == ( const CSound & sound ) const;
    bool operator != ( const CSound & sound ) const;

private:

    // Sound type — loaded or stream
    ESoundType m_type;

    // Which mix bus this sound plays on
    NDefs::EMixBus m_bus;

    // Shared ownership of PCM data (copies share the same data)
    std::shared_ptr<SWavData> m_spWavData;

    // Channel this sound is currently playing on (-1 if not playing)
    int m_channel;

    // Sound volume (0–128)
    int16_t m_volume;
};

#endif
