
/************************************************************************
*    FILE NAME:       alsaaudiodevice.cpp
*
*    DESCRIPTION:     ALSA audio device implementation for Linux
************************************************************************/

#ifdef __linux__

// Physical component dependency
#include <sound/alsaaudiodevice.h>

// Game lib dependencies
#include <utilities/exceptionhandling.h>
#include <utilities/genfunc.h>

// Standard lib dependencies
#include <cstring>
#include <algorithm>
#include <pthread.h>
#include <sched.h>


/************************************************************************
*    DESC:  Constructor
************************************************************************/
CAlsaAudioDevice::CAlsaAudioDevice() :
    m_pPcmHandle( nullptr ),
    m_callback( nullptr ),
    m_pUserData( nullptr ),
    m_running( false ),
    m_paused( false )
{
}


/************************************************************************
*    DESC:  Destructor
************************************************************************/
CAlsaAudioDevice::~CAlsaAudioDevice()
{
    close();
}


/************************************************************************
*    DESC:  Open the ALSA PCM device
************************************************************************/
void CAlsaAudioDevice::open( const SAudioSpec & spec, AudioCallback callback, void * pUserData )
{
    m_spec = spec;
    m_callback = callback;
    m_pUserData = pUserData;

    // Try multiple ALSA device names in priority order:
    //   pipewire  — PipeWire's ALSA plugin (modern desktops)
    //   pulse     — PulseAudio's ALSA plugin
    //   default   — system default (works when dmix is configured)
    //   plughw:0  — direct hardware with automatic format conversion
    const char * deviceNames[] = { "pipewire", "pulse", "default", "plughw:0,0" };
    int rc = -1;

    for( const char * name : deviceNames )
    {
        rc = snd_pcm_open( &m_pPcmHandle, name, SND_PCM_STREAM_PLAYBACK, SND_PCM_NONBLOCK );
        if( rc >= 0 )
        {
            NGenFunc::PostDebugMsg( NGenFunc::FormatString( "ALSA: opened device '%s'", name ) );
            break;
        }
    }

    if( rc < 0 )
        throw NExcept::CCriticalException( "ALSA Audio Error!",
            NGenFunc::FormatString( "Failed to open any ALSA device: %s.\n\n%s\nLine: %d",
                snd_strerror(rc), __FUNCTION__, __LINE__ ) );

    // Configure hardware parameters
    snd_pcm_hw_params_t * hwparams;
    snd_pcm_hw_params_alloca( &hwparams );
    snd_pcm_hw_params_any( m_pPcmHandle, hwparams );

    snd_pcm_hw_params_set_access( m_pPcmHandle, hwparams, SND_PCM_ACCESS_RW_INTERLEAVED );
    snd_pcm_hw_params_set_format( m_pPcmHandle, hwparams, SND_PCM_FORMAT_S16_LE );

    unsigned int rate = m_spec.sampleRate;
    snd_pcm_hw_params_set_rate_near( m_pPcmHandle, hwparams, &rate, nullptr );
    m_spec.sampleRate = rate;

    unsigned int channels = m_spec.channels;
    snd_pcm_hw_params_set_channels_near( m_pPcmHandle, hwparams, &channels );
    m_spec.channels = static_cast<uint16_t>( channels );

    snd_pcm_uframes_t periodSize = m_spec.bufferFrames;
    snd_pcm_hw_params_set_period_size_near( m_pPcmHandle, hwparams, &periodSize, nullptr );
    m_spec.bufferFrames = static_cast<uint16_t>( periodSize );

    // Double-buffer: 2 periods minimum
    unsigned int periods = 2;
    snd_pcm_hw_params_set_periods_near( m_pPcmHandle, hwparams, &periods, nullptr );

    rc = snd_pcm_hw_params( m_pPcmHandle, hwparams );
    if( rc < 0 )
        throw NExcept::CCriticalException( "ALSA Audio Error!",
            NGenFunc::FormatString( "Failed to set ALSA hw params: %s.\n\n%s\nLine: %d",
                snd_strerror(rc), __FUNCTION__, __LINE__ ) );

    // Configure software parameters
    snd_pcm_sw_params_t * swparams;
    snd_pcm_sw_params_alloca( &swparams );
    snd_pcm_sw_params_current( m_pPcmHandle, swparams );
    snd_pcm_sw_params_set_avail_min( m_pPcmHandle, swparams, periodSize );
    snd_pcm_sw_params_set_start_threshold( m_pPcmHandle, swparams, 1 );

    rc = snd_pcm_sw_params( m_pPcmHandle, swparams );
    if( rc < 0 )
        throw NExcept::CCriticalException( "ALSA Audio Error!",
            NGenFunc::FormatString( "Failed to set ALSA sw params: %s.\n\n%s\nLine: %d",
                snd_strerror(rc), __FUNCTION__, __LINE__ ) );

    // Switch to blocking mode for playback
    snd_pcm_nonblock( m_pPcmHandle, 0 );

    // Pre-allocate buffers (never reallocated — audio thread is allocation-free)
    const uint32_t totalSamples = m_spec.bufferFrames * m_spec.channels;
    m_callbackBuffer.resize( totalSamples );
    m_deviceBuffer.resize( totalSamples );

    // Start audio thread
    m_running = true;
    m_paused = false;
    m_thread = std::thread( &CAlsaAudioDevice::audioThread, this );

    NGenFunc::PostDebugMsg( NGenFunc::FormatString(
        "ALSA audio device opened: %d Hz, %d ch, %d frames/period",
        m_spec.sampleRate, m_spec.channels, m_spec.bufferFrames ) );
}


