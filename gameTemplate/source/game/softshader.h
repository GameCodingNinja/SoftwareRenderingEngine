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
    inline bool shaderDefault( uint32_t texel, uint32_t * pDBuffer, uint32_t texU, uint32_t texV, const CColor<uint32_t> & color )
    {
        *pDBuffer = texel;
        return true;
    }

    // Color modulation: multiply each channel by the uniform color
    inline bool shaderColorMod( uint32_t texel, uint32_t * pDBuffer, uint32_t texU, uint32_t texV, const CColor<uint32_t> & color )
    {
        uint32_t r = ((texel >> 16) & 0xFF) * color.r / 255;
        uint32_t g = ((texel >>  8) & 0xFF) * color.g / 255;
        uint32_t b = ( texel        & 0xFF) * color.b / 255;
        *pDBuffer = (255u << 24) | (r << 16) | (g << 8) | b;
        return true;
    }

    // Alpha test: only write fully opaque texels
    inline bool shaderAlphaTest( uint32_t texel, uint32_t * pDBuffer, uint32_t texU, uint32_t texV, const CColor<uint32_t> & color )
    {
        if( ((texel >> 24) & 0xFF) != 255 )
            return false;

        *pDBuffer = texel;
        return true;
    }

    // Color modulation + alpha test
    inline bool shaderColorModAlpha( uint32_t texel, uint32_t * pDBuffer, uint32_t texU, uint32_t texV, const CColor<uint32_t> & color )
    {
        if( ((texel >> 24) & 0xFF) != 255 )
            return false;

        uint32_t r = ((texel >> 16) & 0xFF) * color.r / 255;
        uint32_t g = ((texel >>  8) & 0xFF) * color.g / 255;
        uint32_t b = ( texel        & 0xFF) * color.b / 255;
        *pDBuffer = (255u << 24) | (r << 16) | (g << 8) | b;
        return true;
    }

    // Example custom shader: grayscale
    inline bool shaderGrayscale( uint32_t texel, uint32_t * pDBuffer, uint32_t texU, uint32_t texV, const CColor<uint32_t> & color )
    {
        uint32_t r = (texel >> 16) & 0xFF;
        uint32_t g = (texel >>  8) & 0xFF;
        uint32_t b =  texel        & 0xFF;
        uint32_t gray = (r * 77 + g * 150 + b * 29) >> 8;
        *pDBuffer = (255u << 24) | (gray << 16) | (gray << 8) | gray;
        return true;
    }

    // Alpha blend: src * alpha + dst * (1 - alpha)
    inline bool shaderAlphaBlend( uint32_t texel, uint32_t * pDBuffer, uint32_t texU, uint32_t texV, const CColor<uint32_t> & color )
    {
        uint32_t srcA = (texel >> 24) & 0xFF;

        // Fully transparent — skip
        if( srcA == 0 )
            return false;

        // Fully opaque — no blending needed
        if( srcA == 255 )
        {
            *pDBuffer = texel;
            return true;
        }

        // Blend: out = src * srcA/255 + dst * (255 - srcA)/255
        uint32_t invA = 255 - srcA;
        uint32_t dstColor = *pDBuffer;

        uint32_t srcR = (texel >> 16) & 0xFF;
        uint32_t srcG = (texel >>  8) & 0xFF;
        uint32_t srcB =  texel        & 0xFF;

        uint32_t dstR = (dstColor >> 16) & 0xFF;
        uint32_t dstG = (dstColor >>  8) & 0xFF;
        uint32_t dstB =  dstColor        & 0xFF;

        uint32_t r = (srcR * srcA + dstR * invA) / 255;
        uint32_t g = (srcG * srcA + dstG * invA) / 255;
        uint32_t b = (srcB * srcA + dstB * invA) / 255;

        *pDBuffer = (255u << 24) | (r << 16) | (g << 8) | b;
        return true;
    }

}  // namespace NShader
