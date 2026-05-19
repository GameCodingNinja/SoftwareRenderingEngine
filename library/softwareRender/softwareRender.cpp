
/************************************************************************
*    FILE NAME:       softwareRender.cpp
*
*    DESCRIPTION:     Software Rendering device
************************************************************************/

// Physical component dependency
#include <softwareRender/softwareRender.h>

// Standard lib dependencies
#include <cstring>

// Game lib dependencies
#include <common/matrix.h>
#include <utilities/threadpool.h>
#include <utilities/genfunc.h>
#include <utilities/exceptionhandling.h>
#include <utilities/deletefuncs.h>
#include <softwareRender/srtexture.h>
#include <softwareRender/triangleslope.h>

#include <system/iframebuffer.h>

// Render a single triangle within a screen strip
void RenderTriStrip2d( const CRender2d & render, int yMin, int yMax );

// Render all triangles within a screen strip
void RenderStrip2d( const std::vector<CRender2d> * pTriList, int yMin, int yMax );

// Render a single 3D triangle within a screen strip
void RenderTriStrip3d( const CRender3d & render, int yMin, int yMax );

// Render all 3D triangles within a screen strip
void RenderStrip3d( const std::vector<CRender3d> * pTriList, int yMin, int yMax );

/************************************************************************
*    desc:  Constructor
************************************************************************/
CSoftwareRender::CSoftwareRender() :
    m_textIdInc(0),
    m_vboIdInc(0),
    m_iboIdInc(0)
{
    // Init the thread pool if not already active
    if( !CThreadPool::Instance().isActive() )
        CThreadPool::Instance().init( 2, 0 );

}

/************************************************************************
*    desc:  destructor                                                             
************************************************************************/
CSoftwareRender::~CSoftwareRender()
{
    NDelFunc::DeleteMapPointers(m_pTextureMap);
    NDelFunc::DeleteMapArrayPointers(m_pVBOMap);
    NDelFunc::DeleteMapArrayPointers(m_pIBOMap);

}

/***************************************************************************
*   desc:  Set the surface data from a framebuffer
****************************************************************************/
void CSoftwareRender::SetSurface( IFrameBuffer * pFrameBuffer )
{
    if( pFrameBuffer == nullptr )
        throw NExcept::CCriticalException("SetSurface Error!",
            "Framebuffer pointer is null.");

    m_surfaceData.pixels = pFrameBuffer->GetPixels();
    m_surfaceData.w = pFrameBuffer->GetWidth();
    m_surfaceData.h = pFrameBuffer->GetHeight();
    m_halfScreen.w = m_surfaceData.w / 2;
    m_halfScreen.h = m_surfaceData.h / 2;

    // Allocate z-buffer for 3D rendering
    m_zBuffer.resize( m_surfaceData.w * m_surfaceData.h, 0 );

}

/***************************************************************************
*   desc:  Create a texture. The pointer is now owned by this class
****************************************************************************/
uint CSoftwareRender::CreateTexture( uchar * pData, int w, int h )
{
    ++m_textIdInc;

    m_pTextureMap.insert( std::make_pair(m_textIdInc, new CSRTexture( w, h, pData )) );

    return m_textIdInc;

}

/***************************************************************************
*   desc:  Create the VBO
****************************************************************************/
uint CSoftwareRender::CreateVBO( float * pData, uint sizeInBytes )
{
    ++m_vboIdInc;

    uint size = sizeInBytes / sizeof(float);
    auto mapIter = m_pVBOMap.insert( std::make_pair(m_vboIdInc, new float[size]) ).first;

    std::memcpy( mapIter->second, pData, sizeInBytes );

    return m_vboIdInc;

}

/***************************************************************************
*   desc:  Create the IBO
****************************************************************************/
uint CSoftwareRender::CreateIBO( uint * pData, uint sizeInBytes )
{
    ++m_iboIdInc;

    uint size = sizeInBytes / sizeof(uint);
    auto mapIter = m_pIBOMap.insert( std::make_pair(m_vboIdInc, new uint[size]) ).first;

    std::memcpy( mapIter->second, pData, sizeInBytes );

    return m_iboIdInc;

}

/***************************************************************************
*   desc:  Delete the texture
****************************************************************************/
void CSoftwareRender::DeleteTexture( uint Id )
{
    // Delete the texture if it exists
    auto mapIter = m_pTextureMap.find( Id );
    if( mapIter != m_pTextureMap.end() )
    {
        NDelFunc::Delete( mapIter->second );
        m_pTextureMap.erase( mapIter );
    }

}

