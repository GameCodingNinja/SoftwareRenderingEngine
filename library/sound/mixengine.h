
/************************************************************************
*    FILE NAME:       mixengine.h
*
*    DESCRIPTION:     Audio mix engine singleton — owns the audio device,
*                     provides the mix callback, manages playback.
*                     Phase 1: single-channel WAV playback.
************************************************************************/

#ifndef __mix_engine_h__
#define __mix_engine_h__

// Game lib dependencies
#include <sound/audiospec.h>
#include <sound/wavcodec.h>

// Standard lib dependencies
#include <memory>
#include <mutex>
#include <atomic>
#include <cstdint>

// Forward declaration(s)
class IAudioDevice;

class CMixEngine
{
public:

    // Get the instance of the singleton class
    static CMixEngine & Instance()
    {
        static CMixEngine mixEngine;
        return mixEngine;
    }

    // Initialize the mix engine and open the audio device
    void init( uint32_t sampleRate = 44100, uint16_t channels = 2, uint16_t bufferFrames = 1024 );

    // Shut down the mix engine and close the audio device
    void shutdown();

    // Play a loaded WAV sound (caller must keep wavData alive during playback)
    void playSound( const SWavData & wavData, int loopCount = 0 );

    // Stop playback
    void stop();

    // Pause or resume all audio
    void pause( bool paused );

    // Is a sound currently playing?
    bool isPlaying() const;

    // Get the active audio spec (may differ from requested after device negotiation)
    const SAudioSpec & getSpec() const { return m_spec; }

private:

    CMixEngine();
    ~CMixEngine();

    // Static callback trampoline for IAudioDevice
    static void audioCallback( void * pUserData, float * pBuffer, uint32_t frames );

    // Instance mix callback — fills the output buffer
    void mixCallback( float * pBuffer, uint32_t frames );

    // Audio device (platform-specific, created via factory)
    std::unique_ptr<IAudioDevice> m_pDevice;

    // Active audio spec
    SAudioSpec m_spec;

    // Current playback state (Phase 1: single sound)
    struct SPlayState
    {
        const float * pData  = nullptr;
        uint32_t frameCount  = 0;
        uint32_t position    = 0;
        uint16_t channels    = 0;
        int32_t loopCount    = 0;
        bool playing         = false;
    };

    SPlayState m_playState;

    // Synchronization
    std::mutex m_mutex;
    std::atomic<bool> m_paused;
};

#endif
