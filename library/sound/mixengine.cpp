
/************************************************************************
*    FILE NAME:       mixengine.cpp
*
*    DESCRIPTION:     Audio mix engine singleton — owns the audio device,
*                     provides the mix callback, manages multi-bus
*                     playback with per-channel and per-bus controls.
************************************************************************/

// Physical component dependency
#include <sound/mixengine.h>

// Game lib dependencies
#include <sound/iaudiodevice.h>
#include <sound/audiodevicefactory.h>
#include <sound/wavcodec.h>
#include <utilities/exceptionhandling.h>
#include <utilities/genfunc.h>

// Standard lib dependencies
#include <cstring>
#include <algorithm>
#include <xmmintrin.h>


/************************************************************************
*    DESC:  Constructor
************************************************************************/
CMixEngine::CMixEngine() :
    m_masterVolume( 1.0f ),
    m_nextChannel{},
    m_globalPaused( false )
{
}


/************************************************************************
*    DESC:  Destructor
************************************************************************/
CMixEngine::~CMixEngine()
{
    shutdown();
}


/************************************************************************
*    DESC:  Get the start index and channel count for a bus pool
************************************************************************/
void CMixEngine::getBusPool( NDefs::EMixBus bus, uint16_t & start, uint16_t & count )
{
    switch( bus )
    {
        case NDefs::MUSIC:   start = 0;                                                  count = MUSIC_CHANNELS;   break;
        case NDefs::AMBIENT: start = MUSIC_CHANNELS;                                     count = AMBIENT_CHANNELS; break;
        case NDefs::VOICE:   start = MUSIC_CHANNELS + AMBIENT_CHANNELS;                  count = VOICE_CHANNELS;   break;
        case NDefs::SFX:     start = MUSIC_CHANNELS + AMBIENT_CHANNELS + VOICE_CHANNELS; count = SFX_CHANNELS;     break;
        default:             start = 0; count = 0; break;
    }
}


/************************************************************************
*    DESC:  Allocate the next channel on a bus (round-robin)
************************************************************************/
int CMixEngine::allocateChannel( NDefs::EMixBus bus )
{
    uint16_t start, count;
    getBusPool( bus, start, count );

    if( count == 0 )
        return -1;

    // Round-robin: pick next channel, wrapping around
    const uint16_t offset = m_nextChannel[bus] % count;
    m_nextChannel[bus] = (offset + 1) % count;

    const int channel = start + offset;

    // Stop whatever was playing on this channel
    m_channels[channel].reset();

    return channel;
}


/************************************************************************
*    DESC:  Initialize the mix engine and open the audio device
************************************************************************/
void CMixEngine::init( uint32_t sampleRate, uint16_t channels, uint16_t bufferFrames )
{
    m_spec.sampleRate   = sampleRate;
    m_spec.channels     = channels;
    m_spec.bufferFrames = bufferFrames;

    // Reset all state
    for( auto & ch : m_channels )
        ch.reset();
    for( auto & bs : m_busState )
        bs = SBusState{};
    m_masterVolume = 1.0f;
    m_globalPaused = false;
    std::memset( m_nextChannel, 0, sizeof(m_nextChannel) );

    // Create the platform audio device and open it with our callback
    m_pDevice = CreateAudioDevice();
    m_pDevice->open( m_spec, audioCallback, this );

    NGenFunc::PostDebugMsg( NGenFunc::FormatString(
        "Mix engine initialized: %d Hz, %d ch, %d frames/buffer, %d mix channels",
        m_spec.sampleRate, m_spec.channels, m_spec.bufferFrames, TOTAL_CHANNELS ) );
}


/************************************************************************
*    DESC:  Shut down the mix engine and close the audio device
************************************************************************/
void CMixEngine::shutdown()
{
    // Stop all playback FIRST under the mutex — the audio thread may
    // still be running and sound data may already be freed.
    {
        std::lock_guard<std::mutex> lock( m_mutex );
        for( auto & ch : m_channels )
            ch.reset();
    }

    // Now safe to close the device and join the audio thread
    if( m_pDevice )
    {
        m_pDevice->close();
        m_pDevice.reset();
    }
}


/************************************************************************
*    DESC:  Play a sound on a bus. Returns channel index.
************************************************************************/
int CMixEngine::playSound( const SWavData & wavData, NDefs::EMixBus bus, int loopCount, float volume )
{
    if( !isValidBus( bus ) )
        return -1;

    std::lock_guard<std::mutex> lock( m_mutex );

    const int channel = allocateChannel( bus );
    if( channel < 0 )
        return -1;

    SMixChannel & ch = m_channels[channel];
    ch.pData       = wavData.samples.data();
    ch.frameCount  = wavData.frameCount;
    ch.position    = 0;
    ch.srcChannels = wavData.channels;
    ch.volume      = volume;
    ch.loopCount   = loopCount;
    ch.bus         = bus;
    ch.playing     = true;
    ch.paused      = false;
    ch.pausedByGlobal = false;
    ch.fadeState    = SMixChannel::FADE_NONE;
    ch.fadeVolume   = 1.0f;
    ch.fadeDelta    = 0.0f;

    return channel;
}


