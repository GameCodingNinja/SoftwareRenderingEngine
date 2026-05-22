
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
#include <common/texture.h>
#include <softwareRender/triangleslope.h>

#include <system/iframebuffer.h>
#include <utilities/settings.h>

// Render a single triangle within a screen strip
void RenderTriStrip2d( const CRender2d & render, int yMin, int yMax );

// Render all triangles within a screen strip
void RenderStrip2d( const std::vector<CRender2d> * pTriList, int yMin, int yMax );

// Render a single 3D triangle within a screen strip
void RenderTriStrip3d( const CRender3d & render, int yMin, int yMax );

// Render all 3D triangles within a screen strip
void RenderStrip3d( const std::vector<CRender3d> * pTriList, int yMin, int yMax );

// Fixed-function render functions
void RenderTriStripFixedFunction2d( const CRender2d & render, int yMin, int yMax );
void RenderStripFixedFunction2d( const std::vector<CRender2d> * pTriList, int yMin, int yMax );
void RenderTriStripFixedFunction3d( const CRender3d & render, int yMin, int yMax );
void RenderStripFixedFunction3d( const std::vector<CRender3d> * pTriList, int yMin, int yMax );

/************************************************************************
*    desc:  Constructor
************************************************************************/
CSoftwareRender::CSoftwareRender()
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
}

/***************************************************************************
*   desc:  Set the surface data from a framebuffer
****************************************************************************/
void CSoftwareRender::setSurface( IFrameBuffer * pFrameBuffer )
{
    if( pFrameBuffer == nullptr )
        throw NExcept::CCriticalException("setSurface Error!",
            "Framebuffer pointer is null.");

    m_surfaceData.pixels = pFrameBuffer->getPixels();
    m_surfaceData.w = pFrameBuffer->getWidth();
    m_surfaceData.h = pFrameBuffer->getHeight();
    m_halfScreen.w = m_surfaceData.w / 2;
    m_halfScreen.h = m_surfaceData.h / 2;

    // Allocate z-buffer for 3D rendering
    m_zBuffer.resize( m_surfaceData.w * m_surfaceData.h, 0 );
}

