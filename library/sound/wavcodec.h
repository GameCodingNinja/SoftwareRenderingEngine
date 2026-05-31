
/************************************************************************
*    FILE NAME:       wavcodec.h
*
*    DESCRIPTION:     WAV file codec — loads RIFF/WAVE files into
*                     interleaved F32 PCM data
************************************************************************/

#ifndef __wav_codec_h__
#define __wav_codec_h__

// Standard lib dependencies
#include <vector>
#include <string>
#include <cstdint>

// Loaded WAV data in F32 format
struct SWavData
{
    std::vector<float> samples;     // Interleaved F32 PCM [-1.0, 1.0]
    uint32_t sampleRate = 0;
    uint16_t channels   = 0;
    uint32_t frameCount = 0;        // Total frames (samples.size() / channels)
};

namespace NWavCodec
{
    // Load a WAV file and return its data as interleaved F32 PCM.
    // Supports PCM 8/16/24/32-bit and IEEE float 32-bit.
    SWavData load( const std::string & filePath );
}

namespace NAudioResample
{
    // Resample SWavData in-place to a target sample rate using linear interpolation.
    // No-op if the sample rate already matches.
    void resample( SWavData & wavData, uint32_t targetSampleRate );
}

#endif