/************************************************************************
*    DESC:  Stop the audio thread and close the PCM device
************************************************************************/
void CAlsaAudioDevice::close()
{
    if( m_running )
    {
        m_running = false;

        if( m_thread.joinable() )
            m_thread.join();
    }

    if( m_pPcmHandle )
    {
        snd_pcm_drop( m_pPcmHandle );
        snd_pcm_close( m_pPcmHandle );
        m_pPcmHandle = nullptr;
    }

    m_callbackBuffer.clear();
    m_deviceBuffer.clear();
}


/************************************************************************
*    DESC:  Pause or resume audio output
************************************************************************/
void CAlsaAudioDevice::pause( bool paused )
{
    m_paused = paused;
}


/************************************************************************
*    DESC:  Audio thread — waits for device, fills buffer, writes
************************************************************************/
void CAlsaAudioDevice::audioThread()
{
    // Attempt to set real-time thread priority (best effort)
    sched_param param;
    param.sched_priority = sched_get_priority_max( SCHED_FIFO );
    if( pthread_setschedparam( pthread_self(), SCHED_FIFO, &param ) != 0 )
        NGenFunc::PostDebugMsg( "ALSA: Could not set real-time thread priority (not running as root)" );

    const uint32_t totalSamples = m_spec.bufferFrames * m_spec.channels;

    while( m_running )
    {
        // Wait for device to be ready (100ms timeout for clean shutdown)
        int rc = snd_pcm_wait( m_pPcmHandle, 100 );
        if( rc == 0 )
            continue;   // Timeout — check m_running and retry

        if( rc < 0 )
        {
            rc = snd_pcm_recover( m_pPcmHandle, rc, 0 );
            if( rc < 0 )
            {
                NGenFunc::PostDebugMsg( NGenFunc::FormatString(
                    "ALSA wait error: %s", snd_strerror(rc) ) );
                break;
            }
            continue;
        }

        // Fill the buffer
        if( m_paused )
        {
            std::memset( m_deviceBuffer.data(), 0, totalSamples * sizeof(int16_t) );
        }
        else
        {
            // Zero the callback buffer, then let the mix engine fill it
            std::memset( m_callbackBuffer.data(), 0, totalSamples * sizeof(float) );

            if( m_callback )
                m_callback( m_pUserData, m_callbackBuffer.data(), m_spec.bufferFrames );

            // Convert F32 [-1.0, 1.0] to S16 [-32767, 32767]
            for( uint32_t i = 0; i < totalSamples; ++i )
            {
                float sample = std::clamp( m_callbackBuffer[i], -1.0f, 1.0f );
                m_deviceBuffer[i] = static_cast<int16_t>( sample * 32767.0f );
            }
        }

        // Write to ALSA device (may require multiple writes on partial completion)
        int16_t * pBuf = m_deviceBuffer.data();
        snd_pcm_uframes_t remaining = m_spec.bufferFrames;

        while( remaining > 0 && m_running )
        {
            snd_pcm_sframes_t written = snd_pcm_writei( m_pPcmHandle, pBuf, remaining );
            if( written < 0 )
            {
                written = snd_pcm_recover( m_pPcmHandle, static_cast<int>(written), 0 );
                if( written < 0 )
                {
                    NGenFunc::PostDebugMsg( NGenFunc::FormatString(
                        "ALSA write error: %s", snd_strerror(static_cast<int>(written)) ) );
                    break;
                }
                continue;
            }

            pBuf += written * m_spec.channels;
            remaining -= written;
        }
    }
}

#endif // __linux__
