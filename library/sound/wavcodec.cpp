
/************************************************************************
*    FILE NAME:       wavcodec.cpp
*
*    DESCRIPTION:     WAV file codec — loads RIFF/WAVE files into
*                     interleaved F32 PCM data
************************************************************************/

// Physical component dependency
#include <sound/wavcodec.h>

// Game lib dependencies
#include <utilities/exceptionhandling.h>
#include <utilities/genfunc.h>

// Standard lib dependencies
#include <fstream>
#include <cstring>
#include <algorithm>

namespace
{
    /************************************************************************
    *    DESC:  RIFF/WAVE file structures
    ************************************************************************/
    struct SRiffHeader
    {
        char riffID[4];         // "RIFF"
        uint32_t fileSize;      // File size minus 8
        char waveID[4];         // "WAVE"
    };

    struct SChunkHeader
    {
        char id[4];             // Chunk identifier
        uint32_t size;          // Chunk data size
    };

    struct SFmtChunk
    {
        uint16_t audioFormat;   // 1 = PCM, 3 = IEEE float
        uint16_t channels;
        uint32_t sampleRate;
        uint32_t byteRate;
        uint16_t blockAlign;
        uint16_t bitsPerSample;
    };
}


/************************************************************************
*    DESC:  Load a WAV file and return its data as interleaved F32 PCM
************************************************************************/
SWavData NWavCodec::load( const std::string & filePath )
{
    std::ifstream file( filePath, std::ios::binary );
    if( !file.is_open() )
        throw NExcept::CCriticalException( "WAV Load Error!",
            NGenFunc::FormatString( "Failed to open WAV file (%s).\n\n%s\nLine: %d",
                filePath.c_str(), __FUNCTION__, __LINE__ ) );

    // Read and validate RIFF header
    SRiffHeader riffHeader;
    file.read( reinterpret_cast<char*>(&riffHeader), sizeof(riffHeader) );

    if( std::memcmp( riffHeader.riffID, "RIFF", 4 ) != 0 ||
        std::memcmp( riffHeader.waveID, "WAVE", 4 ) != 0 )
        throw NExcept::CCriticalException( "WAV Load Error!",
            NGenFunc::FormatString( "Not a valid WAV file (%s).\n\n%s\nLine: %d",
                filePath.c_str(), __FUNCTION__, __LINE__ ) );

    // Scan chunks for "fmt " and "data"
    SFmtChunk fmtChunk = {};
    bool foundFmt  = false;
    bool foundData = false;
    std::vector<uint8_t> rawData;

    while( file.good() && !(foundFmt && foundData) )
    {
        SChunkHeader chunkHeader;
        file.read( reinterpret_cast<char*>(&chunkHeader), sizeof(chunkHeader) );
        if( !file.good() )
            break;

        if( std::memcmp( chunkHeader.id, "fmt ", 4 ) == 0 )
        {
            const uint32_t readSize = std::min<uint32_t>( chunkHeader.size, sizeof(SFmtChunk) );
            file.read( reinterpret_cast<char*>(&fmtChunk), readSize );

            // Skip any extra format bytes (e.g., extensible format)
            if( chunkHeader.size > sizeof(SFmtChunk) )
                file.seekg( chunkHeader.size - sizeof(SFmtChunk), std::ios::cur );

            foundFmt = true;
        }
        else if( std::memcmp( chunkHeader.id, "data", 4 ) == 0 )
        {
            rawData.resize( chunkHeader.size );
            file.read( reinterpret_cast<char*>(rawData.data()), chunkHeader.size );
            foundData = true;
        }
        else
        {
            // Skip unknown chunks (LIST, fact, etc.)
            file.seekg( chunkHeader.size, std::ios::cur );

            // WAV chunks are word-aligned — skip padding byte if odd size
            if( chunkHeader.size & 1 )
                file.seekg( 1, std::ios::cur );
        }
    }

    if( !foundFmt || !foundData )
        throw NExcept::CCriticalException( "WAV Load Error!",
            NGenFunc::FormatString( "Missing fmt or data chunk in WAV file (%s).\n\n%s\nLine: %d",
                filePath.c_str(), __FUNCTION__, __LINE__ ) );

    // Validate format
    if( fmtChunk.audioFormat != 1 && fmtChunk.audioFormat != 3 )
        throw NExcept::CCriticalException( "WAV Load Error!",
            NGenFunc::FormatString( "Unsupported WAV format %d in file (%s). Only PCM and IEEE float supported.\n\n%s\nLine: %d",
                fmtChunk.audioFormat, filePath.c_str(), __FUNCTION__, __LINE__ ) );

    // Build output
    SWavData wavData;
    wavData.sampleRate = fmtChunk.sampleRate;
    wavData.channels   = fmtChunk.channels;

    const uint32_t bytesPerSample = fmtChunk.bitsPerSample / 8;
    const uint32_t totalSamples   = static_cast<uint32_t>( rawData.size() ) / bytesPerSample;
    wavData.frameCount = totalSamples / fmtChunk.channels;
    wavData.samples.resize( totalSamples );

    // Convert raw PCM to interleaved F32
    if( fmtChunk.audioFormat == 3 && fmtChunk.bitsPerSample == 32 )
    {
        // IEEE float 32-bit — direct copy
        std::memcpy( wavData.samples.data(), rawData.data(), totalSamples * sizeof(float) );
    }
    else if( fmtChunk.audioFormat == 1 )
    {
        switch( fmtChunk.bitsPerSample )
        {
            case 8:
            {
                // Unsigned 8-bit [0, 255] → [-1.0, 1.0]
                for( uint32_t i = 0; i < totalSamples; ++i )
                    wavData.samples[i] = (static_cast<float>(rawData[i]) - 128.0f) / 128.0f;
                break;
            }
            case 16:
            {
                // Signed 16-bit [-32768, 32767] → [-1.0, 1.0]
                const int16_t * pSrc = reinterpret_cast<const int16_t*>( rawData.data() );
                for( uint32_t i = 0; i < totalSamples; ++i )
                    wavData.samples[i] = static_cast<float>(pSrc[i]) / 32768.0f;
                break;
            }
            case 24:
            {
                // Signed 24-bit packed → [-1.0, 1.0]
                for( uint32_t i = 0; i < totalSamples; ++i )
                {
                    const uint32_t idx = i * 3;
                    int32_t sample = rawData[idx]
                                   | (rawData[idx + 1] << 8)
                                   | (rawData[idx + 2] << 16);

                    // Sign-extend from 24-bit to 32-bit
                    if( sample & 0x800000 )
                        sample |= 0xFF000000;

                    wavData.samples[i] = static_cast<float>(sample) / 8388608.0f;
                }
                break;
            }
            case 32:
            {
                // Signed 32-bit → [-1.0, 1.0]
                const int32_t * pSrc = reinterpret_cast<const int32_t*>( rawData.data() );
                for( uint32_t i = 0; i < totalSamples; ++i )
                    wavData.samples[i] = static_cast<float>(pSrc[i]) / 2147483648.0f;
                break;
            }
            default:
            {
                throw NExcept::CCriticalException( "WAV Load Error!",
                    NGenFunc::FormatString( "Unsupported bit depth %d in WAV file (%s).\n\n%s\nLine: %d",
                        fmtChunk.bitsPerSample, filePath.c_str(), __FUNCTION__, __LINE__ ) );
            }
        }
    }

    NGenFunc::PostDebugMsg( NGenFunc::FormatString(
        "WAV loaded: %s (%d Hz, %d ch, %d frames, %d-bit)",
        filePath.c_str(), wavData.sampleRate, wavData.channels,
        wavData.frameCount, fmtChunk.bitsPerSample ) );

    return wavData;
}


