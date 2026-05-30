
/************************************************************************
*    FILE NAME:       flaccodec.cpp
*
*    DESCRIPTION:     FLAC codec — loads FLAC files into
*                     interleaved F32 PCM data via dr_flac
************************************************************************/

// Physical component dependency
#include <sound/flaccodec.h>

// Game lib dependencies
#include <utilities/exceptionhandling.h>
#include <utilities/genfunc.h>

// dr_flac — header-only FLAC decoder (public domain)
#define DR_FLAC_IMPLEMENTATION
#include "dr_libs/dr_flac.h"


/************************************************************************
*    DESC:  Load a FLAC file as interleaved F32 PCM
************************************************************************/
SWavData NFlacCodec::load( const std::string & filePath )
{
    unsigned int channels = 0;
    unsigned int sampleRate = 0;
    drflac_uint64 totalFrames = 0;

    // Decode entire file to interleaved F32 in one call
    float * pSamples = drflac_open_file_and_read_pcm_frames_f32(
        filePath.c_str(), &channels, &sampleRate, &totalFrames, nullptr );

    if( !pSamples )
        throw NExcept::CCriticalException( "FLAC Load Error!",
            NGenFunc::FormatString( "Failed to decode FLAC file (%s).\n\n%s\nLine: %d",
                filePath.c_str(), __FUNCTION__, __LINE__ ) );

    // Build output
    SWavData wavData;
    wavData.sampleRate = sampleRate;
    wavData.channels   = static_cast<uint16_t>( channels );
    wavData.frameCount = static_cast<uint32_t>( totalFrames );

    // Copy from dr_flac's malloc'd buffer to our vector
    const uint64_t totalSamples = totalFrames * channels;
    wavData.samples.assign( pSamples, pSamples + totalSamples );

    // Free dr_flac's buffer
    drflac_free( pSamples, nullptr );

    NGenFunc::PostDebugMsg( NGenFunc::FormatString(
        "FLAC loaded: %s (%d Hz, %d ch, %d frames)",
        filePath.c_str(), wavData.sampleRate, wavData.channels, wavData.frameCount ) );

    return wavData;
}