/***************************************************************************
*   desc:  Delete the VBO
****************************************************************************/
void CSoftwareRender::DeleteVBO( uint Id )
{
    // Delete the texture if it exists
    auto mapIter = m_pVBOMap.find( Id );
    if( mapIter != m_pVBOMap.end() )
    {
        NDelFunc::DeleteArray( mapIter->second );
        m_pVBOMap.erase( mapIter );
    }

}

/***************************************************************************
*   desc:  Delete the IBO
****************************************************************************/
void CSoftwareRender::DeleteIBO( uint Id )
{
    // Delete the texture if it exists
    auto mapIter = m_pIBOMap.find( Id );
    if( mapIter != m_pIBOMap.end() )
    {
        NDelFunc::DeleteArray( mapIter->second );
        m_pIBOMap.erase( mapIter );
    }

}

/***************************************************************************
*   desc:  Get the texture
****************************************************************************/
CSRTexture * CSoftwareRender::GetTexture( uint Id )
{
    // Delete the texture if it exists
    auto mapIter = m_pTextureMap.find( Id );
    if( mapIter != m_pTextureMap.end() )
    {
        return mapIter->second;
    }
    else
    {
        throw NExcept::CCriticalException("Texture Find Error!",
            NGenFunc::FormatString("Unable to find texture Id (%d).\n\n%s\nLine: %d", Id, __FUNCTION__, __LINE__));
    }

    return nullptr;

}

/***************************************************************************
*   desc:  Get the VBO
****************************************************************************/
float * CSoftwareRender::GetVBO( uint Id )
{
    // Delete the texture if it exists
    auto mapIter = m_pVBOMap.find( Id );
    if( mapIter != m_pVBOMap.end() )
    {
        return mapIter->second;
    }
    else
    {
        throw NExcept::CCriticalException("VBO Find Error!",
            NGenFunc::FormatString("Unable to find VBO Id (%d).\n\n%s\nLine: %d", Id, __FUNCTION__, __LINE__));
    }

    return nullptr;

}

/***************************************************************************
*   desc:  Get the IBO
****************************************************************************/
uint * CSoftwareRender::GetIBO( uint Id )
{
    // Delete the texture if it exists
    auto mapIter = m_pIBOMap.find( Id );
    if( mapIter != m_pIBOMap.end() )
    {
        return mapIter->second;
    }
    else
    {
        throw NExcept::CCriticalException("IBO Find Error!",
            NGenFunc::FormatString("Unable to find IBO Id (%d).\n\n%s\nLine: %d", Id, __FUNCTION__, __LINE__));
    }

    return nullptr;

}

/***************************************************************************
*   Perspective Projection: ((trans.vert[0].vert.x / trans.vert[0].vert.z) * m_halfSize.w) + m_halfSize.w + 0.5f;
*   Orthographic Projection: (trans.vert[0].vert.x * m_halfSize.w) + m_halfSize.w + 0.5f;
****************************************************************************/
void CSoftwareRender::Render2D( const CMatrix & matrix, const uint vertCount, const uint indexCount, uint textId, uint vboId, uint iboId, const CColor & color, bool blendAlpha )
{
    CSRTexture * pText = GetTexture( textId );
    CVertex * pVert = (CVertex *)GetVBO( vboId );
    uint * pIBO = GetIBO( iboId );

    CVertex * pTrans = new CVertex[vertCount];

    for( uint i = 0; i < vertCount; ++i )
    {
        // Transform the verts
        matrix.Transform( pTrans[i].vert, pVert[i].vert );

        // Convert to screen coordinates
        pTrans[i].vert.x = (pTrans[i].vert.x * m_halfScreen.w) + m_halfScreen.w;
        pTrans[i].vert.y = (pTrans[i].vert.y * m_halfScreen.h) + m_halfScreen.h;

        // Transform the UV to pixel coordinates
        pTrans[i].uv.u = pVert[i].uv.u * pText->m_size.w;
        pTrans[i].uv.v = pVert[i].uv.v * pText->m_size.h;
    }

    // Convert float color (0.0-1.0) to fixed-point (0-255) once per sprite
    bool applyColor = false;
    uint32_t cr = (uint32_t)(color.r * 255.0f);
    uint32_t cg = (uint32_t)(color.g * 255.0f);
    uint32_t cb = (uint32_t)(color.b * 255.0f);
    uint32_t ca = (uint32_t)(color.a * 255.0f);

    if( cr != 255 || cg != 255 || cb != 255 || ca != 255 )
        applyColor = true;

    // Collect surviving triangles for strip-based rendering
    std::vector<CRender2d> triList;
    int triCount = indexCount / TRI;
    int vIndex(0);

    for( int i = 0; i < triCount; ++i )
    {
        CRender2d render2d( pText, &m_surfaceData, cr, cg, cb, ca, applyColor, blendAlpha );

        // Copy over the verts for this triangle
        for( int j = 0; j < TRI; ++j )
            render2d.m_vec[j] = pTrans[ pIBO[vIndex++] ];

        // Only keep visible triangles
        if( !render2d.Cull( m_surfaceData.w, m_surfaceData.h ) )
            triList.push_back( render2d );
    }

    // Dispatch strip-rendering jobs: each thread owns a horizontal
    // strip of the screen so there are no write conflicts
    if( !triList.empty() )
    {
        int screenH = m_surfaceData.h;
        size_t threads = CThreadPool::Instance().threadCount();

        if( threads > 0 )
        {
            int stripH = screenH / threads;
            std::vector<std::future<void>> futures;

            for( size_t t = 0; t < threads; ++t )
            {
                int yMin = t * stripH;
                int yMax = (t == threads - 1) ? screenH : (t + 1) * stripH;

                futures.emplace_back(
                    CThreadPool::Instance().post( RenderStrip2d, &triList, yMin, yMax ) );
            }

            for( auto & fut : futures )
                fut.get();
        }
        else
        {
            // Fallback: single-threaded
            RenderStrip2d( &triList, 0, screenH );
        }
    }

    NDelFunc::DeleteArray( pTrans );

}