/************************************************************************
*    DESC:  Per-channel controls
************************************************************************/
void CMixEngine::stopChannel( int channel )
{
    if( !isValidChannel( channel ) ) return;
    std::lock_guard<std::mutex> lock( m_mutex );
    m_channels[channel].reset();
}

void CMixEngine::pauseChannel( int channel )
{
    if( !isValidChannel( channel ) ) return;
    std::lock_guard<std::mutex> lock( m_mutex );
    m_channels[channel].paused = true;
}

void CMixEngine::resumeChannel( int channel )
{
    if( !isValidChannel( channel ) ) return;
    std::lock_guard<std::mutex> lock( m_mutex );

    SMixChannel & ch = m_channels[channel];
    // Don't resume if paused by pauseAll — only resumeAll can undo that
    if( !ch.pausedByGlobal )
        ch.paused = false;
}

void CMixEngine::setChannelVolume( int channel, float volume )
{
    if( !isValidChannel( channel ) ) return;
    std::lock_guard<std::mutex> lock( m_mutex );
    m_channels[channel].volume = std::clamp( volume, 0.0f, 1.0f );
}

bool CMixEngine::isChannelPlaying( int channel ) const
{
    if( !isValidChannel( channel ) ) return false;
    std::lock_guard<std::mutex> lock( m_mutex );
    return m_channels[channel].playing && !m_channels[channel].paused;
}

bool CMixEngine::isChannelPaused( int channel ) const
{
    if( !isValidChannel( channel ) ) return false;
    std::lock_guard<std::mutex> lock( m_mutex );
    return m_channels[channel].playing && m_channels[channel].paused;
}

void CMixEngine::fadeInChannel( int channel, uint32_t durationMs )
{
    if( !isValidChannel( channel ) || durationMs == 0 ) return;
    std::lock_guard<std::mutex> lock( m_mutex );

    SMixChannel & ch = m_channels[channel];
    if( !ch.playing ) return;

    ch.fadeState  = SMixChannel::FADE_IN;
    ch.fadeVolume = 0.0f;
    ch.fadeDelta  = 1000.0f / (static_cast<float>(durationMs) * m_spec.sampleRate);
}

void CMixEngine::fadeOutChannel( int channel, uint32_t durationMs )
{
    if( !isValidChannel( channel ) || durationMs == 0 ) return;
    std::lock_guard<std::mutex> lock( m_mutex );

    SMixChannel & ch = m_channels[channel];
    if( !ch.playing ) return;

    ch.fadeState  = SMixChannel::FADE_OUT;
    // Start from current fade volume (don't reset to 1.0 if already fading)
    ch.fadeDelta  = -(ch.fadeVolume * 1000.0f) / (static_cast<float>(durationMs) * m_spec.sampleRate);
}


/************************************************************************
*    DESC:  Per-bus controls
************************************************************************/
void CMixEngine::setBusVolume( NDefs::EMixBus bus, float volume )
{
    if( !isValidBus( bus ) ) return;
    std::lock_guard<std::mutex> lock( m_mutex );
    m_busState[bus].volume = std::clamp( volume, 0.0f, 1.0f );
}

float CMixEngine::getBusVolume( NDefs::EMixBus bus ) const
{
    if( !isValidBus( bus ) ) return 0.0f;
    std::lock_guard<std::mutex> lock( m_mutex );
    return m_busState[bus].volume;
}

void CMixEngine::setBusEnabled( NDefs::EMixBus bus, bool enabled )
{
    if( !isValidBus( bus ) ) return;
    std::lock_guard<std::mutex> lock( m_mutex );
    m_busState[bus].enabled = enabled;
}

bool CMixEngine::isBusEnabled( NDefs::EMixBus bus ) const
{
    if( !isValidBus( bus ) ) return false;
    std::lock_guard<std::mutex> lock( m_mutex );
    return m_busState[bus].enabled;
}

void CMixEngine::pauseBus( NDefs::EMixBus bus )
{
    if( !isValidBus( bus ) ) return;
    std::lock_guard<std::mutex> lock( m_mutex );
    m_busState[bus].paused = true;
}

void CMixEngine::resumeBus( NDefs::EMixBus bus )
{
    if( !isValidBus( bus ) ) return;
    std::lock_guard<std::mutex> lock( m_mutex );
    m_busState[bus].paused = false;
}

