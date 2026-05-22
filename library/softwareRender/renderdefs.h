
/************************************************************************
*    FILE NAME:       renderdefs.h
*
*    DESCRIPTION:     Render type definitions and shader interface
************************************************************************/

#pragma once

// Standard lib dependencies
#include <cstdint>

// Game lib dependencies
#include <common/vertex.h>
#include <common/color.h>
#include <common/defs.h>

/************************************************************************
*    Surface data struct for the render pipeline
************************************************************************/
struct CSurfaceData
{
    uint32_t* pixels = nullptr;
    int w = 0;
    int h = 0;
};

/************************************************************************
*    Fragment shader function signature.
*    The rasterizer calls this once per pixel.
************************************************************************/
typedef bool (*FragmentShaderFunc)( uint32_t texel, uint32_t * pDBuffer, uint32_t texU, uint32_t texV, const CColor<uint32_t> & color );

/************************************************************************
*    Default fallback shader (engine-side, no game dependency)
************************************************************************/
namespace NShaderDefault
{
    inline bool shaderDefault( uint32_t texel, uint32_t * pDBuffer, uint32_t texU, uint32_t texV, const CColor<uint32_t> & color )
    {
        *pDBuffer = texel;
        return true;
    }
}
