
/************************************************************************
*    FILE NAME:       alsaaudiodevice.h
*
*    DESCRIPTION:     ALSA audio device implementation for Linux
************************************************************************/

#ifndef __alsa_audio_device_h__
#define __alsa_audio_device_h__

#ifdef __linux__

// Game lib dependencies
#include <sound/iaudiodevice.h>

// Linux lib dependencies
#include <alsa/asoundlib.h>

// Standard lib dependencies
#include <thread>
#include <atomic>
#include <vector>

class CAlsaAudioDevice : public IAudioDevice
{
public:

    CAlsaAudioDevice();
    ~CAlsaAudioDevice() override;

    // Open the ALSA PCM device, configure format, start audio thread
    void open( const SAudioSpec & spec, AudioCallback callback, void * pUserData ) override;

    // Stop the audio thread and close the PCM device
    void close() override;

    // Pause or resume audio output (writes silence when paused)
    void pause( bool paused ) override;

private:

    // Audio thread entry point
    void audioThread();

    // ALSA PCM handle
    snd_pcm_t * m_pPcmHandle;

    // Callback and user data
    AudioCallback m_callback;
    void * m_pUserData;

    // Audio spec (may be adjusted by ALSA to nearest supported values)
    SAudioSpec m_spec;

    // Audio thread
    std::thread m_thread;
    std::atomic<bool> m_running;
    std::atomic<bool> m_paused;

    // Pre-allocated buffers (no heap allocation on audio thread)
    std::vector<float> m_callbackBuffer;    // F32 buffer for mix callback
    std::vector<int16_t> m_deviceBuffer;    // S16 buffer for ALSA output
};

#endif // __linux__
#endif
