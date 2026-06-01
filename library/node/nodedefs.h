
/************************************************************************
*    FILE NAME:       nodedefs.h
*
*    DESCRIPTION:     Node system definitions
************************************************************************/

#pragma once

// Standard lib dependencies
#include <cstdint>

// Node handle type
typedef uint16_t handle16_t;

// Default values
constexpr uint8_t defs_DEFAULT_NODE_ID = 0;
constexpr int16_t defs_DEFAULT_ID = -1;
constexpr handle16_t defs_DEFAULT_HANDLE = 0;

// Node type
enum class ENodeType : uint8_t
{
    _NULL_ = 0,
    OBJECT,
    SPRITE,
    UI_CONTROL,
};

// UI control subtype (for in-game UI controls attached to sprites, e.g., health bars)
enum class EControlType : uint8_t
{
    _NULL_ = 0,
    METER,
    PROGRESS_BAR,
};
