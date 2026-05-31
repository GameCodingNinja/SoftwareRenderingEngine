
/************************************************************************
*    FILE NAME:       mixengine.h
*
*    DESCRIPTION:     Audio mix engine singleton — owns the audio device,
*                     provides the mix callback, manages multi-bus
*                     playback with per-channel and per-bus controls.
************************************************************************/

#ifndef __mix_engine_h__
#define __mix_engine_h__

// Game lib dependencies
#include <sound/audiospec.h>
#include <sound/sounddefs.h>

// Standard lib dependencies
#include <memory>
#include <mutex>
#include <atomic>
#include <array>
#include <cstdint>

// Forward declaration(s)
class IAudioDevice;
struct SWavData;

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

    // Play a sound on a bus. Returns channel index for per-channel control.
    // Caller must keep wavData alive for the duration of playback.
    // loopCount: 0 = play once, N = repeat N times, -1 = infinite
    int playSound( const SWavData & wavData, NDefs::EMixBus bus, int loopCount = 0, float volume = 1.0f );

    // Per-channel controls (channel index returned by playSound)
    void stopChannel( int channel );
    void pauseChannel( int channel );
    void resumeChannel( int channel );
    void setChannelVolume( int channel, float volume );
    bool isChannelPlaying( int channel ) const;
    bool isChannelPaused( int channel ) const;
    void fadeInChannel( int channel, uint32_t durationMs );
    void fadeOutChannel( int channel, uint32_t durationMs );

    // Per-bus controls
    void setBusVolume( NDefs::EMixBus bus, float volume );
    float getBusVolume( NDefs::EMixBus bus ) const;
    void setBusEnabled( NDefs::EMixBus bus, bool enabled );
    bool isBusEnabled( NDefs::EMixBus bus ) const;
    void pauseBus( NDefs::EMixBus bus );
    void resumeBus( NDefs::EMixBus bus );
    void stopBus( NDefs::EMixBus bus );
    bool isBusPlaying( NDefs::EMixBus bus ) const;
    bool isBusPaused( NDefs::EMixBus bus ) const;

    // Master volume
    void setMasterVolume( float volume );
    float getMasterVolume() const;

    // Global controls — pauseAll/resumeAll track per-channel state
    // so individually paused channels stay paused after resumeAll
    void pauseAll();
    void resumeAll();
    void stopAll();

    // Is any channel currently playing?
    bool isPlaying() const;

    // Get the active audio spec
    const SAudioSpec & getSpec() const { return m_spec; }

    // Channel pool sizes
    static constexpr uint16_t MUSIC_CHANNELS   = 4;
    static constexpr uint16_t AMBIENT_CHANNELS = 4;
    static constexpr uint16_t VOICE_CHANNELS   = 4;
    static constexpr uint16_t SFX_CHANNELS     = 16;
    static constexpr uint16_t TOTAL_CHANNELS   = MUSIC_CHANNELS + AMBIENT_CHANNELS
                                                + VOICE_CHANNELS + SFX_CHANNELS;

private:

    CMixEngine();
    ~CMixEngine();

    // Static callback trampoline for IAudioDevice
    static void audioCallback( void * pUserData, float * pBuffer, uint32_t frames );

    // Instance mix callback — fills the output buffer
    void mixCallback( float * pBuffer, uint32_t frames );

    // Per-channel state
    struct SMixChannel
    {
        const float * pData  = nullptr;
        uint32_t frameCount  = 0;
        uint32_t position    = 0;
        uint16_t srcChannels = 0;
        float volume         = 1.0f;
        int32_t loopCount    = 0;
        NDefs::EMixBus bus   = NDefs::SFX;
        bool playing         = false;
        bool paused          = false;
        bool pausedByGlobal  = false;   // Set by pauseAll, cleared by resumeAll

        // Fade state
        enum EFadeState : uint8_t { FADE_NONE, FADE_IN, FADE_OUT };
        EFadeState fadeState = FADE_NONE;
        float fadeVolume     = 1.0f;    // Current fade multiplier [0.0, 1.0]
        float fadeDelta      = 0.0f;    // Per-frame fade increment

        void reset()
        {
            pData = nullptr; frameCount = 0; position = 0; srcChannels = 0;
            volume = 1.0f; loopCount = 0; bus = NDefs::SFX;
            playing = false; paused = false; pausedByGlobal = false;
            fadeState = FADE_NONE; fadeVolume = 1.0f; fadeDelta = 0.0f;
        }
    };

    // Per-bus state
    struct SBusState
    {
        float volume = 1.0f;
        bool enabled = true;
        bool paused  = false;
    };

    // Get the start index and channel count for a bus pool
    static void getBusPool( NDefs::EMixBus bus, uint16_t & start, uint16_t & count );

    // Allocate the next channel on a bus (round-robin)
    int allocateChannel( NDefs::EMixBus bus );

    // Mix a single channel into the output buffer (SSE + scalar paths)
    void mixChannel( float * pBuffer, SMixChannel & channel, float effectiveVol, uint32_t frames );

    // Validation helpers
    bool isValidChannel( int channel ) const { return channel >= 0 && channel < TOTAL_CHANNELS; }
    bool isValidBus( NDefs::EMixBus bus ) const { return bus < NDefs::MIX_BUS_MAX; }

    // Audio device (platform-specific, created via factory)
    std::unique_ptr<IAudioDevice> m_pDevice;

    // Active audio spec
    SAudioSpec m_spec;

    // Channel and bus state
    std::array<SMixChannel, TOTAL_CHANNELS> m_channels;
    std::array<SBusState, NDefs::MIX_BUS_MAX> m_busState;
    float m_masterVolume;

    // Round-robin counters per bus
    uint16_t m_nextChannel[NDefs::MIX_BUS_MAX];

    // Synchronization
    mutable std::mutex m_mutex;
    std::atomic<bool> m_globalPaused;
};

#endif