void CMixEngine::stopBus( NDefs::EMixBus bus )
{
    if( !isValidBus( bus ) ) return;
    std::lock_guard<std::mutex> lock( m_mutex );

    uint16_t start, count;
    getBusPool( bus, start, count );

    for( uint16_t i = start; i < start + count; ++i )
        m_channels[i].reset();
}

bool CMixEngine::isBusPlaying( NDefs::EMixBus bus ) const
{
    if( !isValidBus( bus ) ) return false;
    std::lock_guard<std::mutex> lock( m_mutex );

    uint16_t start, count;
    getBusPool( bus, start, count );

    for( uint16_t i = start; i < start + count; ++i )
    {
        if( m_channels[i].playing && !m_channels[i].paused )
            return true;
    }
    return false;
}

bool CMixEngine::isBusPaused( NDefs::EMixBus bus ) const
{
    if( !isValidBus( bus ) ) return false;
    std::lock_guard<std::mutex> lock( m_mutex );
    return m_busState[bus].paused;
}


/************************************************************************
*    DESC:  Master volume
************************************************************************/
void CMixEngine::setMasterVolume( float volume )
{
    std::lock_guard<std::mutex> lock( m_mutex );
    m_masterVolume = std::clamp( volume, 0.0f, 1.0f );
}

float CMixEngine::getMasterVolume() const
{
    std::lock_guard<std::mutex> lock( m_mutex );
    return m_masterVolume;
}


/************************************************************************
*    DESC:  Global controls
************************************************************************/
void CMixEngine::pauseAll()
{
    std::lock_guard<std::mutex> lock( m_mutex );

    for( auto & ch : m_channels )
    {
        if( ch.playing && !ch.paused )
        {
            ch.paused = true;
            ch.pausedByGlobal = true;
        }
    }

    m_globalPaused = true;
}

void CMixEngine::resumeAll()
{
    m_globalPaused = false;

    std::lock_guard<std::mutex> lock( m_mutex );

    for( auto & ch : m_channels )
    {
        // Only resume channels that were paused by pauseAll.
        // Individually paused channels stay paused.
        if( ch.pausedByGlobal )
        {
            ch.paused = false;
            ch.pausedByGlobal = false;
        }
    }
}

void CMixEngine::stopAll()
{
    std::lock_guard<std::mutex> lock( m_mutex );

    for( auto & ch : m_channels )
        ch.reset();
}

bool CMixEngine::isPlaying() const
{
    std::lock_guard<std::mutex> lock( m_mutex );

    for( const auto & ch : m_channels )
    {
        if( ch.playing && !ch.paused )
            return true;
    }
    return false;
}


/************************************************************************
*    DESC:  Static callback trampoline — IAudioDevice calls this
************************************************************************/
void CMixEngine::audioCallback( void * pUserData, float * pBuffer, uint32_t frames )
{
    static_cast<CMixEngine*>( pUserData )->mixCallback( pBuffer, frames );
}


/************************************************************************
*    DESC:  Mix a single channel into the output buffer.
*           Uses SSE when source and device channel counts match,
*           scalar path with conversion otherwise.
************************************************************************/
void CMixEngine::mixChannel( float * pBuffer, SMixChannel & channel, float effectiveVol, uint32_t frames )
{
    const uint16_t deviceCh = m_spec.channels;
    const uint16_t srcCh    = channel.srcChannels;
    uint32_t framesWritten  = 0;

    while( framesWritten < frames && channel.playing )
    {
        const uint32_t srcRemaining = channel.frameCount - channel.position;
        const uint32_t framesToMix  = std::min( frames - framesWritten, srcRemaining );

        if( srcCh == deviceCh )
        {
            // SSE path — channel counts match, process 4 samples at a time
            const float * srcPtr = channel.pData + channel.position * srcCh;
            float * dstPtr       = pBuffer + framesWritten * deviceCh;
            const uint32_t totalSamples = framesToMix * deviceCh;
            const __m128 vol4 = _mm_set1_ps( effectiveVol );

            uint32_t i = 0;
            for( ; i + 4 <= totalSamples; i += 4 )
            {
                __m128 src = _mm_loadu_ps( srcPtr + i );
                __m128 dst = _mm_loadu_ps( dstPtr + i );
                dst = _mm_add_ps( dst, _mm_mul_ps( src, vol4 ) );
                _mm_storeu_ps( dstPtr + i, dst );
            }

            // Remainder (0–3 samples)
            for( ; i < totalSamples; ++i )
                dstPtr[i] += srcPtr[i] * effectiveVol;
        }
        else
        {
            // Scalar path — channel conversion required
            for( uint32_t i = 0; i < framesToMix; ++i )
            {
                const uint32_t srcOff = (channel.position + i) * srcCh;
                const uint32_t dstOff = (framesWritten + i) * deviceCh;

                if( srcCh == 1 && deviceCh == 2 )
                {
                    // Mono → stereo: duplicate
                    const float sample = channel.pData[srcOff] * effectiveVol;
                    pBuffer[dstOff]     += sample;
                    pBuffer[dstOff + 1] += sample;
                }
                else if( srcCh == 2 && deviceCh == 1 )
                {
                    // Stereo → mono: average
                    pBuffer[dstOff] += (channel.pData[srcOff] + channel.pData[srcOff + 1])
                                      * 0.5f * effectiveVol;
                }
                else
                {
                    // General: copy available channels, duplicate last for extras
                    for( uint16_t ch = 0; ch < deviceCh; ++ch )
                    {
                        const uint16_t srcIdx = std::min( ch, static_cast<uint16_t>(srcCh - 1) );
                        pBuffer[dstOff + ch] += channel.pData[srcOff + srcIdx] * effectiveVol;
                    }
                }
            }
        }

        framesWritten += framesToMix;
        channel.position += framesToMix;

        // Handle end-of-sound
        if( channel.position >= channel.frameCount )
        {
            if( channel.loopCount == -1 )
            {
                // Infinite loop — restart
                channel.position = 0;
            }
            else if( channel.loopCount > 0 )
            {
                // Finite loop — decrement and restart
                channel.loopCount--;
                channel.position = 0;
            }
            else
            {
                // Done
                channel.playing = false;
                channel.position = 0;
            }
        }
    }
}


