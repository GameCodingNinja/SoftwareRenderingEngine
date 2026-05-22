
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
*    Fragment shader input — what the rasterizer provides per pixel
************************************************************************/
struct SFragIn
{
    // Sampled texel from the bound texture (ARGB packed uint32)
    uint32_t texel;

    // Current destination pixel in the framebuffer (for blending)
    uint32_t dstColor;

    // Interpolated texture coordinates (pixel space, integer)
    uint32_t texU;
    uint32_t texV;
};

/************************************************************************
*    Uniform block — constant data for the entire draw call.
*    Set once per sprite, shared across all pixels.
************************************************************************/
struct SShaderUniforms
{
    // Color modulation (0-255 fixed-point, 255 = identity)
    uint32_t cr, cg, cb, ca;
};

/************************************************************************
*    Fragment shader output — what the shader produces per pixel
************************************************************************/
struct SFragOut
{
    // The final pixel color (ARGB packed uint32)
    uint32_t color;

    // Should the rasterizer write this pixel?
    bool write;
};

/************************************************************************
*    Fragment shader function signature.
*    The rasterizer calls this once per pixel.
************************************************************************/
typedef void (*FragmentShaderFunc)( const SFragIn & in, const SShaderUniforms & uniforms, SFragOut & out );

/************************************************************************
*    Default fallback shader (engine-side, no game dependency)
************************************************************************/
namespace NShaderDefault
{
    inline void shaderDefault( const SFragIn & in, const SShaderUniforms & uniforms, SFragOut & out )
    {
        out.color = in.texel;
        out.write = true;
    }
}