/***************************************************************************
*   desc:  Render all triangles within a horizontal screen strip
****************************************************************************/
void RenderStrip2d( const std::vector<CRender2d> * pTriList, int yMin, int yMax )
{
    for( const auto & tri : *pTriList )
        RenderTriStrip2d( tri, yMin, yMax );
}

/***************************************************************************
*   desc:  Render a single triangle, only writing scanlines in [yMin, yMax)
****************************************************************************/
void RenderTriStrip2d( const CRender2d & render, int yMin, int yMax )
{
    // Early out if triangle doesn't overlap this strip
    float triYMin = render.m_vec[0].vert.y;
    float triYMax = render.m_vec[0].vert.y;
    for( int i = 1; i < TRI; ++i )
    {
        if( render.m_vec[i].vert.y < triYMin ) triYMin = render.m_vec[i].vert.y;
        if( render.m_vec[i].vert.y > triYMax ) triYMax = render.m_vec[i].vert.y;
    }
    if( triYMax < yMin || triYMin >= yMax )
        return;

    // Define all the variables up here for speed reasons.
    int xStart, xEnd, width, height, slopeCount(TRI);
    int64_t fixStepU, fixStepV, fixU, fixV;
    float u, v, stepU, stepV, step;
    uint * pDBuffer;

    // Fixed point shift amount needed for UV (64-bit for precision on x64)
    const int UV_SHIFT(32);
    const double FIX_SCALE_UV = (double)(1LL << UV_SHIFT);

    // Setup local variables for faster access to data
    uint screenW( render.m_pSurface->w );
    uint textureW( render.m_pText->m_size.w );
    uint textureH( render.m_pText->m_size.h );
    uint * pPixels = (uint *)render.m_pSurface->pixels;
    uint * pText = (uint *)render.m_pText->m_pData;

    // Calculate if we need uv plotting correction. .5 is needed for odd sizes
    double uOffset( (textureW % 2) ? 0.5 : 0.0 );
    double vOffset( (textureH % 2) ? 0.5 : 0.0 );

    // Create the range check variables
    uint uvOffsetMax = render.m_pText->m_size.w * render.m_pText->m_size.h;
    uint uvOffset;

    // Pointer-based screen bounds check (eliminates per-pixel offset increment)
    uint * pPixelsEnd = pPixels + (render.m_pSurface->w * render.m_pSurface->h);

    // Loop to find the top vert of the triangle to extablish vertex order
    int vTop(0);
    for( int i = 1; i < TRI; ++i )
        if( render.m_vec[i].vert.y < render.m_vec[vTop].vert.y )
            vTop = i;

    // Detect triangle winding order via 2D cross product.
    // If the winding is reversed (e.g. X or Y-axis flip), swap left/right slopes
    // so the rasterizer still produces positive scanline widths.
    float e1x = render.m_vec[1].vert.x - render.m_vec[0].vert.x;
    float e1y = render.m_vec[1].vert.y - render.m_vec[0].vert.y;
    float e2x = render.m_vec[2].vert.x - render.m_vec[0].vert.x;
    float e2y = render.m_vec[2].vert.y - render.m_vec[0].vert.y;

    CTriangleSlope::ESlopeType leftType  = CTriangleSlope::EST_LEFT;
    CTriangleSlope::ESlopeType rightType = CTriangleSlope::EST_RIGHT;

    if( (e1x * e2y - e1y * e2x) < 0.0f )
    {
        leftType  = CTriangleSlope::EST_RIGHT;
        rightType = CTriangleSlope::EST_LEFT;
    }

    // Init the slope class for managing the scan lines
    CTriangleSlope leftSlope( render.m_vec, vTop, leftType );
    CTriangleSlope rightSlope( render.m_vec, vTop, rightType );
    
    while( slopeCount > 0 )
    {
        // Determine if the left side needs (re)initializing
        if( leftSlope.Init() )
            --slopeCount;

        // Determine if the right side needs (re)initializing
        if( rightSlope.Init() )
            --slopeCount;

        // Will the left side or the right side define
        // the starting point of the Y
        if( leftSlope.m_length < rightSlope.m_length )
        {
            // Number of scan lines to fill
            height = leftSlope.m_length;

            // Clip to the bottom of this strip
            if( (leftSlope.y + height) >= yMax )
            {
                height = yMax - leftSlope.y;
                slopeCount = 0;
            }
        }
        else
        {
            // Number of scan lines to fill
            height = rightSlope.m_length;

            // Clip to the bottom of this strip
            if( (rightSlope.y + height) >= yMax )
            {
                height = yMax - rightSlope.y;
                slopeCount = 0;
            }
        }

        ////////////////////////////////////////////
        // The real work is done from this point on.
        // This is where we spend most of our time
        ////////////////////////////////////////////

        // Init the y index for scanline offset accumulation (avoids per-line multiply)
        int yIndex = leftSlope.y * (int)screenW;

        // Loop for the height of the slope
        while( height-- > 0 )
        {
            // Only render scanlines within this strip's range
            if( leftSlope.y >= yMin )
            {
                // Calculate initial values
                xStart = leftSlope.m_slope.vert.x;
                xEnd = rightSlope.m_slope.vert.x;
                width = xEnd - xStart;

                // Make sure we are within the bounds of the screen
                if( ( width > 0 ) && ( xEnd > 0 ) && ( xStart < (int)screenW ) && (leftSlope.y < yMax) )
                {
                    u = leftSlope.m_slope.uv.u;
                    v = leftSlope.m_slope.uv.v;

                    // Create the step amounts for the scan line
                    stepU = (rightSlope.m_slope.uv.u - u) / width;
                    stepV = (rightSlope.m_slope.uv.v - v) / width;

                    // Clip the scan-line
                    if( xStart < 0 )
                    {
                        step = -xStart;

                        u += (stepU * step);
                        v += (stepV * step);

                        xStart = 0;
                        width = xEnd;
                    }

                    if( xEnd > (int)screenW )
                    {
                        xEnd = screenW;
                        width = xEnd - xStart;
                    }

                    // Index into the starting point of the display buffers scan line
                    pDBuffer = pPixels + yIndex + xStart;

                    // Init the 64-bit fix point variables for speedy rendering
                    fixStepU = (int64_t)((double)stepU * FIX_SCALE_UV);
                    fixStepV = (int64_t)((double)stepV * FIX_SCALE_UV);
                    fixU = (int64_t)(((double)u + uOffset) * FIX_SCALE_UV);
                    fixV = (int64_t)(((double)v + vOffset) * FIX_SCALE_UV);

                    if( render.m_applyColor && render.m_blendAlpha )
                    {
                        // Color modulation + alpha test (only render fully opaque pixels)
                        uint32_t cr = render.m_cr;
                        uint32_t cg = render.m_cg;
                        uint32_t cb = render.m_cb;

                        while( width-- > 0 )
                        {
                            uvOffset = ((uint)(fixV >> UV_SHIFT) * textureW) + (uint)(fixU >> UV_SHIFT);

                            if( (uvOffset < uvOffsetMax) && (pDBuffer < pPixelsEnd) )
                            {
                                uint32_t texel = *(pText + uvOffset);

                                if( ((texel >> 24) & 0xFF) == 255 )
                                {
                                    uint32_t r = ((texel >> 16) & 0xFF) * cr / 255;
                                    uint32_t g = ((texel >>  8) & 0xFF) * cg / 255;
                                    uint32_t b = ( texel        & 0xFF) * cb / 255;
                                    *pDBuffer = (255 << 24) | (r << 16) | (g << 8) | b;
                                }
                            }

                            ++pDBuffer;
                            fixU += fixStepU;
                            fixV += fixStepV;
                        }
                    }
                    else if( render.m_applyColor )
                    {
                        // Color modulation only (no alpha blending)
                        uint32_t cr = render.m_cr;
                        uint32_t cg = render.m_cg;
                        uint32_t cb = render.m_cb;

                        while( width-- > 0 )
                        {
                            uvOffset = ((uint)(fixV >> UV_SHIFT) * textureW) + (uint)(fixU >> UV_SHIFT);

                            if( (uvOffset < uvOffsetMax) && (pDBuffer < pPixelsEnd) )
                            {
                                uint32_t texel = *(pText + uvOffset);
                                uint32_t r = ((texel >> 16) & 0xFF) * cr / 255;
                                uint32_t g = ((texel >>  8) & 0xFF) * cg / 255;
                                uint32_t b = ( texel        & 0xFF) * cb / 255;
                                *pDBuffer = (255 << 24) | (r << 16) | (g << 8) | b;
                            }

                            ++pDBuffer;
                            fixU += fixStepU;
                            fixV += fixStepV;
                        }
                    }
                    else if( render.m_blendAlpha )
                    {
                        // Alpha test only - render pixels with alpha == 255, skip the rest
                        while( width-- > 0 )
                        {
                            uvOffset = ((uint)(fixV >> UV_SHIFT) * textureW) + (uint)(fixU >> UV_SHIFT);

                            if( (uvOffset < uvOffsetMax) && (pDBuffer < pPixelsEnd) )
                            {
                                uint32_t texel = *(pText + uvOffset);

                                if( ((texel >> 24) & 0xFF) == 255 )
                                    *pDBuffer = texel;
                            }

                            ++pDBuffer;
                            fixU += fixStepU;
                            fixV += fixStepV;
                        }
                    }
                    else
                    {
                        // Fast path: no color modulation, no alpha blending
                        while( width-- > 0 )
                        {
                            uvOffset = ((uint)(fixV >> UV_SHIFT) * textureW) + (uint)(fixU >> UV_SHIFT);

                            if( (uvOffset < uvOffsetMax) && (pDBuffer < pPixelsEnd) )
                                *pDBuffer = *(pText + uvOffset);

                            ++pDBuffer;
                            fixU += fixStepU;
                            fixV += fixStepV;
                        }
                    }
                }
            }

            leftSlope.Inc();
            rightSlope.Inc();
            yIndex += screenW;
        }
    }
}