/************************************************************************
*    DESC:  Instance mix callback — mixes all active channels across
*           all buses into the output buffer. Called on the audio thread.
*           Buffer is pre-zeroed (silence).
************************************************************************/
void CMixEngine::mixCallback( float * pBuffer, uint32_t frames )
{
    // Early out if globally paused (no mutex needed — atomic)
    if( m_globalPaused )
        return;

    std::lock_guard<std::mutex> lock( m_mutex );

    const uint16_t deviceCh    = m_spec.channels;
    const uint32_t totalSamples = frames * deviceCh;

    // Mix each active channel
    for( uint16_t i = 0; i < TOTAL_CHANNELS; ++i )
    {
        SMixChannel & channel = m_channels[i];

        if( !channel.playing || channel.paused )
            continue;

        const SBusState & busState = m_busState[channel.bus];

        if( !busState.enabled || busState.paused )
            continue;

        // Update fade (per buffer fill — ~23ms resolution at 44100/1024)
        if( channel.fadeState == SMixChannel::FADE_IN )
        {
            channel.fadeVolume += channel.fadeDelta * frames;
            if( channel.fadeVolume >= 1.0f )
            {
                channel.fadeVolume = 1.0f;
                channel.fadeState  = SMixChannel::FADE_NONE;
            }
        }
        else if( channel.fadeState == SMixChannel::FADE_OUT )
        {
            channel.fadeVolume += channel.fadeDelta * frames;   // fadeDelta is negative
            if( channel.fadeVolume <= 0.0f )
            {
                channel.fadeVolume = 0.0f;
                channel.fadeState  = SMixChannel::FADE_NONE;
                channel.playing    = false;
                continue;
            }
        }

        // Volume hierarchy: sound × fade × bus × master
        const float effectiveVol = channel.volume * channel.fadeVolume
                                 * busState.volume * m_masterVolume;

        if( effectiveVol < 0.0001f )
        {
            // Nearly silent — advance position without mixing to save CPU
            uint32_t remaining = frames;
            while( remaining > 0 && channel.playing )
            {
                const uint32_t srcRemaining = channel.frameCount - channel.position;
                const uint32_t toAdvance = std::min( remaining, srcRemaining );
                channel.position += toAdvance;
                remaining -= toAdvance;

                if( channel.position >= channel.frameCount )
                {
                    if( channel.loopCount == -1 )
                        channel.position = 0;
                    else if( channel.loopCount > 0 )
                    { channel.loopCount--; channel.position = 0; }
                    else
                        channel.playing = false;
                }
            }
            continue;
        }

        // Mix this channel into the output buffer
        mixChannel( pBuffer, channel, effectiveVol, frames );
    }

    // Clamp output buffer to [-1.0, 1.0] with SSE
    const __m128 minVal = _mm_set1_ps( -1.0f );
    const __m128 maxVal = _mm_set1_ps( 1.0f );

    uint32_t i = 0;
    for( ; i + 4 <= totalSamples; i += 4 )
    {
        __m128 v = _mm_loadu_ps( pBuffer + i );
        v = _mm_max_ps( v, minVal );
        v = _mm_min_ps( v, maxVal );
        _mm_storeu_ps( pBuffer + i, v );
    }

    for( ; i < totalSamples; ++i )
        pBuffer[i] = std::clamp( pBuffer[i], -1.0f, 1.0f );
}
