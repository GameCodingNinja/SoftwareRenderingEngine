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

    // Color modulation
    inline bool shaderColorModulation( uint32_t texel, uint32_t * pDBuffer, uint32_t texU, uint32_t texV, const CColor<uint32_t> & color )
    {
        if( color.r != 255 || color.g != 255 || color.b != 255 || color.a != 255 )
        {
            // (product + 1 + (product >> 8)) >> 8 approximates product / 255
            uint32_t pr = ((texel >> 16) & 0xFF) * color.r;
            uint32_t pg = ((texel >>  8) & 0xFF) * color.g;
            uint32_t pb = ( texel        & 0xFF) * color.b;
            uint32_t r = (pr + 1 + (pr >> 8)) >> 8;
            uint32_t g = (pg + 1 + (pg >> 8)) >> 8;
            uint32_t b = (pb + 1 + (pb >> 8)) >> 8;
            *pDBuffer = (255u << 24) | (r << 16) | (g << 8) | b;
        }
        else
        {
            *pDBuffer = texel;
        }

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

        uint32_t pr = ((texel >> 16) & 0xFF) * srcA + ((dstColor >> 16) & 0xFF) * invA;
        uint32_t pg = ((texel >>  8) & 0xFF) * srcA + ((dstColor >>  8) & 0xFF) * invA;
        uint32_t pb = ( texel        & 0xFF) * srcA + ( dstColor        & 0xFF) * invA;

        uint32_t r = (pr + 1 + (pr >> 8)) >> 8;
        uint32_t g = (pg + 1 + (pg >> 8)) >> 8;
        uint32_t b = (pb + 1 + (pb >> 8)) >> 8;

        *pDBuffer = (255u << 24) | (r << 16) | (g << 8) | b;
        return true;
    }

    // Color modulation + alpha blend using color.a as the blend factor
    inline bool shaderColorAlphaBlend( uint32_t texel, uint32_t * pDBuffer, uint32_t texU, uint32_t texV, const CColor<uint32_t> & color )
    {
        if( color.a == 0 )
            return false;

        // Color modulate the texel
        uint32_t mr = ((texel >> 16) & 0xFF) * color.r;
        uint32_t mg = ((texel >>  8) & 0xFF) * color.g;
        uint32_t mb = ( texel        & 0xFF) * color.b;
        uint32_t srcR = (mr + 1 + (mr >> 8)) >> 8;
        uint32_t srcG = (mg + 1 + (mg >> 8)) >> 8;
        uint32_t srcB = (mb + 1 + (mb >> 8)) >> 8;

        if( color.a == 255 )
        {
            *pDBuffer = (255u << 24) | (srcR << 16) | (srcG << 8) | srcB;
            return true;
        }

        // Blend modulated texel with destination
        uint32_t invA = 255 - color.a;
        uint32_t dstColor = *pDBuffer;

        uint32_t pr = srcR * color.a + ((dstColor >> 16) & 0xFF) * invA;
        uint32_t pg = srcG * color.a + ((dstColor >>  8) & 0xFF) * invA;
        uint32_t pb = srcB * color.a + ( dstColor        & 0xFF) * invA;

        uint32_t r = (pr + 1 + (pr >> 8)) >> 8;
        uint32_t g = (pg + 1 + (pg >> 8)) >> 8;
        uint32_t b = (pb + 1 + (pb >> 8)) >> 8;

        *pDBuffer = (255u << 24) | (r << 16) | (g << 8) | b;
        return true;
    }

}