/***************************************************************************
*   desc:  Clear the z-buffer for a new frame
****************************************************************************/
void CSoftwareRender::ClearZBuffer()
{
    std::memset( m_zBuffer.data(), 0, m_zBuffer.size() * sizeof(int32_t) );
}

/***************************************************************************
*   desc:  Render 3D with perspective projection and z-buffer
*
*   Perspective Projection: ((vert.x / vert.z) * halfW) + halfW
*                           ((vert.y / vert.z) * halfH) + halfH
*   Stores 1/Z, U/Z, V/Z for perspective-correct texture mapping
****************************************************************************/
void CSoftwareRender::Render3D( const CMatrix & matrix, const uint vertCount, const uint indexCount, uint textId, uint vboId, uint iboId, const CColor & color )
{
    CSRTexture * pText = GetTexture( textId );
    CVertex * pVert = (CVertex *)GetVBO( vboId );
    uint * pIBO = GetIBO( iboId );

    CVertex * pTrans = new CVertex[vertCount];

    // Get raw matrix data for W computation
    const float * mat = matrix();

    for( uint i = 0; i < vertCount; ++i )
    {
        // Transform the verts (computes x, y, z but not w)
        matrix.Transform( pTrans[i].vert, pVert[i].vert );

        // Compute W from the projection matrix (row 3: m03, m13, m23, m33)
        float w = ( pVert[i].vert.x * mat[m03] )
                + ( pVert[i].vert.y * mat[m13] )
                + ( pVert[i].vert.z * mat[m23] )
                + mat[m33];

        // Near clip: skip verts behind the camera
        if( w <= 0.0f )
            w = 0.001f;

        // Calculate 1/W for perspective-correct interpolation and z-buffer
        float oneOverW = 1.0f / w;

        // Perspective divide and convert to screen coordinates
        pTrans[i].vert.x = (pTrans[i].vert.x * oneOverW * m_halfScreen.w) + m_halfScreen.w;
        pTrans[i].vert.y = (pTrans[i].vert.y * oneOverW * m_halfScreen.h) + m_halfScreen.h;

        // Store 1/W in vert.z for z-buffer and perspective correction
        pTrans[i].vert.z = oneOverW;

        // Convert UV to U/W and V/W for perspective-correct mapping (raw 0-1 range)
        pTrans[i].uv.u = pVert[i].uv.u * oneOverW;
        pTrans[i].uv.v = pVert[i].uv.v * oneOverW;
    }

    // Convert float color (0.0-1.0) to fixed-point (0-255) once per mesh
    bool applyColor = false;
    uint32_t cr = (uint32_t)(color.r * 255.0f);
    uint32_t cg = (uint32_t)(color.g * 255.0f);
    uint32_t cb = (uint32_t)(color.b * 255.0f);
    uint32_t ca = (uint32_t)(color.a * 255.0f);

    if( cr != 255 || cg != 255 || cb != 255 || ca != 255 )
        applyColor = true;

    // Collect surviving triangles for strip-based rendering
    std::vector<CRender3d> triList;
    int triCount = indexCount / TRI;
    int vIndex(0);

    for( int i = 0; i < triCount; ++i )
    {
        CRender3d render3d( pText, &m_surfaceData, m_zBuffer.data(), cr, cg, cb, ca, applyColor );

        // Copy over the verts for this triangle
        for( int j = 0; j < TRI; ++j )
            render3d.m_vec[j] = pTrans[ pIBO[vIndex++] ];

        // Only keep visible triangles
        if( !render3d.Cull( m_surfaceData.w, m_surfaceData.h ) )
            triList.push_back( render3d );
    }

    // Dispatch strip-rendering jobs: each thread owns a horizontal
    // strip of the screen so there are no write conflicts
    if( !triList.empty() )
    {
        int screenH = m_surfaceData.h;
        size_t threads = CThreadPool::Instance().threadCount();

        /*if( threads > 0 )
        {
            int stripH = screenH / threads;
            std::vector<std::future<void>> futures;

            for( size_t t = 0; t < threads; ++t )
            {
                int yMin = t * stripH;
                int yMax = (t == threads - 1) ? screenH : (t + 1) * stripH;

                futures.emplace_back(
                    CThreadPool::Instance().post( RenderStrip3d, &triList, yMin, yMax ) );
            }

            for( auto & fut : futures )
                fut.get();
        }
        else*/
        {
            // Fallback: single-threaded
            RenderStrip3d( &triList, 0, screenH );
        }
    }

    NDelFunc::DeleteArray( pTrans );
}

