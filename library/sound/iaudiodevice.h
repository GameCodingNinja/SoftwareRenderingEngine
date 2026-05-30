
/************************************************************************
*    FILE NAME:       iaudiodevice.h
*
*    DESCRIPTION:     Pure virtual audio device interface
************************************************************************/

#ifndef __iaudio_device_h__
#define __iaudio_device_h__

// Game lib dependencies
#include <sound/audiospec.h>

/// Audio callback function pointer.
/// Called by the audio thread to request mixed audio data.
/// @param pUserData  User-provided context (typically CMixEngine*)
/// @param pBuffer    Pre-zeroed interleaved F32 buffer to fill
/// @param frames     Number of frames requested (total samples = frames * channels)
using AudioCallback = void(*)(void* pUserData, float* pBuffer, uint32_t frames);

class IAudioDevice
{
public:

    virtual ~IAudioDevice() = default;

    // Open the audio device, start the audio thread
    virtual void open( const SAudioSpec & spec, AudioCallback callback, void * pUserData ) = 0;

    // Close the audio device, stop the audio thread
    virtual void close() = 0;

    // Pause or resume audio output
    virtual void pause( bool paused ) = 0;
};

#endif