/************************************************************************
*    DESC:  Resample SWavData in-place to a target sample rate
*           using linear interpolation. No-op if rates already match.
************************************************************************/
void NAudioResample::resample( SWavData & wavData, uint32_t targetSampleRate )
{
    if( wavData.sampleRate == targetSampleRate || wavData.sampleRate == 0 || wavData.frameCount == 0 )
        return;

    const double ratio = static_cast<double>(targetSampleRate) / wavData.sampleRate;
    const uint32_t newFrameCount = static_cast<uint32_t>( wavData.frameCount * ratio );
    const uint16_t channels = wavData.channels;

    std::vector<float> newSamples( newFrameCount * channels );

    for( uint32_t i = 0; i < newFrameCount; ++i )
    {
        const double srcPos = i / ratio;
        const uint32_t srcIndex = static_cast<uint32_t>( srcPos );
        const float frac = static_cast<float>( srcPos - srcIndex );

        // Clamp next index to last valid frame
        const uint32_t srcNext = std::min( srcIndex + 1, wavData.frameCount - 1 );

        for( uint16_t ch = 0; ch < channels; ++ch )
        {
            const float s0 = wavData.samples[srcIndex * channels + ch];
            const float s1 = wavData.samples[srcNext * channels + ch];
            newSamples[i * channels + ch] = s0 + (s1 - s0) * frac;
        }
    }

    wavData.samples = std::move( newSamples );
    wavData.frameCount = newFrameCount;

    NGenFunc::PostDebugMsg( NGenFunc::FormatString(
        "Resampled: %d Hz → %d Hz (%d frames)",
        wavData.sampleRate, targetSampleRate, newFrameCount ) );

    wavData.sampleRate = targetSampleRate;
}