/***************************************************************************
*   desc:  Render all 3D triangles within a horizontal screen strip
****************************************************************************/
void RenderStrip3d( const std::vector<CRender3d> * pTriList, int yMin, int yMax )
{
    for( const auto & tri : *pTriList )
        RenderTriStrip3d( tri, yMin, yMax );
}

/***************************************************************************
*   desc:  Render a single 3D triangle with z-buffer and perspective-correct
*          texture mapping, only writing scanlines in [yMin, yMax).
*
*          Follows the approach from the legacy tri3D library:
*          - Edge walking with left/right slope interpolation
*          - Per-pixel z-buffer depth testing
*          - Perspective-correct texture mapping via 1/Z, U/Z, V/Z
*          - Affine subdivision every 16 pixels for speed
****************************************************************************/
void RenderTriStrip3d( const CRender3d & render, int yMin, int yMax )
{
    // Early out if triangle doesn't overlap this strip
    float triYMin = render.m_vec[0].vert.y;
    float triYMax = render.m_vec[0].vert.y;
    for( int i = 1; i < TRI; ++i )
    {
        if( render.m_vec[i].vert.y < triYMin ) triYMin = render.m_vec[i].vert.y;
        if( render.m_vec[i].vert.y > triYMax ) triYMax = render.m_vec[i].vert.y;
    }
    if( triYMax < yMin || triYMin >= yMax )
        return;

    // Define all the variables up here for speed reasons
    int xStart, xEnd, width, height, slopeCount(TRI);
    float u, v, z, stepU, stepV, stepZ;
    int64_t fixZ, fixStepZ;
    int64_t fixTx1, fixTy1;
    uint * pDBuffer;
    int32_t * pZBuffer;

    // Fixed point scale for Z-buffer (2^26 like legacy)
    const double FIX_SCALE_Z = 67108864.0; // 2^26

    // Fixed point shift for texture coordinates (32-bit fraction for precision)
    const int TEX_SHIFT(32);
    const double FIX_SCALE_TEX = (double)(1LL << TEX_SHIFT);

    // Subdivision run length
    const int RUN_LENGTH(16);
    const int RUN_SHIFT(4);

    // Setup local variables for faster access to data
    uint screenW( render.m_pSurface->w );
    uint textureW( render.m_pText->m_size.w );
    uint textureH( render.m_pText->m_size.h );
    uint * pPixels = (uint *)render.m_pSurface->pixels;
    uint * pText = (uint *)render.m_pText->m_pData;
    int32_t * pZBuf = render.m_pZBuffer;

    // Create the range check variables
    uint uvOffsetMax = textureW * textureH;

    // Pointer-based screen bounds check
    uint * pPixelsEnd = pPixels + (render.m_pSurface->w * render.m_pSurface->h);

    // Loop to find the top vert of the triangle
    int vTop(0);
    for( int i = 1; i < TRI; ++i )
        if( render.m_vec[i].vert.y < render.m_vec[vTop].vert.y )
            vTop = i;

    // Init the slope class for managing the scan lines
    CTriangleSlope leftSlope( render.m_vec, vTop, CTriangleSlope::EST_LEFT );
    CTriangleSlope rightSlope( render.m_vec, vTop, CTriangleSlope::EST_RIGHT );

    while( slopeCount > 0 )
    {
        // Determine if the left side needs (re)initializing
        if( leftSlope.Init() )
            --slopeCount;

        // Determine if the right side needs (re)initializing
        if( rightSlope.Init() )
            --slopeCount;

        // Will the left side or the right side define
        // the starting point of the Y
        if( leftSlope.m_length < rightSlope.m_length )
        {
            height = leftSlope.m_length;

            if( (leftSlope.y + height) >= yMax )
            {
                height = yMax - leftSlope.y;
                slopeCount = 0;
            }
        }
        else
        {
            height = rightSlope.m_length;

            if( (rightSlope.y + height) >= yMax )
            {
                height = yMax - rightSlope.y;
                slopeCount = 0;
            }
        }

        ////////////////////////////////////////////
        // The real work is done from this point on.
        // This is where we spend most of our time
        ////////////////////////////////////////////

        // Init the y index for scanline offset accumulation
        int yIndex = leftSlope.y * (int)screenW;

        // Loop for the height of the slope
        while( height-- > 0 )
        {
            // Only render scanlines within this strip's range
            if( leftSlope.y >= yMin )
            {
                // Calculate initial values
                xStart = leftSlope.m_slope.vert.x;
                xEnd = rightSlope.m_slope.vert.x;
                width = xEnd - xStart;

                // Make sure we are within the bounds of the screen
                if( ( width > 0 ) && ( xEnd > 0 ) && ( xStart < (int)screenW ) && (leftSlope.y < yMax) )
                {
                    // Get the interpolated 1/W and U/W, V/W at the left edge
                    z = leftSlope.m_slope.vert.z;
                    u = leftSlope.m_slope.uv.u;
                    v = leftSlope.m_slope.uv.v;

                    // Create the step amounts for the scan line
                    stepZ = (rightSlope.m_slope.vert.z - z) / width;
                    stepU = (rightSlope.m_slope.uv.u - u) / width;
                    stepV = (rightSlope.m_slope.uv.v - v) / width;

                    // Clip the scan-line
                    if( xStart < 0 )
                    {
                        float clip = -xStart;

                        z += (stepZ * clip);
                        u += (stepU * clip);
                        v += (stepV * clip);

                        xStart = 0;
                        width = xEnd;
                    }

                    if( xEnd > (int)screenW )
                    {
                        xEnd = screenW;
                        width = xEnd - xStart;
                    }

                    // Index into the starting point of the display and z buffers
                    pDBuffer = pPixels + yIndex + xStart;
                    pZBuffer = pZBuf + yIndex + xStart;

                    // Init the 64-bit fixed point Z for z-buffer testing
                    fixZ = (int64_t)(z * FIX_SCALE_Z);
                    fixStepZ = (int64_t)(stepZ * FIX_SCALE_Z);

                    ////////////////////////////////////////////
                    // Perspective-correct texture mapping with
                    // affine subdivision (16-pixel runs)
                    // Following the legacy tri3D approach
                    ////////////////////////////////////////////

                    // Calculate the first real texture coordinates
                    // U/W divided by 1/W recovers raw U (0-1), multiply by texture size
                    float realZ = 1.0f / z;
                    fixTx1 = (int64_t)((double)(u * realZ) * (double)textureW * FIX_SCALE_TEX);
                    fixTy1 = (int64_t)((double)(v * realZ) * (double)textureH * FIX_SCALE_TEX);

                    // Calculate subdivision loops (width / 16)
                    int runLoops = width >> RUN_SHIFT;

                    // Premultiply steps for subdivision
                    float subUStep = stepU * RUN_LENGTH;
                    float subVStep = stepV * RUN_LENGTH;
                    float subZStep = stepZ * RUN_LENGTH;

                    // Loop for number of subdivisions
                    for( int count = 0; count < runLoops; ++count )
                    {
                        int length = RUN_LENGTH;

                        float RZ = z;

                        // Jump ahead to next subdivision
                        u  += subUStep;
                        v  += subVStep;
                        RZ += subZStep;

                        // Calculate the next point to interpolate
                        float nextRealZ = 1.0f / RZ;
                        int64_t fixTx2 = (int64_t)((double)(u * nextRealZ) * (double)textureW * FIX_SCALE_TEX);
                        int64_t fixTy2 = (int64_t)((double)(v * nextRealZ) * (double)textureH * FIX_SCALE_TEX);

                        // Divide by 16 via shift
                        int64_t fixTxStep = (fixTx2 - fixTx1) >> RUN_SHIFT;
                        int64_t fixTyStep = (fixTy2 - fixTy1) >> RUN_SHIFT;

                        int64_t fixTx = fixTx1;
                        int64_t fixTy = fixTy1;

                        // Loop for width of scan-line
                        while( length-- > 0 )
                        {
                            if( *pZBuffer < (int32_t)fixZ && pDBuffer < pPixelsEnd )
                            {
                                uint texX = (uint)(fixTx >> TEX_SHIFT);
                                uint texY = (uint)(fixTy >> TEX_SHIFT);

                                // Clamp texture coordinates
                                if( texX >= textureW ) texX = textureW - 1;
                                if( texY >= textureH ) texY = textureH - 1;

                                uint uvOffset = texY * textureW + texX;

                                if( uvOffset < uvOffsetMax )
                                {
                                    uint32_t texel = *(pText + uvOffset);

                                    if( render.m_applyColor )
                                    {
                                        uint32_t r = ((texel >> 16) & 0xFF) * render.m_cr / 255;
                                        uint32_t g = ((texel >>  8) & 0xFF) * render.m_cg / 255;
                                        uint32_t b = ( texel        & 0xFF) * render.m_cb / 255;
                                        texel = (255 << 24) | (r << 16) | (g << 8) | b;
                                    }

                                    *pDBuffer = texel;
                                    *pZBuffer = (int32_t)fixZ;
                                }
                            }

                            fixZ += fixStepZ;
                            fixTx += fixTxStep;
                            fixTy += fixTyStep;
                            ++pDBuffer;
                            ++pZBuffer;
                        }

                        // Reuse the last calculations as the first
                        fixTx1 = fixTx2;
                        fixTy1 = fixTy2;
                    }

                    // Calculate remainder of scan-line left to rasterize
                    int length = width & (RUN_LENGTH - 1);

                    if( length > 0 )
                    {
                        float RZ = z;

                        // Jump ahead to the remainder
                        u  += (stepU * length);
                        v  += (stepV * length);
                        RZ += (stepZ * length);

                        // Calculate the next point to interpolate
                        float nextRealZ = 1.0f / RZ;
                        int64_t fixTx2 = (int64_t)((double)(u * nextRealZ) * (double)textureW * FIX_SCALE_TEX);
                        int64_t fixTy2 = (int64_t)((double)(v * nextRealZ) * (double)textureH * FIX_SCALE_TEX);

                        int64_t fixTxStep = (fixTx2 - fixTx1) / length;
                        int64_t fixTyStep = (fixTy2 - fixTy1) / length;

                        int64_t fixTx = fixTx1;
                        int64_t fixTy = fixTy1;

                        // Loop for remainder width
                        while( length-- > 0 )
                        {
                            if( *pZBuffer < (int32_t)fixZ && pDBuffer < pPixelsEnd )
                            {
                                uint texX = (uint)(fixTx >> TEX_SHIFT);
                                uint texY = (uint)(fixTy >> TEX_SHIFT);

                                // Clamp texture coordinates
                                if( texX >= textureW ) texX = textureW - 1;
                                if( texY >= textureH ) texY = textureH - 1;

                                uint uvOffset = texY * textureW + texX;

                                if( uvOffset < uvOffsetMax )
                                {
                                    uint32_t texel = *(pText + uvOffset);

                                    if( render.m_applyColor )
                                    {
                                        uint32_t r = ((texel >> 16) & 0xFF) * render.m_cr / 255;
                                        uint32_t g = ((texel >>  8) & 0xFF) * render.m_cg / 255;
                                        uint32_t b = ( texel        & 0xFF) * render.m_cb / 255;
                                        texel = (255 << 24) | (r << 16) | (g << 8) | b;
                                    }

                                    *pDBuffer = texel;
                                    *pZBuffer = (int32_t)fixZ;
                                }
                            }

                            fixZ += fixStepZ;
                            fixTx += fixTxStep;
                            fixTy += fixTyStep;
                            ++pDBuffer;
                            ++pZBuffer;
                        }
                    }
                }
            }

            leftSlope.Inc();
            rightSlope.Inc();
            yIndex += screenW;
        }
    }
}
