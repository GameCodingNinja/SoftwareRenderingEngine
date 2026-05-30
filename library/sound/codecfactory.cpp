
/************************************************************************
*    FILE NAME:       codecfactory.cpp
*
*    DESCRIPTION:     Codec factory — auto-detects audio format by
*                     magic bytes and loads via the appropriate codec
************************************************************************/

// Physical component dependency
#include <sound/codecfactory.h>

// Game lib dependencies
#include <sound/wavcodec.h>
#include <sound/oggcodec.h>
#include <sound/mp3codec.h>
#include <sound/flaccodec.h>
#include <utilities/exceptionhandling.h>
#include <utilities/genfunc.h>

// Standard lib dependencies
#include <fstream>
#include <cstring>

namespace
{
    /************************************************************************
    *    DESC:  Supported audio format types
    ************************************************************************/
    enum EAudioFormat
    {
        FORMAT_UNKNOWN = 0,
        FORMAT_WAV,
        FORMAT_OGG,
        FORMAT_MP3,
        FORMAT_FLAC
    };


    /************************************************************************
    *    DESC:  Detect audio format from file magic bytes.
    *           Reads the first 12 bytes and matches known signatures.
    ************************************************************************/
    EAudioFormat detectFormat( const std::string & filePath )
    {
        std::ifstream file( filePath, std::ios::binary );
        if( !file.is_open() )
            return FORMAT_UNKNOWN;

        uint8_t header[12] = {};
        file.read( reinterpret_cast<char*>(header), sizeof(header) );

        // RIFF....WAVE → WAV
        if( std::memcmp( header, "RIFF", 4 ) == 0 &&
            std::memcmp( header + 8, "WAVE", 4 ) == 0 )
            return FORMAT_WAV;

        // OggS → OGG Vorbis
        if( std::memcmp( header, "OggS", 4 ) == 0 )
            return FORMAT_OGG;

        // fLaC → FLAC
        if( std::memcmp( header, "fLaC", 4 ) == 0 )
            return FORMAT_FLAC;

        // ID3 tag (ID3v2) → MP3
        if( std::memcmp( header, "ID3", 3 ) == 0 )
            return FORMAT_MP3;

        // MP3 frame sync: 0xFF followed by 0xE0-0xFF (11 sync bits set)
        if( header[0] == 0xFF && (header[1] & 0xE0) == 0xE0 )
            return FORMAT_MP3;

        return FORMAT_UNKNOWN;
    }
}


/************************************************************************
*    DESC:  Load any supported audio file, auto-detecting format
************************************************************************/
SWavData NCodecFactory::load( const std::string & filePath )
{
    const EAudioFormat format = detectFormat( filePath );

    switch( format )
    {
        case FORMAT_WAV:
            return NWavCodec::load( filePath );

        case FORMAT_OGG:
            return NOggCodec::load( filePath );

        case FORMAT_MP3:
            return NMp3Codec::load( filePath );

        case FORMAT_FLAC:
            return NFlacCodec::load( filePath );

        default:
            throw NExcept::CCriticalException( "Audio Load Error!",
                NGenFunc::FormatString( "Unrecognized audio format in file (%s).\n\n%s\nLine: %d",
                    filePath.c_str(), __FUNCTION__, __LINE__ ) );
    }
}