/***************************************************************************
*   Perspective Projection: ((trans.vert[0].vert.x / trans.vert[0].vert.z) * m_halfSize.w) + m_halfSize.w + 0.5f;
*   Orthographic Projection: (trans.vert[0].vert.x * m_halfSize.w) + m_halfSize.w + 0.5f;
****************************************************************************/
void CSoftwareRender::render2D( const CMatrix & matrix, const uint vertCount, const uint indexCount, const CTexture * pText, float * pVBO, uint * pIBO, const CColor<float> & color, FragmentShaderFunc shader )
{
    CVertex * pVert = (CVertex *)pVBO;

    CVertex * pTrans = new CVertex[vertCount];

    for( uint i = 0; i < vertCount; ++i )
    {
        // Transform the verts
        matrix.transform( pTrans[i].vert, pVert[i].vert );

        // Convert to screen coordinates
        pTrans[i].vert.x = (pTrans[i].vert.x * m_halfScreen.w) + m_halfScreen.w;
        pTrans[i].vert.y = (pTrans[i].vert.y * m_halfScreen.h) + m_halfScreen.h;

        // Transform the UV to pixel coordinates
        pTrans[i].uv.u = pVert[i].uv.u * pText->m_size.w;
        pTrans[i].uv.v = pVert[i].uv.v * pText->m_size.h;
    }

    // Convert float color (0.0-1.0) to fixed-point (0-255) once per sprite
    CColor<uint32_t> color32( (uint32_t)(color.r * 255.0f), (uint32_t)(color.g * 255.0f), (uint32_t)(color.b * 255.0f), (uint32_t)(color.a * 255.0f) );

    // Collect surviving triangles for strip-based rendering
    std::vector<CRender2d> triList;
    int triCount = indexCount / TRI;
    int vIndex(0);

    for( int i = 0; i < triCount; ++i )
    {
        CRender2d render2d( pText, &m_surfaceData, color32, shader );

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

    delete[] pTrans;
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

                    while( width-- > 0 )
                    {
                        uvOffset = ((uint)(fixV >> UV_SHIFT) * textureW) + (uint)(fixU >> UV_SHIFT);

                        if( (uvOffset < uvOffsetMax) && (pDBuffer < pPixelsEnd) )
                            render.m_shader( *(pText + uvOffset), pDBuffer, (uint)(fixU >> UV_SHIFT), (uint)(fixV >> UV_SHIFT), render.m_color );

                        ++pDBuffer;
                        fixU += fixStepU;
                        fixV += fixStepV;
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
void CSoftwareRender::clearZBuffer()
{
    std::memset( m_zBuffer.data(), 0, m_zBuffer.size() * sizeof(int32_t) );
}

/***************************************************************************
*   desc:  Render 3D with perspective projection, near-Z clipping, and z-buffer.
*          Near-Z clipping uses the Sutherland-Hodgman algorithm against
*          the near plane (W >= nearClip), following the legacy tri3D approach.
*          Clipping is done in clip space before projection.
****************************************************************************/
void CSoftwareRender::render3D( const CMatrix & matrix, const uint vertCount, const uint indexCount, const CTexture * pText, float * pVBO, uint * pIBO, const CColor<float> & color, FragmentShaderFunc shader )
{
    CVertex * pVert = (CVertex *)pVBO;

    // Temporary struct to hold transformed but unprojected vertex data
    struct TransVert
    {
        CPoint<float> pos;  // Clip-space position (x, y, z from matrix transform)
        float w;            // Clip-space W (represents -eye.z for PerspectiveFovRH)
        float u, v;         // Raw UV (0-1)
    };

    TransVert * pTrans = new TransVert[vertCount];

    // Get raw matrix data for W computation
    const float * mat = matrix();

    // Near clip distance — W must be >= this to be in front of the near plane.
    // W equals -eye.z for PerspectiveFovRH, so this matches the frustum near plane.
    const float nearClip = CSettings::Instance().getMinZdist();

    // Max clipped output verts from a triangle (clipping can add 1 vertex)
    const int MAX_CLIP_VERTS = 4;

    for( uint i = 0; i < vertCount; ++i )
    {
        // Transform the verts (computes x, y, z but not w)
        matrix.transform( pTrans[i].pos, pVert[i].vert );

        // Compute W from the projection matrix
        pTrans[i].w = ( pVert[i].vert.x * mat[CMatrix::m03] )
                    + ( pVert[i].vert.y * mat[CMatrix::m13] )
                    + ( pVert[i].vert.z * mat[CMatrix::m23] )
                    + mat[CMatrix::m33];

        // Store raw UVs for clipping interpolation
        pTrans[i].u = pVert[i].uv.u;
        pTrans[i].v = pVert[i].uv.v;
    }

    // Convert float color (0.0-1.0) to fixed-point (0-255) once per mesh
    CColor<uint32_t> color32( (uint32_t)(color.r * 255.0f), (uint32_t)(color.g * 255.0f), (uint32_t)(color.b * 255.0f), (uint32_t)(color.a * 255.0f) );

    // Collect surviving triangles for strip-based rendering
    std::vector<CRender3d> triList;
    int triCount = indexCount / TRI;
    int vIndex(0);

    for( int i = 0; i < triCount; ++i )
    {
        // Get the three transformed (unprojected) verts for this triangle
        TransVert triVerts[TRI];
        for( int j = 0; j < TRI; ++j )
            triVerts[j] = pTrans[ pIBO[vIndex++] ];

        // Count how many verts are behind the near clip plane
        int behindCount = 0;
        for( int j = 0; j < TRI; ++j )
            if( triVerts[j].w < nearClip )
                ++behindCount;

        // All verts behind near plane — skip entirely
        if( behindCount == TRI )
            continue;

        // Clipped output verts
        TransVert clipped[MAX_CLIP_VERTS];
        int clipCount = 0;

        if( behindCount == 0 )
        {
            // No clipping needed — all verts in front of the near plane
            clipCount = TRI;
            for( int j = 0; j < TRI; ++j )
                clipped[j] = triVerts[j];
        }
        else
        {
            // Sutherland-Hodgman clip against near plane (W >= nearClip)
            // Following the legacy tri3D ClipProjectXYZ approach
            int startI = TRI - 1;

            for( int endI = 0; endI < TRI; ++endI )
            {
                bool startInside = (triVerts[startI].w >= nearClip);
                bool endInside   = (triVerts[endI].w >= nearClip);

                if( startInside )
                {
                    if( endInside )
                    {
                        // Case 1: Both inside — output end vertex
                        clipped[clipCount++] = triVerts[endI];
                    }
                    else
                    {
                        // Case 2: Leaving view volume — output intersection
                        // p = (nearClip - startW) / (endW - startW)
                        float deltaW = triVerts[endI].w - triVerts[startI].w;
                        float p = (nearClip - triVerts[startI].w) / deltaW;

                        clipped[clipCount].pos.x = triVerts[startI].pos.x + (triVerts[endI].pos.x - triVerts[startI].pos.x) * p;
                        clipped[clipCount].pos.y = triVerts[startI].pos.y + (triVerts[endI].pos.y - triVerts[startI].pos.y) * p;
                        clipped[clipCount].pos.z = triVerts[startI].pos.z + (triVerts[endI].pos.z - triVerts[startI].pos.z) * p;
                        clipped[clipCount].w = nearClip;
                        clipped[clipCount].u = triVerts[startI].u + (triVerts[endI].u - triVerts[startI].u) * p;
                        clipped[clipCount].v = triVerts[startI].v + (triVerts[endI].v - triVerts[startI].v) * p;
                        ++clipCount;
                    }
                }
                else
                {
                    if( endInside )
                    {
                        // Case 3: Entering view volume — output intersection + end vertex
                        float deltaW = triVerts[endI].w - triVerts[startI].w;
                        float p = (nearClip - triVerts[startI].w) / deltaW;

                        clipped[clipCount].pos.x = triVerts[startI].pos.x + (triVerts[endI].pos.x - triVerts[startI].pos.x) * p;
                        clipped[clipCount].pos.y = triVerts[startI].pos.y + (triVerts[endI].pos.y - triVerts[startI].pos.y) * p;
                        clipped[clipCount].pos.z = triVerts[startI].pos.z + (triVerts[endI].pos.z - triVerts[startI].pos.z) * p;
                        clipped[clipCount].w = nearClip;
                        clipped[clipCount].u = triVerts[startI].u + (triVerts[endI].u - triVerts[startI].u) * p;
                        clipped[clipCount].v = triVerts[startI].v + (triVerts[endI].v - triVerts[startI].v) * p;
                        ++clipCount;

                        clipped[clipCount++] = triVerts[endI];
                    }
                    // Case 4: Both outside — output nothing
                }

                startI = endI;
            }
        }

        // Skip degenerate results
        if( clipCount < TRI )
            continue;

        // Project clipped verts to screen space
        CVertex projected[MAX_CLIP_VERTS];
        for( int j = 0; j < clipCount; ++j )
        {
            float oneOverW = 1.0f / clipped[j].w;

            projected[j].vert.x = (clipped[j].pos.x * oneOverW * m_halfScreen.w) + m_halfScreen.w;
            projected[j].vert.y = (clipped[j].pos.y * oneOverW * m_halfScreen.h) + m_halfScreen.h;
            projected[j].vert.z = oneOverW;              // 1/W for z-buffer
            projected[j].uv.u = clipped[j].u * oneOverW; // U/W
            projected[j].uv.v = clipped[j].v * oneOverW; // V/W
        }

        // Fan-triangulate the clipped polygon (3 verts = 1 tri, 4 verts = 2 tris)
        for( int j = 1; j < clipCount - 1; ++j )
        {
            CRender3d render3d( pText, &m_surfaceData, m_zBuffer.data(), color32, shader );

            render3d.m_vec[0] = projected[0];
            render3d.m_vec[1] = projected[j];
            render3d.m_vec[2] = projected[j + 1];

            // Only keep visible triangles
            if( !render3d.Cull( m_surfaceData.w, m_surfaceData.h ) )
                triList.push_back( render3d );
        }
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
                    CThreadPool::Instance().post( RenderStrip3d, &triList, yMin, yMax ) );
            }

            for( auto & fut : futures )
                fut.get();
        }
        else
        {
            // Fallback: single-threaded
            RenderStrip3d( &triList, 0, screenH );
        }
    }

    delete[] pTrans;
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

                        // Jump ahead to next subdivision
                        float nextU = u + subUStep;
                        float nextV = v + subVStep;
                        float nextZ = z + subZStep;

                        // Calculate the next point to interpolate
                        float nextRealZ = 1.0f / nextZ;
                        int64_t fixTx2 = (int64_t)((double)(nextU * nextRealZ) * (double)textureW * FIX_SCALE_TEX);
                        int64_t fixTy2 = (int64_t)((double)(nextV * nextRealZ) * (double)textureH * FIX_SCALE_TEX);

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
                                    if( render.m_shader( *(pText + uvOffset), pDBuffer, texX, texY, render.m_color ) )
                                        *pZBuffer = (int32_t)fixZ;
                                }
                            }

                            fixZ += fixStepZ;
                            fixTx += fixTxStep;
                            fixTy += fixTyStep;
                            ++pDBuffer;
                            ++pZBuffer;
                        }

                        // Advance u, v, z to the next subdivision point
                        u = nextU;
                        v = nextV;
                        z = nextZ;

                        // Reuse the last calculations as the first
                        fixTx1 = fixTx2;
                        fixTy1 = fixTy2;
                    }

                    // Calculate remainder of scan-line left to rasterize
                    int length = width & (RUN_LENGTH - 1);

                    if( length > 0 )
                    {
                        // Jump ahead to the remainder
                        float nextU = u + (stepU * length);
                        float nextV = v + (stepV * length);
                        float nextZ = z + (stepZ * length);

                        // Calculate the next point to interpolate
                        float nextRealZ = 1.0f / nextZ;
                        int64_t fixTx2 = (int64_t)((double)(nextU * nextRealZ) * (double)textureW * FIX_SCALE_TEX);
                        int64_t fixTy2 = (int64_t)((double)(nextV * nextRealZ) * (double)textureH * FIX_SCALE_TEX);

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
                                    if( render.m_shader( *(pText + uvOffset), pDBuffer, texX, texY, render.m_color ) )
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


/***************************************************************************
*   desc:  Render 2D fixed-function (orthographic, no shader)
****************************************************************************/
void CSoftwareRender::renderFixedFunction2D( const CMatrix & matrix, const uint vertCount, const uint indexCount, const CTexture * pText, float * pVBO, uint * pIBO, const CColor<float> & color, bool blendAlpha )
{
    CVertex * pVert = (CVertex *)pVBO;

    CVertex * pTrans = new CVertex[vertCount];

    for( uint i = 0; i < vertCount; ++i )
    {
        matrix.transform( pTrans[i].vert, pVert[i].vert );

        pTrans[i].vert.x = (pTrans[i].vert.x * m_halfScreen.w) + m_halfScreen.w;
        pTrans[i].vert.y = (pTrans[i].vert.y * m_halfScreen.h) + m_halfScreen.h;

        pTrans[i].uv.u = pVert[i].uv.u * pText->m_size.w;
        pTrans[i].uv.v = pVert[i].uv.v * pText->m_size.h;
    }

    bool applyColor = false;
    CColor<uint32_t> color32;
    color32.set( (uint32_t)(color.r * 255.0f), (uint32_t)(color.g * 255.0f), (uint32_t)(color.b * 255.0f), (uint32_t)(color.a * 255.0f) );

    if( color32.r != 255 || color32.g != 255 || color32.b != 255 || color32.a != 255 )
        applyColor = true;

    std::vector<CRender2d> triList;
    int triCount = indexCount / TRI;
    int vIndex(0);

    for( int i = 0; i < triCount; ++i )
    {
        CRender2d render2d( pText, &m_surfaceData, color32, applyColor, blendAlpha );

        for( int j = 0; j < TRI; ++j )
            render2d.m_vec[j] = pTrans[ pIBO[vIndex++] ];

        if( !render2d.Cull( m_surfaceData.w, m_surfaceData.h ) )
            triList.push_back( render2d );
    }

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
                    CThreadPool::Instance().post( RenderStripFixedFunction2d, &triList, yMin, yMax ) );
            }

            for( auto & fut : futures )
                fut.get();
        }
        else
        {
            RenderStripFixedFunction2d( &triList, 0, screenH );
        }
    }

    delete[] pTrans;
}

