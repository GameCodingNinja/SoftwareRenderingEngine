
/************************************************************************
*    FILE NAME:       audiodevicefactory.cpp
*
*    DESCRIPTION:     Factory function to create the appropriate
*                     native audio device based on the platform.
************************************************************************/

// Physical component dependency
#include <sound/audiodevicefactory.h>

// Game lib dependencies
#include <sound/iaudiodevice.h>
#include <utilities/genfunc.h>
#include <utilities/exceptionhandling.h>

#ifdef __linux__
    #include <sound/alsaaudiodevice.h>
#endif

#ifdef _WIN32
    #include <sound/wasapiaudiodevice.h>
#endif


/************************************************************************
*    DESC:  Create a native audio device for the current platform
************************************************************************/
std::unique_ptr<IAudioDevice> CreateAudioDevice()
{
    #ifdef __linux__

    NGenFunc::PostDebugMsg( "Audio system: ALSA" );
    return std::make_unique<CAlsaAudioDevice>();

    #elif defined(_WIN32)

    NGenFunc::PostDebugMsg( "Audio system: WASAPI" );
    return std::make_unique<CWasapiAudioDevice>();

    #else

    throw NExcept::CCriticalException( "Audio Device Error!",
        "Unsupported platform for audio." );

    #endif
}
