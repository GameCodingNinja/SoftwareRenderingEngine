
/************************************************************************
*    FILE NAME:       oggcodec.cpp
*
*    DESCRIPTION:     OGG Vorbis codec — loads OGG files into
*                     interleaved F32 PCM data via stb_vorbis
************************************************************************/

// Physical component dependency
#include <sound/oggcodec.h>

// Game lib dependencies
#include <utilities/exceptionhandling.h>
#include <utilities/genfunc.h>

// stb_vorbis — header-only OGG Vorbis decoder (public domain)
#include "stb/stb_vorbis.c"


/************************************************************************
*    DESC:  Load an OGG Vorbis file as interleaved F32 PCM
************************************************************************/
SWavData NOggCodec::load( const std::string & filePath )
{
    int error = 0;
    stb_vorbis * vorbis = stb_vorbis_open_filename( filePath.c_str(), &error, nullptr );
    if( !vorbis )
        throw NExcept::CCriticalException( "OGG Load Error!",
            NGenFunc::FormatString( "Failed to open OGG file (%s), error code %d.\n\n%s\nLine: %d",
                filePath.c_str(), error, __FUNCTION__, __LINE__ ) );

    // Get stream info
    stb_vorbis_info info = stb_vorbis_get_info( vorbis );
    const uint32_t totalFrames = stb_vorbis_stream_length_in_samples( vorbis );

    // Build output
    SWavData wavData;
    wavData.sampleRate = info.sample_rate;
    wavData.channels   = static_cast<uint16_t>( info.channels );
    wavData.frameCount = totalFrames;
    wavData.samples.resize( totalFrames * info.channels );

    // Decode directly to interleaved F32
    const int framesDecoded = stb_vorbis_get_samples_float_interleaved(
        vorbis, info.channels, wavData.samples.data(),
        static_cast<int>( wavData.samples.size() ) );

    // Adjust if fewer frames were decoded than expected
    if( static_cast<uint32_t>(framesDecoded) < totalFrames )
    {
        wavData.frameCount = static_cast<uint32_t>( framesDecoded );
        wavData.samples.resize( framesDecoded * info.channels );
    }

    stb_vorbis_close( vorbis );

    NGenFunc::PostDebugMsg( NGenFunc::FormatString(
        "OGG loaded: %s (%d Hz, %d ch, %d frames)",
        filePath.c_str(), wavData.sampleRate, wavData.channels, wavData.frameCount ) );

    return wavData;
}
