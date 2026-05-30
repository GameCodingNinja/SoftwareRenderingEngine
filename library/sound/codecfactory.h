
/************************************************************************
*    FILE NAME:       codecfactory.h
*
*    DESCRIPTION:     Codec factory — auto-detects audio format by
*                     magic bytes and loads via the appropriate codec
************************************************************************/

#ifndef __codec_factory_h__
#define __codec_factory_h__

// Game lib dependencies
#include <sound/wavcodec.h>

// Standard lib dependencies
#include <string>

namespace NCodecFactory
{
    // Load any supported audio file (WAV, OGG, MP3, FLAC).
    // Format is detected automatically from the file's magic bytes.
    SWavData load( const std::string & filePath );
}

#endif