void RenderStripFixedFunction2d( const std::vector<CRender2d> * pTriList, int yMin, int yMax )
{
    for( const auto & tri : *pTriList )
        RenderTriStripFixedFunction2d( tri, yMin, yMax );
}

void RenderTriStripFixedFunction2d( const CRender2d & render, int yMin, int yMax )
{
    float triYMin = render.m_vec[0].vert.y;
    float triYMax = render.m_vec[0].vert.y;
    for( int i = 1; i < TRI; ++i )
    {
        if( render.m_vec[i].vert.y < triYMin ) triYMin = render.m_vec[i].vert.y;
        if( render.m_vec[i].vert.y > triYMax ) triYMax = render.m_vec[i].vert.y;
    }
    if( triYMax < yMin || triYMin >= yMax )
        return;

    int xStart, xEnd, width, height, slopeCount(TRI);
    int64_t fixStepU, fixStepV, fixU, fixV;
    float u, v, stepU, stepV, step;
    uint * pDBuffer;

    const int UV_SHIFT(32);
    const double FIX_SCALE_UV = (double)(1LL << UV_SHIFT);

    uint screenW( render.m_pSurface->w );
    uint textureW( render.m_pText->m_size.w );
    uint textureH( render.m_pText->m_size.h );
    uint * pPixels = (uint *)render.m_pSurface->pixels;
    uint * pText = (uint *)render.m_pText->m_pData;

    double uOffset( (textureW % 2) ? 0.5 : 0.0 );
    double vOffset( (textureH % 2) ? 0.5 : 0.0 );

    uint uvOffsetMax = render.m_pText->m_size.w * render.m_pText->m_size.h;
    uint uvOffset;
    uint * pPixelsEnd = pPixels + (render.m_pSurface->w * render.m_pSurface->h);

    int vTop(0);
    for( int i = 1; i < TRI; ++i )
        if( render.m_vec[i].vert.y < render.m_vec[vTop].vert.y )
            vTop = i;

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

    CTriangleSlope leftSlope( render.m_vec, vTop, leftType );
    CTriangleSlope rightSlope( render.m_vec, vTop, rightType );

    while( slopeCount > 0 )
    {
        if( leftSlope.Init() ) --slopeCount;
        if( rightSlope.Init() ) --slopeCount;

        if( leftSlope.m_length < rightSlope.m_length )
        {
            height = leftSlope.m_length;
            if( (leftSlope.y + height) >= yMax ) { height = yMax - leftSlope.y; slopeCount = 0; }
        }
        else
        {
            height = rightSlope.m_length;
            if( (rightSlope.y + height) >= yMax ) { height = yMax - rightSlope.y; slopeCount = 0; }
        }

        int yIndex = leftSlope.y * (int)screenW;

        while( height-- > 0 )
        {
            if( leftSlope.y >= yMin )
            {
                xStart = leftSlope.m_slope.vert.x;
                xEnd = rightSlope.m_slope.vert.x;
                width = xEnd - xStart;

                if( ( width > 0 ) && ( xEnd > 0 ) && ( xStart < (int)screenW ) && (leftSlope.y < yMax) )
                {
                    u = leftSlope.m_slope.uv.u;
                    v = leftSlope.m_slope.uv.v;
                    stepU = (rightSlope.m_slope.uv.u - u) / width;
                    stepV = (rightSlope.m_slope.uv.v - v) / width;

                    if( xStart < 0 )
                    {
                        step = -xStart;
                        u += (stepU * step);
                        v += (stepV * step);
                        xStart = 0;
                        width = xEnd;
                    }

                    if( xEnd > (int)screenW ) { xEnd = screenW; width = xEnd - xStart; }

                    pDBuffer = pPixels + yIndex + xStart;
                    fixStepU = (int64_t)((double)stepU * FIX_SCALE_UV);
                    fixStepV = (int64_t)((double)stepV * FIX_SCALE_UV);
                    fixU = (int64_t)(((double)u + uOffset) * FIX_SCALE_UV);
                    fixV = (int64_t)(((double)v + vOffset) * FIX_SCALE_UV);

                    if( render.m_applyColor && render.m_blendAlpha )
                    {
                        uint32_t cr = render.m_color.r, cg = render.m_color.g, cb = render.m_color.b;
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
                                    *pDBuffer = (255u << 24) | (r << 16) | (g << 8) | b;
                                }
                            }
                            ++pDBuffer; fixU += fixStepU; fixV += fixStepV;
                        }
                    }
                    else if( render.m_applyColor )
                    {
                        uint32_t cr = render.m_color.r, cg = render.m_color.g, cb = render.m_color.b;
                        while( width-- > 0 )
                        {
                            uvOffset = ((uint)(fixV >> UV_SHIFT) * textureW) + (uint)(fixU >> UV_SHIFT);
                            if( (uvOffset < uvOffsetMax) && (pDBuffer < pPixelsEnd) )
                            {
                                uint32_t texel = *(pText + uvOffset);
                                uint32_t r = ((texel >> 16) & 0xFF) * cr / 255;
                                uint32_t g = ((texel >>  8) & 0xFF) * cg / 255;
                                uint32_t b = ( texel        & 0xFF) * cb / 255;
                                *pDBuffer = (255u << 24) | (r << 16) | (g << 8) | b;
                            }
                            ++pDBuffer; fixU += fixStepU; fixV += fixStepV;
                        }
                    }
                    else if( render.m_blendAlpha )
                    {
                        while( width-- > 0 )
                        {
                            uvOffset = ((uint)(fixV >> UV_SHIFT) * textureW) + (uint)(fixU >> UV_SHIFT);
                            if( (uvOffset < uvOffsetMax) && (pDBuffer < pPixelsEnd) )
                            {
                                uint32_t texel = *(pText + uvOffset);
                                if( ((texel >> 24) & 0xFF) == 255 )
                                    *pDBuffer = texel;
                            }
                            ++pDBuffer; fixU += fixStepU; fixV += fixStepV;
                        }
                    }
                    else
                    {
                        while( width-- > 0 )
                        {
                            uvOffset = ((uint)(fixV >> UV_SHIFT) * textureW) + (uint)(fixU >> UV_SHIFT);
                            if( (uvOffset < uvOffsetMax) && (pDBuffer < pPixelsEnd) )
                                *pDBuffer = *(pText + uvOffset);
                            ++pDBuffer; fixU += fixStepU; fixV += fixStepV;
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


void CSoftwareRender::renderFixedFunction3D( const CMatrix & matrix, const uint vertCount, const uint indexCount, const CTexture * pText, float * pVBO, uint * pIBO, const CColor<float> & color )
{
    CVertex * pVert = (CVertex *)pVBO;

    struct TransVert { CPoint<float> pos; float w; float u, v; };

    TransVert * pTrans = new TransVert[vertCount];
    const float * mat = matrix();
    const float nearClip = CSettings::Instance().getMinZdist();
    const int MAX_CLIP_VERTS = 4;

    for( uint i = 0; i < vertCount; ++i )
    {
        matrix.transform( pTrans[i].pos, pVert[i].vert );
        pTrans[i].w = ( pVert[i].vert.x * mat[CMatrix::m03] )
                    + ( pVert[i].vert.y * mat[CMatrix::m13] )
                    + ( pVert[i].vert.z * mat[CMatrix::m23] )
                    + mat[CMatrix::m33];
        pTrans[i].u = pVert[i].uv.u;
        pTrans[i].v = pVert[i].uv.v;
    }

    bool applyColor = false;
    CColor<uint32_t> color32;
    color32.set( (uint32_t)(color.r * 255.0f), (uint32_t)(color.g * 255.0f), (uint32_t)(color.b * 255.0f), (uint32_t)(color.a * 255.0f) );

    if( color32.r != 255 || color32.g != 255 || color32.b != 255 || color32.a != 255 )
        applyColor = true;

    std::vector<CRender3d> triList;
    int triCount = indexCount / TRI;
    int vIndex(0);

    for( int i = 0; i < triCount; ++i )
    {
        TransVert triVerts[TRI];
        for( int j = 0; j < TRI; ++j )
            triVerts[j] = pTrans[ pIBO[vIndex++] ];

        int behindCount = 0;
        for( int j = 0; j < TRI; ++j )
            if( triVerts[j].w < nearClip ) ++behindCount;

        if( behindCount == TRI ) continue;

        TransVert clipped[MAX_CLIP_VERTS];
        int clipCount = 0;

        if( behindCount == 0 )
        {
            clipCount = TRI;
            for( int j = 0; j < TRI; ++j ) clipped[j] = triVerts[j];
        }
        else
        {
            int startI = TRI - 1;
            for( int endI = 0; endI < TRI; ++endI )
            {
                bool startInside = (triVerts[startI].w >= nearClip);
                bool endInside   = (triVerts[endI].w >= nearClip);

                if( startInside )
                {
                    if( endInside )
                    {
                        clipped[clipCount++] = triVerts[endI];
                    }
                    else
                    {
                        float deltaW = triVerts[endI].w - triVerts[startI].w;
                        float p = (nearClip - triVerts[startI].w) / deltaW;
                        clipped[clipCount].pos.x = triVerts[startI].pos.x + (triVerts[endI].pos.x - triVerts[startI].pos.x) * p;
                        clipped[clipCount].pos.y = triVerts[startI].pos.y + (triVerts[endI].pos.y - triVerts[startI].pos.y) * p;
                        clipped[clipCount].pos.z = triVerts[startI].pos.z + (triVerts[endI].pos.z - triVerts[startI].pos.z) * p;
                        clipped[clipCount].w = nearClip;
                        clipped[clipCount].u = triVerts[startI].u + (triVerts[endI].u - triVerts[startI].u) * p;
                        clipped[clipCount].v = triVerts[startI].v + (triVerts[endI].v - triVerts[startI].v) * p;
                        ++clipCount;
                    }
                }
                else if( endInside )
                {
                    float deltaW = triVerts[endI].w - triVerts[startI].w;
                    float p = (nearClip - triVerts[startI].w) / deltaW;
                    clipped[clipCount].pos.x = triVerts[startI].pos.x + (triVerts[endI].pos.x - triVerts[startI].pos.x) * p;
                    clipped[clipCount].pos.y = triVerts[startI].pos.y + (triVerts[endI].pos.y - triVerts[startI].pos.y) * p;
                    clipped[clipCount].pos.z = triVerts[startI].pos.z + (triVerts[endI].pos.z - triVerts[startI].pos.z) * p;
                    clipped[clipCount].w = nearClip;
                    clipped[clipCount].u = triVerts[startI].u + (triVerts[endI].u - triVerts[startI].u) * p;
                    clipped[clipCount].v = triVerts[startI].v + (triVerts[endI].v - triVerts[startI].v) * p;
                    ++clipCount;
                    clipped[clipCount++] = triVerts[endI];
                }

                startI = endI;
            }
        }

        if( clipCount < TRI ) continue;

        CVertex projected[MAX_CLIP_VERTS];
        for( int j = 0; j < clipCount; ++j )
        {
            float oneOverW = 1.0f / clipped[j].w;
            projected[j].vert.x = (clipped[j].pos.x * oneOverW * m_halfScreen.w) + m_halfScreen.w;
            projected[j].vert.y = (clipped[j].pos.y * oneOverW * m_halfScreen.h) + m_halfScreen.h;
            projected[j].vert.z = oneOverW;
            projected[j].uv.u = clipped[j].u * oneOverW;
            projected[j].uv.v = clipped[j].v * oneOverW;
        }

        for( int j = 1; j < clipCount - 1; ++j )
        {
            CRender3d render3d( pText, &m_surfaceData, m_zBuffer.data(), color32, applyColor );
            render3d.m_vec[0] = projected[0];
            render3d.m_vec[1] = projected[j];
            render3d.m_vec[2] = projected[j + 1];

            if( !render3d.Cull( m_surfaceData.w, m_surfaceData.h ) )
                triList.push_back( render3d );
        }
    }

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
                    CThreadPool::Instance().post( RenderStripFixedFunction3d, &triList, yMin, yMax ) );
            }
            for( auto & fut : futures ) fut.get();
        }
        else
        {
            RenderStripFixedFunction3d( &triList, 0, screenH );
        }
    }

    delete[] pTrans;
}

