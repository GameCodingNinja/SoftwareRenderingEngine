
/************************************************************************
*    FILE NAME:       mixengine.cpp
*
*    DESCRIPTION:     Audio mix engine singleton — owns the audio device,
*                     provides the mix callback, manages playback.
*                     Phase 1: single-channel WAV playback.
************************************************************************/

// Physical component dependency
#include <sound/mixengine.h>

// Game lib dependencies
#include <sound/iaudiodevice.h>
#include <sound/audiodevicefactory.h>
#include <utilities/exceptionhandling.h>
#include <utilities/genfunc.h>

// Standard lib dependencies
#include <cstring>
#include <algorithm>


/************************************************************************
*    DESC:  Constructor
************************************************************************/
CMixEngine::CMixEngine() :
    m_paused( false )
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
*    DESC:  Initialize the mix engine and open the audio device
************************************************************************/
void CMixEngine::init( uint32_t sampleRate, uint16_t channels, uint16_t bufferFrames )
{
    m_spec.sampleRate   = sampleRate;
    m_spec.channels     = channels;
    m_spec.bufferFrames = bufferFrames;

    // Create the platform audio device and open it with our callback
    m_pDevice = CreateAudioDevice();
    m_pDevice->open( m_spec, audioCallback, this );

    NGenFunc::PostDebugMsg( NGenFunc::FormatString(
        "Mix engine initialized: %d Hz, %d ch, %d frames/buffer",
        m_spec.sampleRate, m_spec.channels, m_spec.bufferFrames ) );
}


/************************************************************************
*    DESC:  Shut down the mix engine and close the audio device
************************************************************************/
void CMixEngine::shutdown()
{
    // Stop playback FIRST under the mutex — the audio thread may still be
    // running and the sound data it points to may already be freed.
    // The mutex guarantees the audio thread won't be mid-read when we null the pointer.
    {
        std::lock_guard<std::mutex> lock( m_mutex );
        m_playState = SPlayState{};
    }

    // Now safe to close the device and join the audio thread
    if( m_pDevice )
    {
        m_pDevice->close();
        m_pDevice.reset();
    }
}


/************************************************************************
*    DESC:  Play a loaded WAV sound.
*           Caller must keep wavData alive for the duration of playback.
*           loopCount: 0 = play once, N = repeat N times, -1 = infinite
************************************************************************/
void CMixEngine::playSound( const SWavData & wavData, int loopCount )
{
    std::lock_guard<std::mutex> lock( m_mutex );

    m_playState.pData      = wavData.samples.data();
    m_playState.frameCount = wavData.frameCount;
    m_playState.position   = 0;
    m_playState.channels   = wavData.channels;
    m_playState.loopCount  = loopCount;
    m_playState.playing    = true;
}


/************************************************************************
*    DESC:  Stop playback
************************************************************************/
void CMixEngine::stop()
{
    std::lock_guard<std::mutex> lock( m_mutex );

    m_playState.playing  = false;
    m_playState.position = 0;
}


/************************************************************************
*    DESC:  Pause or resume all audio
************************************************************************/
void CMixEngine::pause( bool paused )
{
    m_paused = paused;

    if( m_pDevice )
        m_pDevice->pause( paused );
}


/************************************************************************
*    DESC:  Is a sound currently playing?
************************************************************************/
bool CMixEngine::isPlaying() const
{
    return m_playState.playing;
}


/************************************************************************
*    DESC:  Static callback trampoline — IAudioDevice calls this
************************************************************************/
void CMixEngine::audioCallback( void * pUserData, float * pBuffer, uint32_t frames )
{
    static_cast<CMixEngine*>( pUserData )->mixCallback( pBuffer, frames );
}


/************************************************************************
*    DESC:  Instance mix callback — fills the output buffer with audio.
*           Called on the audio thread. Buffer is pre-zeroed (silence).
*           Handles channel conversion (mono↔stereo) and looping.
************************************************************************/
void CMixEngine::mixCallback( float * pBuffer, uint32_t frames )
{
    // If paused, leave buffer silent (already zeroed)
    if( m_paused )
        return;

    std::lock_guard<std::mutex> lock( m_mutex );

    if( !m_playState.playing || !m_playState.pData )
        return;

    const uint16_t deviceCh = m_spec.channels;
    const uint16_t srcCh    = m_playState.channels;
    uint32_t framesWritten  = 0;

    while( framesWritten < frames && m_playState.playing )
    {
        // How many source frames remain before end-of-sound
        const uint32_t srcRemaining = m_playState.frameCount - m_playState.position;
        const uint32_t framesToMix  = std::min( frames - framesWritten, srcRemaining );

        for( uint32_t i = 0; i < framesToMix; ++i )
        {
            const uint32_t srcOff = (m_playState.position + i) * srcCh;
            const uint32_t dstOff = (framesWritten + i) * deviceCh;

            if( srcCh == deviceCh )
            {
                // Channel counts match — direct copy
                for( uint16_t ch = 0; ch < deviceCh; ++ch )
                    pBuffer[dstOff + ch] += m_playState.pData[srcOff + ch];
            }
            else if( srcCh == 1 && deviceCh == 2 )
            {
                // Mono → stereo: duplicate
                const float sample = m_playState.pData[srcOff];
                pBuffer[dstOff]     += sample;
                pBuffer[dstOff + 1] += sample;
            }
            else if( srcCh == 2 && deviceCh == 1 )
            {
                // Stereo → mono: average
                pBuffer[dstOff] += (m_playState.pData[srcOff] + m_playState.pData[srcOff + 1]) * 0.5f;
            }
            else
            {
                // General fallback: copy available, duplicate last for extra channels
                for( uint16_t ch = 0; ch < deviceCh; ++ch )
                {
                    const uint16_t srcIdx = std::min( ch, static_cast<uint16_t>(srcCh - 1) );
                    pBuffer[dstOff + ch] += m_playState.pData[srcOff + srcIdx];
                }
            }
        }

        framesWritten += framesToMix;
        m_playState.position += framesToMix;

        // Handle end of sound data
        if( m_playState.position >= m_playState.frameCount )
        {
            if( m_playState.loopCount == -1 )
            {
                // Infinite loop — restart from beginning
                m_playState.position = 0;
            }
            else if( m_playState.loopCount > 0 )
            {
                // Finite loop — decrement and restart
                m_playState.loopCount--;
                m_playState.position = 0;
            }
            else
            {
                // No loops remaining — stop
                m_playState.playing = false;
            }
        }
    }
}
