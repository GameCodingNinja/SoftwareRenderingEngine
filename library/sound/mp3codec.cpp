
/************************************************************************
*    FILE NAME:       mp3codec.cpp
*
*    DESCRIPTION:     MP3 codec — loads MP3 files into
*                     interleaved F32 PCM data via minimp3
************************************************************************/

// Physical component dependency
#include <sound/mp3codec.h>

// Game lib dependencies
#include <utilities/exceptionhandling.h>
#include <utilities/genfunc.h>

// minimp3 — header-only MP3 decoder (CC0)
// Define MINIMP3_FLOAT_OUTPUT so decoded samples are float instead of int16
#define MINIMP3_FLOAT_OUTPUT
#define MINIMP3_IMPLEMENTATION
#include "minimp3/minimp3.h"
#define MINIMP3_EX_IMPLEMENTATION
#include "minimp3/minimp3_ex.h"


/************************************************************************
*    DESC:  Load an MP3 file as interleaved F32 PCM
************************************************************************/
SWavData NMp3Codec::load( const std::string & filePath )
{
    mp3dec_t dec;
    mp3dec_file_info_t info = {};

    int result = mp3dec_load( &dec, filePath.c_str(), &info, nullptr, nullptr );
    if( result != 0 || !info.buffer || info.samples == 0 )
    {
        if( info.buffer )
            free( info.buffer );

        throw NExcept::CCriticalException( "MP3 Load Error!",
            NGenFunc::FormatString( "Failed to decode MP3 file (%s), error %d.\n\n%s\nLine: %d",
                filePath.c_str(), result, __FUNCTION__, __LINE__ ) );
    }

    // Build output — minimp3 with MINIMP3_FLOAT_OUTPUT gives us float samples directly
    SWavData wavData;
    wavData.sampleRate = static_cast<uint32_t>( info.hz );
    wavData.channels   = static_cast<uint16_t>( info.channels );
    wavData.frameCount = static_cast<uint32_t>( info.samples / info.channels );

    // Copy from minimp3's malloc'd buffer to our vector
    wavData.samples.assign( info.buffer, info.buffer + info.samples );

    // Free minimp3's buffer
    free( info.buffer );

    NGenFunc::PostDebugMsg( NGenFunc::FormatString(
        "MP3 loaded: %s (%d Hz, %d ch, %d frames)",
        filePath.c_str(), wavData.sampleRate, wavData.channels, wavData.frameCount ) );

    return wavData;
}
