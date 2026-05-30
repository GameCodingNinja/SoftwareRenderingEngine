
/************************************************************************
*    FILE NAME:       flaccodec.h
*
*    DESCRIPTION:     FLAC codec — loads FLAC files into
*                     interleaved F32 PCM data via dr_flac
************************************************************************/

#ifndef __flac_codec_h__
#define __flac_codec_h__

// Game lib dependencies
#include <sound/wavcodec.h>

// Standard lib dependencies
#include <string>

namespace NFlacCodec
{
    // Load a FLAC file and return its data as interleaved F32 PCM
    SWavData load( const std::string & filePath );
}

#endif
