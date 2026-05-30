
/************************************************************************
*    FILE NAME:       audiospec.h
*
*    DESCRIPTION:     Audio specification struct for device configuration
************************************************************************/

#ifndef __audio_spec_h__
#define __audio_spec_h__

// Standard lib dependencies
#include <cstdint>

struct SAudioSpec
{
    uint32_t sampleRate   = 44100;
    uint16_t channels     = 2;      // 1 = mono, 2 = stereo
    uint16_t bufferFrames = 1024;   // Frames per buffer period
};

#endif
