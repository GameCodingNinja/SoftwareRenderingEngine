/************************************************************************
*    FILE NAME:       softshader.h
*
*    DESCRIPTION:     Software shader interface and built-in shaders.
*                     A shader is a function called per-pixel during
*                     scanline rasterization.
************************************************************************/

#pragma once

#include <softwareRender/renderdefs.h>

/************************************************************************
*    Built-in shaders
************************************************************************/
namespace NShader
{
    // Default: write the texel as-is
    inline void shaderDefault( const SFragIn & in, const SShaderUniforms & uniforms, SFragOut & out )
    {
        out.color = in.texel;
        out.write = true;
    }

    // Color modulation: multiply each channel by the uniform color
    inline void shaderColorMod( const SFragIn & in, const SShaderUniforms & uniforms, SFragOut & out )
    {
        uint32_t r = ((in.texel >> 16) & 0xFF) * uniforms.cr / 255;
        uint32_t g = ((in.texel >>  8) & 0xFF) * uniforms.cg / 255;
        uint32_t b = ( in.texel        & 0xFF) * uniforms.cb / 255;
        out.color = (255u << 24) | (r << 16) | (g << 8) | b;
        out.write = true;
    }

    // Alpha test: only write fully opaque texels
    inline void shaderAlphaTest( const SFragIn & in, const SShaderUniforms & uniforms, SFragOut & out )
    {
        out.color = in.texel;
        out.write = ((in.texel >> 24) & 0xFF) == 255;
    }

    // Color modulation + alpha test
    inline void shaderColorModAlpha( const SFragIn & in, const SShaderUniforms & uniforms, SFragOut & out )
    {
        if( ((in.texel >> 24) & 0xFF) != 255 )
        {
            out.write = false;
            return;
        }

        uint32_t r = ((in.texel >> 16) & 0xFF) * uniforms.cr / 255;
        uint32_t g = ((in.texel >>  8) & 0xFF) * uniforms.cg / 255;
        uint32_t b = ( in.texel        & 0xFF) * uniforms.cb / 255;
        out.color = (255u << 24) | (r << 16) | (g << 8) | b;
        out.write = true;
    }

    // Example custom shader: grayscale
    inline void shaderGrayscale( const SFragIn & in, const SShaderUniforms & uniforms, SFragOut & out )
    {
        uint32_t r = (in.texel >> 16) & 0xFF;
        uint32_t g = (in.texel >>  8) & 0xFF;
        uint32_t b =  in.texel        & 0xFF;
        uint32_t gray = (r * 77 + g * 150 + b * 29) >> 8;  // luminance weights
        out.color = (255u << 24) | (gray << 16) | (gray << 8) | gray;
        out.write = true;
    }

    // Alpha blend: src * alpha + dst * (1 - alpha)
    inline void shaderAlphaBlend( const SFragIn & in, const SShaderUniforms & uniforms, SFragOut & out )
    {
        uint32_t srcA = (in.texel >> 24) & 0xFF;

        // Fully transparent — skip
        if( srcA == 0 )
        {
            out.write = false;
            return;
        }

        // Fully opaque — no blending needed
        if( srcA == 255 )
        {
            out.color = in.texel;
            out.write = true;
            return;
        }

        // Blend: out = src * srcA/255 + dst * (255 - srcA)/255
        uint32_t invA = 255 - srcA;

        uint32_t srcR = (in.texel >> 16) & 0xFF;
        uint32_t srcG = (in.texel >>  8) & 0xFF;
        uint32_t srcB =  in.texel        & 0xFF;

        uint32_t dstR = (in.dstColor >> 16) & 0xFF;
        uint32_t dstG = (in.dstColor >>  8) & 0xFF;
        uint32_t dstB =  in.dstColor        & 0xFF;

        uint32_t r = (srcR * srcA + dstR * invA) / 255;
        uint32_t g = (srcG * srcA + dstG * invA) / 255;
        uint32_t b = (srcB * srcA + dstB * invA) / 255;

        out.color = (255u << 24) | (r << 16) | (g << 8) | b;
        out.write = true;
    }

}  // namespace NShader
