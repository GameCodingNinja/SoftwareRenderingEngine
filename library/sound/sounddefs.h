
/************************************************************************
*    FILE NAME:       sounddefs.h
*
*    DESCRIPTION:     Sound system definitions, enums, and constants
************************************************************************/

#ifndef __sound_defs_h__
#define __sound_defs_h__

// Standard lib dependencies
#include <cstdint>

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
}

// Max volume constant (0–128 range)
constexpr int16_t MAX_SOUND_VOLUME = 128;

#endif
