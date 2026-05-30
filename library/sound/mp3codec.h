
/************************************************************************
*    FILE NAME:       mp3codec.h
*
*    DESCRIPTION:     MP3 codec — loads MP3 files into
*                     interleaved F32 PCM data via minimp3
************************************************************************/

#ifndef __mp3_codec_h__
#define __mp3_codec_h__

// Game lib dependencies
#include <sound/wavcodec.h>

// Standard lib dependencies
#include <string>

namespace NMp3Codec
{
    // Load an MP3 file and return its data as interleaved F32 PCM
    SWavData load( const std::string & filePath );
}

#endif
