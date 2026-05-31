
/************************************************************************
*    FILE NAME:       sounddefs.h
*
*    DESCRIPTION:     Sound system definitions, enums, and constants
************************************************************************/

#ifndef __sound_defs_h__
#define __sound_defs_h__

// Standard lib dependencies
#include <cstdint>
#include <cstring>

namespace NDefs
{
    enum EMixBus : uint8_t
    {
        MUSIC = 0,
        AMBIENT,
        VOICE,
        SFX,
        MIX_BUS_MAX
    };

    // Convert a bus name string to the enum value
    inline EMixBus StringToBus( const char * str )
    {
        if( std::strcmp( str, "music" )   == 0 ) return MUSIC;
        if( std::strcmp( str, "ambient" ) == 0 ) return AMBIENT;
        if( std::strcmp( str, "voice" )   == 0 ) return VOICE;
        if( std::strcmp( str, "sfx" )     == 0 ) return SFX;
        return SFX;     // Default to SFX if unrecognized
    }
}

// Max volume constant (0–128 range)
constexpr int16_t MAX_SOUND_VOLUME = 128;

#endif
