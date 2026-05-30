
/************************************************************************
*    FILE NAME:       wasapiaudiodevice.h
*
*    DESCRIPTION:     WASAPI audio device implementation for Windows
************************************************************************/

#ifndef __wasapi_audio_device_h__
#define __wasapi_audio_device_h__

#ifdef _WIN32

// Game lib dependencies
#include <sound/iaudiodevice.h>

// Windows lib dependencies
#include <windows.h>
#include <mmdeviceapi.h>
#include <audioclient.h>

// Standard lib dependencies
#include <thread>
#include <atomic>
#include <vector>

class CWasapiAudioDevice : public IAudioDevice
{
public:

    CWasapiAudioDevice();
    ~CWasapiAudioDevice() override;

    // Open the WASAPI device in shared mode, start audio thread
    void open( const SAudioSpec & spec, AudioCallback callback, void * pUserData ) override;

    // Stop the audio thread and release all COM objects
    void close() override;

    // Pause or resume audio output (writes silence when paused)
    void pause( bool paused ) override;

private:

    // Audio thread entry point
    void audioThread();

    // WASAPI COM objects
    IAudioClient * m_pAudioClient;
    IAudioRenderClient * m_pRenderClient;

    // Event for buffer notification
    HANDLE m_hEvent;

    // MMCSS task handle for thread priority
    HANDLE m_hTask;

    // Device buffer size in frames
    uint32_t m_bufferFrameCount;

    // Callback and user data
    AudioCallback m_callback;
    void * m_pUserData;

    // Audio spec
    SAudioSpec m_spec;

    // Audio thread
    std::thread m_thread;
    std::atomic<bool> m_running;
    std::atomic<bool> m_paused;

    // Pre-allocated callback buffer
    std::vector<float> m_callbackBuffer;

    // COM initialization tracking
    bool m_comInitialized;
};

#endif // _WIN32
#endif
