
/************************************************************************
*    FILE NAME:       audiodevicefactory.h
*
*    DESCRIPTION:     Factory function to create the appropriate
*                     native audio device based on the platform.
************************************************************************/

#ifndef __audio_device_factory_h__
#define __audio_device_factory_h__

// Standard lib dependencies
#include <memory>

// Forward declaration(s)
class IAudioDevice;

// Create a native audio device for the current platform
std::unique_ptr<IAudioDevice> CreateAudioDevice();

#endif
