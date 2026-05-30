
/************************************************************************
*    FILE NAME:       oggcodec.h
*
*    DESCRIPTION:     OGG Vorbis codec — loads OGG files into
*                     interleaved F32 PCM data via stb_vorbis
************************************************************************/

#ifndef __ogg_codec_h__
#define __ogg_codec_h__

// Game lib dependencies
#include <sound/wavcodec.h>

// Standard lib dependencies
#include <string>

namespace NOggCodec
{
    // Load an OGG Vorbis file and return its data as interleaved F32 PCM
    SWavData load( const std::string & filePath );
}

#endif