void RenderStripFixedFunction3d( const std::vector<CRender3d> * pTriList, int yMin, int yMax )
{
    for( const auto & tri : *pTriList )
        RenderTriStripFixedFunction3d( tri, yMin, yMax );
}

void RenderTriStripFixedFunction3d( const CRender3d & render, int yMin, int yMax )
{
    float triYMin = render.m_vec[0].vert.y;
    float triYMax = render.m_vec[0].vert.y;
    for( int i = 1; i < TRI; ++i )
    {
        if( render.m_vec[i].vert.y < triYMin ) triYMin = render.m_vec[i].vert.y;
        if( render.m_vec[i].vert.y > triYMax ) triYMax = render.m_vec[i].vert.y;
    }
    if( triYMax < yMin || triYMin >= yMax )
        return;

    int xStart, xEnd, width, height, slopeCount(TRI);
    float u, v, z, stepU, stepV, stepZ;
    int64_t fixZ, fixStepZ, fixTx1, fixTy1;
    uint * pDBuffer;
    int32_t * pZBuffer;

    const double FIX_SCALE_Z = 67108864.0;
    const int TEX_SHIFT(32);
    const double FIX_SCALE_TEX = (double)(1LL << TEX_SHIFT);
    const int RUN_LENGTH(16);
    const int RUN_SHIFT(4);

    uint screenW( render.m_pSurface->w );
    uint textureW( render.m_pText->m_size.w );
    uint textureH( render.m_pText->m_size.h );
    uint * pPixels = (uint *)render.m_pSurface->pixels;
    uint * pText = (uint *)render.m_pText->m_pData;
    int32_t * pZBuf = render.m_pZBuffer;

    uint uvOffsetMax = textureW * textureH;
    uint * pPixelsEnd = pPixels + (render.m_pSurface->w * render.m_pSurface->h);

    int vTop(0);
    for( int i = 1; i < TRI; ++i )
        if( render.m_vec[i].vert.y < render.m_vec[vTop].vert.y )
            vTop = i;

    CTriangleSlope leftSlope( render.m_vec, vTop, CTriangleSlope::EST_LEFT );
    CTriangleSlope rightSlope( render.m_vec, vTop, CTriangleSlope::EST_RIGHT );

    while( slopeCount > 0 )
    {
        if( leftSlope.Init() ) --slopeCount;
        if( rightSlope.Init() ) --slopeCount;

        if( leftSlope.m_length < rightSlope.m_length )
        {
            height = leftSlope.m_length;
            if( (leftSlope.y + height) >= yMax ) { height = yMax - leftSlope.y; slopeCount = 0; }
        }
        else
        {
            height = rightSlope.m_length;
            if( (rightSlope.y + height) >= yMax ) { height = yMax - rightSlope.y; slopeCount = 0; }
        }

        int yIndex = leftSlope.y * (int)screenW;

        while( height-- > 0 )
        {
            if( leftSlope.y >= yMin )
            {
                xStart = leftSlope.m_slope.vert.x;
                xEnd = rightSlope.m_slope.vert.x;
                width = xEnd - xStart;

                if( ( width > 0 ) && ( xEnd > 0 ) && ( xStart < (int)screenW ) && (leftSlope.y < yMax) )
                {
                    z = leftSlope.m_slope.vert.z;
                    u = leftSlope.m_slope.uv.u;
                    v = leftSlope.m_slope.uv.v;
                    stepZ = (rightSlope.m_slope.vert.z - z) / width;
                    stepU = (rightSlope.m_slope.uv.u - u) / width;
                    stepV = (rightSlope.m_slope.uv.v - v) / width;

                    if( xStart < 0 )
                    {
                        float clip = -xStart;
                        z += (stepZ * clip); u += (stepU * clip); v += (stepV * clip);
                        xStart = 0; width = xEnd;
                    }

                    if( xEnd > (int)screenW ) { xEnd = screenW; width = xEnd - xStart; }

                    pDBuffer = pPixels + yIndex + xStart;
                    pZBuffer = pZBuf + yIndex + xStart;
                    fixZ = (int64_t)(z * FIX_SCALE_Z);
                    fixStepZ = (int64_t)(stepZ * FIX_SCALE_Z);

                    float realZ = 1.0f / z;
                    fixTx1 = (int64_t)((double)(u * realZ) * (double)textureW * FIX_SCALE_TEX);
                    fixTy1 = (int64_t)((double)(v * realZ) * (double)textureH * FIX_SCALE_TEX);

                    int runLoops = width >> RUN_SHIFT;
                    float subUStep = stepU * RUN_LENGTH;
                    float subVStep = stepV * RUN_LENGTH;
                    float subZStep = stepZ * RUN_LENGTH;

                    for( int count = 0; count < runLoops; ++count )
                    {
                        int length = RUN_LENGTH;
                        float nextU = u + subUStep, nextV = v + subVStep, nextZ = z + subZStep;
                        float nextRealZ = 1.0f / nextZ;
                        int64_t fixTx2 = (int64_t)((double)(nextU * nextRealZ) * (double)textureW * FIX_SCALE_TEX);
                        int64_t fixTy2 = (int64_t)((double)(nextV * nextRealZ) * (double)textureH * FIX_SCALE_TEX);
                        int64_t fixTxStep = (fixTx2 - fixTx1) >> RUN_SHIFT;
                        int64_t fixTyStep = (fixTy2 - fixTy1) >> RUN_SHIFT;
                        int64_t fixTx = fixTx1, fixTy = fixTy1;

                        while( length-- > 0 )
                        {
                            if( *pZBuffer < (int32_t)fixZ && pDBuffer < pPixelsEnd )
                            {
                                uint texX = (uint)(fixTx >> TEX_SHIFT);
                                uint texY = (uint)(fixTy >> TEX_SHIFT);
                                if( texX >= textureW ) texX = textureW - 1;
                                if( texY >= textureH ) texY = textureH - 1;
                                uint uvOffset = texY * textureW + texX;

                                if( uvOffset < uvOffsetMax )
                                {
                                    uint32_t texel = *(pText + uvOffset);
                                    if( render.m_applyColor )
                                    {
                                        uint32_t r = ((texel >> 16) & 0xFF) * render.m_color.r / 255;
                                        uint32_t g = ((texel >>  8) & 0xFF) * render.m_color.g / 255;
                                        uint32_t b = ( texel        & 0xFF) * render.m_color.b / 255;
                                        texel = (255u << 24) | (r << 16) | (g << 8) | b;
                                    }
                                    *pDBuffer = texel;
                                    *pZBuffer = (int32_t)fixZ;
                                }
                            }
                            fixZ += fixStepZ; fixTx += fixTxStep; fixTy += fixTyStep;
                            ++pDBuffer; ++pZBuffer;
                        }

                        u = nextU; v = nextV; z = nextZ;
                        fixTx1 = fixTx2; fixTy1 = fixTy2;
                    }

                    int length = width & (RUN_LENGTH - 1);
                    if( length > 0 )
                    {
                        float nextU = u + (stepU * length), nextV = v + (stepV * length), nextZ = z + (stepZ * length);
                        float nextRealZ = 1.0f / nextZ;
                        int64_t fixTx2 = (int64_t)((double)(nextU * nextRealZ) * (double)textureW * FIX_SCALE_TEX);
                        int64_t fixTy2 = (int64_t)((double)(nextV * nextRealZ) * (double)textureH * FIX_SCALE_TEX);
                        int64_t fixTxStep = (fixTx2 - fixTx1) / length;
                        int64_t fixTyStep = (fixTy2 - fixTy1) / length;
                        int64_t fixTx = fixTx1, fixTy = fixTy1;

                        while( length-- > 0 )
                        {
                            if( *pZBuffer < (int32_t)fixZ && pDBuffer < pPixelsEnd )
                            {
                                uint texX = (uint)(fixTx >> TEX_SHIFT);
                                uint texY = (uint)(fixTy >> TEX_SHIFT);
                                if( texX >= textureW ) texX = textureW - 1;
                                if( texY >= textureH ) texY = textureH - 1;
                                uint uvOffset = texY * textureW + texX;

                                if( uvOffset < uvOffsetMax )
                                {
                                    uint32_t texel = *(pText + uvOffset);
                                    if( render.m_applyColor )
                                    {
                                        uint32_t r = ((texel >> 16) & 0xFF) * render.m_color.r / 255;
                                        uint32_t g = ((texel >>  8) & 0xFF) * render.m_color.g / 255;
                                        uint32_t b = ( texel        & 0xFF) * render.m_color.b / 255;
                                        texel = (255u << 24) | (r << 16) | (g << 8) | b;
                                    }
                                    *pDBuffer = texel;
                                    *pZBuffer = (int32_t)fixZ;
                                }
                            }
                            fixZ += fixStepZ; fixTx += fixTxStep; fixTy += fixTyStep;
                            ++pDBuffer; ++pZBuffer;
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
