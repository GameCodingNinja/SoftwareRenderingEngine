
/************************************************************************
*    FILE NAME:       softwareRender.cpp
*
*    DESCRIPTION:     Software Rendering device
************************************************************************/

// Physical component dependency
#include <softwareRender/softwareRender.h>

// Standard lib dependencies
#include <cstring>
#ifdef MATRIX_USE_SSE
#include <xmmintrin.h>
#endif

// Game lib dependencies
#include <common/matrix.h>
#include <utilities/threadpool.h>
#include <utilities/genfunc.h>
#include <utilities/exceptionhandling.h>
#include <common/texture.h>
#include <2d/visualcomponent2d.h>
#include <3d/visualcomponent3d.h>
#include <softwareRender/triangleslope.h>

#include <system/iframebuffer.h>
#include <utilities/settings.h>

// Render a single triangle within a screen strip
void RenderTriStrip2d( const CRender2d & render, int yMin, int yMax );

// Render all triangles within a screen strip (binned pointer list)
void RenderStrip2d( const std::vector<const CRender2d *> * pTriList, int yMin, int yMax );

// Render a single 3D triangle within a screen strip
void RenderTriStrip3d( const CRender3d & render, int yMin, int yMax );

// Render all 3D triangles within a screen strip (binned pointer list)
void RenderStrip3d( const std::vector<const CRender3d *> * pTriList, int yMin, int yMax );

// Fixed-function render functions
void RenderTriStripFixedFunction2d( const CRender2d & render, int yMin, int yMax );
void RenderStripFixedFunction2d( const std::vector<const CRender2d *> * pTriList, int yMin, int yMax );
void RenderTriStripFixedFunction3d( const CRender3d & render, int yMin, int yMax );
void RenderStripFixedFunction3d( const std::vector<const CRender3d *> * pTriList, int yMin, int yMax );

/************************************************************************
*    desc:  Compute and cache triangle Y bounds for strip binning
************************************************************************/
template<typename T>
void CalcTriYBounds( T & tri )
{
    tri.m_triYMin = tri.m_vec[0].vert.y;
    tri.m_triYMax = tri.m_vec[0].vert.y;
    for( int i = 1; i < TRI; ++i )
    {
        if( tri.m_vec[i].vert.y < tri.m_triYMin ) tri.m_triYMin = tri.m_vec[i].vert.y;
        if( tri.m_vec[i].vert.y > tri.m_triYMax ) tri.m_triYMax = tri.m_vec[i].vert.y;
    }
}

/************************************************************************
*    desc:  Bin triangles into per-strip vectors based on cached Y bounds
************************************************************************/
template<typename T>
void BinTriangles( const std::vector<T> & triList, std::vector<std::vector<const T *>> & stripBins, int screenH, size_t threads )
{
    int stripH = screenH / threads;

    for( const auto & tri : triList )
    {
        for( size_t t = 0; t < threads; ++t )
        {
            int yMin = t * stripH;
            int yMax = (t == threads - 1) ? screenH : (t + 1) * stripH;

            if( tri.m_triYMax >= yMin && tri.m_triYMin < yMax )
                stripBins[t].push_back( &tri );
        }
    }
}

/************************************************************************
*    desc:  Constructor
************************************************************************/
CSoftwareRender::CSoftwareRender()
{
    // Init the thread pool if not already active
    if( !CThreadPool::Instance().isActive() )
        CThreadPool::Instance().init( 2, -1 );
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
void CSoftwareRender::render2D( const CMatrix & matrix, const CVisualComponent2d & visualComponent )
{
    CVertex * pVert = (CVertex *)visualComponent.getVBO();
    const uint vertCount = visualComponent.getVertexCount();
    const uint indexCount = visualComponent.getIndexCount();
    uint * pIBO = visualComponent.getIBO();
    const CTexture * pText = visualComponent.getTexture();
    const CColor<float> & color = visualComponent.getColor();
    FragmentShaderFunc shader = visualComponent.getShader();

    m_transVerts2D.resize( vertCount );
    CVertex * pTrans = m_transVerts2D.data();

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
        {
            CalcTriYBounds( render2d );
            triList.push_back( render2d );
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
            // Bin triangles into per-strip lists
            std::vector<std::vector<const CRender2d *>> stripBins( threads );
            BinTriangles( triList, stripBins, screenH, threads );

            int stripH = screenH / threads;
            std::vector<std::future<void>> futures;

            for( size_t t = 0; t < threads; ++t )
            {
                int yMin = t * stripH;
                int yMax = (t == threads - 1) ? screenH : (t + 1) * stripH;

                futures.emplace_back(
                    CThreadPool::Instance().post( RenderStrip2d, &stripBins[t], yMin, yMax ) );
            }

            for( auto & fut : futures )
                fut.get();
        }
        else
        {
            // Fallback: single-threaded (build a single bin with all triangles)
            std::vector<const CRender2d *> allTris;
            allTris.reserve( triList.size() );
            for( const auto & tri : triList )
                allTris.push_back( &tri );
            RenderStrip2d( &allTris, 0, screenH );
        }
    }
}

/***************************************************************************
*   desc:  Render all triangles within a horizontal screen strip
****************************************************************************/
void RenderStrip2d( const std::vector<const CRender2d *> * pTriList, int yMin, int yMax )
{
    for( const auto * pTri : *pTriList )
        RenderTriStrip2d( *pTri, yMin, yMax );
}

/***************************************************************************
*   desc:  Render a single triangle, only writing scanlines in [yMin, yMax)
****************************************************************************/
void RenderTriStrip2d( const CRender2d & render, int yMin, int yMax )
{
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

        // Skip scanlines above this strip's range in one shot
        if( leftSlope.y < yMin )
        {
            int skip = yMin - leftSlope.y;
            if( skip > height ) skip = height;
            leftSlope.Advance( skip );
            rightSlope.Advance( skip );
            height -= skip;
        }

        // Init the y index for scanline offset accumulation (avoids per-line multiply)
        int yIndex = leftSlope.y * (int)screenW;

        // Loop for the height of the slope
        while( height-- > 0 )
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
                    uint texU = (uint)(fixU >> UV_SHIFT);
                    uint texV = (uint)(fixV >> UV_SHIFT);

                    // Clamp texture coordinates
                    if( texU >= textureW ) texU = textureW - 1;
                    if( texV >= textureH ) texV = textureH - 1;

                    render.m_shader( *(pText + texV * textureW + texU), pDBuffer, texU, texV, render.m_color );

                    ++pDBuffer;
                    fixU += fixStepU;
                    fixV += fixStepV;
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
void CSoftwareRender::render3D( const CMatrix & matrix, const CVisualComponent3d & visualComponent )
{
    CVertex * pVert = (CVertex *)visualComponent.getVBO();
    const uint indexCount = visualComponent.getIndexCount();
    uint * pIBO = visualComponent.getIBO();
    const CTexture * pText = visualComponent.getTexture();
    const std::vector<CPoint<float>*> & uniqueVerts = visualComponent.getUniqueVerts();
    const std::vector<uint> & vertToUniqueVec = visualComponent.getVertToUniqueVec();
    const CColor<float> & color = visualComponent.getColor();
    FragmentShaderFunc shader = visualComponent.getShader();

    // Get raw matrix data for W computation
    const float * mat = matrix();

    // Near clip distance — W must be >= this to be in front of the near plane.
    // W equals -eye.z for PerspectiveFovRH, so this matches the frustum near plane.
    const float nearClip = CSettings::Instance().getMinZdist();

    // Max clipped output verts from a triangle (clipping can add 1 vertex)
    const int MAX_CLIP_VERTS = 4;

#ifdef MATRIX_USE_SSE
    // Load the W-row of the projection matrix for SSE dot product
    const __m128 wRow = _mm_set_ps( mat[CMatrix::m33], mat[CMatrix::m23], mat[CMatrix::m13], mat[CMatrix::m03] );
#endif

    // Transform only unique vertex positions
    const uint uniqueCount = static_cast<uint>(uniqueVerts.size());
    m_transUniqueVerts.resize( uniqueCount );

    for( uint i = 0; i < uniqueCount; ++i )
    {
        matrix.transform( m_transUniqueVerts[i].pos, *uniqueVerts[i] );

#ifdef MATRIX_USE_SSE
        // Compute W via SSE: (x * m03) + (y * m13) + (z * m23) + m33
        __m128 vert = _mm_set_ps( 1.0f, uniqueVerts[i]->z, uniqueVerts[i]->y, uniqueVerts[i]->x );
        __m128 mul = _mm_mul_ps( vert, wRow );
        __m128 shuf1 = _mm_movehl_ps( mul, mul );
        __m128 sum1 = _mm_add_ps( mul, shuf1 );
        __m128 shuf2 = _mm_shuffle_ps( sum1, sum1, _MM_SHUFFLE(0,0,0,1) );
        m_transUniqueVerts[i].w = _mm_cvtss_f32( _mm_add_ss( sum1, shuf2 ) );
#else
        m_transUniqueVerts[i].w = ( uniqueVerts[i]->x * mat[CMatrix::m03] )
                                + ( uniqueVerts[i]->y * mat[CMatrix::m13] )
                                + ( uniqueVerts[i]->z * mat[CMatrix::m23] )
                                + mat[CMatrix::m33];
#endif
    }

    // Convert float color (0.0-1.0) to fixed-point (0-255) once per mesh
    CColor<uint32_t> color32( (uint32_t)(color.r * 255.0f), (uint32_t)(color.g * 255.0f), (uint32_t)(color.b * 255.0f), (uint32_t)(color.a * 255.0f) );

    // Collect surviving triangles for strip-based rendering
    std::vector<CRender3d> triList;
    int triCount = indexCount / TRI;
    int vIndex(0);

    for( int i = 0; i < triCount; ++i )
    {
        // Build clip verts from unique transforms + pVert UVs
        SClipVert triVerts[TRI];
        for( int j = 0; j < TRI; ++j )
        {
            uint idx = pIBO[vIndex++];
            const SUniqueVert & vert = m_transUniqueVerts[ vertToUniqueVec[idx] ];
            triVerts[j].pos = vert.pos;
            triVerts[j].w   = vert.w;
            triVerts[j].u   = pVert[idx].uv.u;
            triVerts[j].v   = pVert[idx].uv.v;
        }

        // Count how many verts are behind the near clip plane
        int behindCount = 0;
        for( int j = 0; j < TRI; ++j )
            if( triVerts[j].w < nearClip )
                ++behindCount;

        // All verts behind near plane — skip entirely
        if( behindCount == TRI )
            continue;

        // Clipped output verts
        SClipVert clipped[MAX_CLIP_VERTS];
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
            {
                CalcTriYBounds( render3d );
                triList.push_back( render3d );
            }
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
            // Bin triangles into per-strip lists
            std::vector<std::vector<const CRender3d *>> stripBins( threads );
            BinTriangles( triList, stripBins, screenH, threads );

            int stripH = screenH / threads;
            std::vector<std::future<void>> futures;

            for( size_t t = 0; t < threads; ++t )
            {
                int yMin = t * stripH;
                int yMax = (t == threads - 1) ? screenH : (t + 1) * stripH;

                futures.emplace_back(
                    CThreadPool::Instance().post( RenderStrip3d, &stripBins[t], yMin, yMax ) );
            }

            for( auto & fut : futures )
                fut.get();
        }
        else
        {
            // Fallback: single-threaded
            std::vector<const CRender3d *> allTris;
            allTris.reserve( triList.size() );
            for( const auto & tri : triList )
                allTris.push_back( &tri );
            RenderStrip3d( &allTris, 0, screenH );
        }
    }
}

/***************************************************************************
*   desc:  Render all 3D triangles within a horizontal screen strip
****************************************************************************/
void RenderStrip3d( const std::vector<const CRender3d *> * pTriList, int yMin, int yMax )
{
    for( const auto * pTri : *pTriList )
        RenderTriStrip3d( *pTri, yMin, yMax );
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

        // Skip scanlines above this strip's range in one shot
        if( leftSlope.y < yMin )
        {
            int skip = yMin - leftSlope.y;
            if( skip > height ) skip = height;
            leftSlope.Advance( skip );
            rightSlope.Advance( skip );
            height -= skip;
        }

        // Init the y index for scanline offset accumulation
        int yIndex = leftSlope.y * (int)screenW;

        // Loop for the height of the slope
        while( height-- > 0 )
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
                        if( *pZBuffer < (int32_t)fixZ )
                        {
                            uint texX = (uint)(fixTx >> TEX_SHIFT);
                            uint texY = (uint)(fixTy >> TEX_SHIFT);

                            // Clamp texture coordinates
                            if( texX >= textureW ) texX = textureW - 1;
                            if( texY >= textureH ) texY = textureH - 1;

                            if( render.m_shader( *(pText + texY * textureW + texX), pDBuffer, texX, texY, render.m_color ) )
                                *pZBuffer = (int32_t)fixZ;
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
                        if( *pZBuffer < (int32_t)fixZ )
                        {
                            uint texX = (uint)(fixTx >> TEX_SHIFT);
                            uint texY = (uint)(fixTy >> TEX_SHIFT);

                            // Clamp texture coordinates
                            if( texX >= textureW ) texX = textureW - 1;
                            if( texY >= textureH ) texY = textureH - 1;

                            if( render.m_shader( *(pText + texY * textureW + texX), pDBuffer, texX, texY, render.m_color ) )
                                *pZBuffer = (int32_t)fixZ;
                        }

                        fixZ += fixStepZ;
                        fixTx += fixTxStep;
                        fixTy += fixTyStep;
                        ++pDBuffer;
                        ++pZBuffer;
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
void CSoftwareRender::renderFixedFunction2D( const CMatrix & matrix, const CVisualComponent2d & visualComponent )
{
    CVertex * pVert = (CVertex *)visualComponent.getVBO();
    const uint vertCount = visualComponent.getVertexCount();
    const uint indexCount = visualComponent.getIndexCount();
    uint * pIBO = visualComponent.getIBO();
    const CTexture * pText = visualComponent.getTexture();
    const CColor<float> & color = visualComponent.getColor();
    bool blendAlpha = visualComponent.getBlendAlpha();

    m_transVerts2D.resize( vertCount );
    CVertex * pTrans = m_transVerts2D.data();

    for( uint i = 0; i < vertCount; ++i )
    {
        matrix.transform( pTrans[i].vert, pVert[i].vert );

        pTrans[i].vert.x = (pTrans[i].vert.x * m_halfScreen.w) + m_halfScreen.w;
        pTrans[i].vert.y = (pTrans[i].vert.y * m_halfScreen.h) + m_halfScreen.h;

        pTrans[i].uv.u = pVert[i].uv.u * pText->m_size.w;
        pTrans[i].uv.v = pVert[i].uv.v * pText->m_size.h;
    }

    bool applyColor = false;
    CColor<uint32_t> color32( (uint32_t)(color.r * 255.0f), (uint32_t)(color.g * 255.0f), (uint32_t)(color.b * 255.0f), (uint32_t)(color.a * 255.0f) );

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
        {
            CalcTriYBounds( render2d );
            triList.push_back( render2d );
        }
    }

    if( !triList.empty() )
    {
        int screenH = m_surfaceData.h;
        size_t threads = CThreadPool::Instance().threadCount();

        if( threads > 0 )
        {
            // Bin triangles into per-strip lists
            std::vector<std::vector<const CRender2d *>> stripBins( threads );
            BinTriangles( triList, stripBins, screenH, threads );

            int stripH = screenH / threads;
            std::vector<std::future<void>> futures;

            for( size_t t = 0; t < threads; ++t )
            {
                int yMin = t * stripH;
                int yMax = (t == threads - 1) ? screenH : (t + 1) * stripH;

                futures.emplace_back(
                    CThreadPool::Instance().post( RenderStripFixedFunction2d, &stripBins[t], yMin, yMax ) );
            }

            for( auto & fut : futures )
                fut.get();
        }
        else
        {
            std::vector<const CRender2d *> allTris;
            allTris.reserve( triList.size() );
            for( const auto & tri : triList )
                allTris.push_back( &tri );
            RenderStripFixedFunction2d( &allTris, 0, screenH );
        }
    }
}

void RenderStripFixedFunction2d( const std::vector<const CRender2d *> * pTriList, int yMin, int yMax )
{
    for( const auto * pTri : *pTriList )
        RenderTriStripFixedFunction2d( *pTri, yMin, yMax );
}

void RenderTriStripFixedFunction2d( const CRender2d & render, int yMin, int yMax )
{
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

        // Skip scanlines above this strip's range in one shot
        if( leftSlope.y < yMin )
        {
            int skip = yMin - leftSlope.y;
            if( skip > height ) skip = height;
            leftSlope.Advance( skip );
            rightSlope.Advance( skip );
            height -= skip;
        }

        int yIndex = leftSlope.y * (int)screenW;

        while( height-- > 0 )
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
                        uint texU = (uint)(fixU >> UV_SHIFT);
                        uint texV = (uint)(fixV >> UV_SHIFT);
                        if( texU >= textureW ) texU = textureW - 1;
                        if( texV >= textureH ) texV = textureH - 1;
                        uint32_t texel = *(pText + texV * textureW + texU);
                        if( ((texel >> 24) & 0xFF) == 255 )
                        {
                            uint32_t r = ((texel >> 16) & 0xFF) * cr / 255;
                            uint32_t g = ((texel >>  8) & 0xFF) * cg / 255;
                            uint32_t b = ( texel        & 0xFF) * cb / 255;
                            *pDBuffer = (255u << 24) | (r << 16) | (g << 8) | b;
                        }
                        ++pDBuffer; fixU += fixStepU; fixV += fixStepV;
                    }
                }
                else if( render.m_applyColor )
                {
                    uint32_t cr = render.m_color.r, cg = render.m_color.g, cb = render.m_color.b;
                    while( width-- > 0 )
                    {
                        uint texU = (uint)(fixU >> UV_SHIFT);
                        uint texV = (uint)(fixV >> UV_SHIFT);
                        if( texU >= textureW ) texU = textureW - 1;
                        if( texV >= textureH ) texV = textureH - 1;
                        uint32_t texel = *(pText + texV * textureW + texU);
                        uint32_t r = ((texel >> 16) & 0xFF) * cr / 255;
                        uint32_t g = ((texel >>  8) & 0xFF) * cg / 255;
                        uint32_t b = ( texel        & 0xFF) * cb / 255;
                        *pDBuffer = (255u << 24) | (r << 16) | (g << 8) | b;
                        ++pDBuffer; fixU += fixStepU; fixV += fixStepV;
                    }
                }
                else if( render.m_blendAlpha )
                {
                    while( width-- > 0 )
                    {
                        uint texU = (uint)(fixU >> UV_SHIFT);
                        uint texV = (uint)(fixV >> UV_SHIFT);
                        if( texU >= textureW ) texU = textureW - 1;
                        if( texV >= textureH ) texV = textureH - 1;
                        uint32_t texel = *(pText + texV * textureW + texU);
                        if( ((texel >> 24) & 0xFF) == 255 )
                            *pDBuffer = texel;
                        ++pDBuffer; fixU += fixStepU; fixV += fixStepV;
                    }
                }
                else
                {
                    while( width-- > 0 )
                    {
                        uint texU = (uint)(fixU >> UV_SHIFT);
                        uint texV = (uint)(fixV >> UV_SHIFT);
                        if( texU >= textureW ) texU = textureW - 1;
                        if( texV >= textureH ) texV = textureH - 1;
                        *pDBuffer = *(pText + texV * textureW + texU);
                        ++pDBuffer; fixU += fixStepU; fixV += fixStepV;
                    }
                }
            }

            leftSlope.Inc();
            rightSlope.Inc();
            yIndex += screenW;
        }
    }
}


void CSoftwareRender::renderFixedFunction3D( const CMatrix & matrix, const CVisualComponent3d & visualComponent )
{
    CVertex * pVert = (CVertex *)visualComponent.getVBO();
    const uint indexCount = visualComponent.getIndexCount();
    uint * pIBO = visualComponent.getIBO();
    const CTexture * pText = visualComponent.getTexture();
    const std::vector<CPoint<float>*> & uniqueVerts = visualComponent.getUniqueVerts();
    const std::vector<uint> & vertToUniqueVec = visualComponent.getVertToUniqueVec();
    const CColor<float> & color = visualComponent.getColor();

    const float * mat = matrix();
    const float nearClip = CSettings::Instance().getMinZdist();
    const int MAX_CLIP_VERTS = 4;

#ifdef MATRIX_USE_SSE
    // Load the W-row of the projection matrix for SSE dot product
    const __m128 wRow = _mm_set_ps( mat[CMatrix::m33], mat[CMatrix::m23], mat[CMatrix::m13], mat[CMatrix::m03] );
#endif

    // Transform only unique vertex positions
    const uint uniqueCount = static_cast<uint>(uniqueVerts.size());
    m_transUniqueVerts.resize( uniqueCount );

    for( uint i = 0; i < uniqueCount; ++i )
    {
        matrix.transform( m_transUniqueVerts[i].pos, *uniqueVerts[i] );

#ifdef MATRIX_USE_SSE
        // Compute W via SSE: (x * m03) + (y * m13) + (z * m23) + m33
        __m128 vert = _mm_set_ps( 1.0f, uniqueVerts[i]->z, uniqueVerts[i]->y, uniqueVerts[i]->x );
        __m128 mul = _mm_mul_ps( vert, wRow );
        __m128 shuf1 = _mm_movehl_ps( mul, mul );
        __m128 sum1 = _mm_add_ps( mul, shuf1 );
        __m128 shuf2 = _mm_shuffle_ps( sum1, sum1, _MM_SHUFFLE(0,0,0,1) );
        m_transUniqueVerts[i].w = _mm_cvtss_f32( _mm_add_ss( sum1, shuf2 ) );
#else
        m_transUniqueVerts[i].w = ( uniqueVerts[i]->x * mat[CMatrix::m03] )
                                + ( uniqueVerts[i]->y * mat[CMatrix::m13] )
                                + ( uniqueVerts[i]->z * mat[CMatrix::m23] )
                                + mat[CMatrix::m33];
#endif
    }

    bool applyColor = false;
    CColor<uint32_t> color32( (uint32_t)(color.r * 255.0f), (uint32_t)(color.g * 255.0f), (uint32_t)(color.b * 255.0f), (uint32_t)(color.a * 255.0f) );

    if( color32.r != 255 || color32.g != 255 || color32.b != 255 || color32.a != 255 )
        applyColor = true;

    std::vector<CRender3d> triList;
    int triCount = indexCount / TRI;
    int vIndex(0);

    for( int i = 0; i < triCount; ++i )
    {
        // Build clip verts from unique transforms + pVert UVs
        SClipVert triVerts[TRI];
        for( int j = 0; j < TRI; ++j )
        {
            uint idx = pIBO[vIndex++];
            const SUniqueVert & vert = m_transUniqueVerts[ vertToUniqueVec[idx] ];
            triVerts[j].pos = vert.pos;
            triVerts[j].w   = vert.w;
            triVerts[j].u   = pVert[idx].uv.u;
            triVerts[j].v   = pVert[idx].uv.v;
        }

        int behindCount = 0;
        for( int j = 0; j < TRI; ++j )
            if( triVerts[j].w < nearClip ) ++behindCount;

        if( behindCount == TRI ) continue;

        SClipVert clipped[MAX_CLIP_VERTS];
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
            {
                CalcTriYBounds( render3d );
                triList.push_back( render3d );
            }
        }
    }

    if( !triList.empty() )
    {
        int screenH = m_surfaceData.h;
        size_t threads = CThreadPool::Instance().threadCount();

        if( threads > 0 )
        {
            // Bin triangles into per-strip lists
            std::vector<std::vector<const CRender3d *>> stripBins( threads );
            BinTriangles( triList, stripBins, screenH, threads );

            int stripH = screenH / threads;
            std::vector<std::future<void>> futures;

            for( size_t t = 0; t < threads; ++t )
            {
                int yMin = t * stripH;
                int yMax = (t == threads - 1) ? screenH : (t + 1) * stripH;
                futures.emplace_back(
                    CThreadPool::Instance().post( RenderStripFixedFunction3d, &stripBins[t], yMin, yMax ) );
            }
            for( auto & fut : futures ) fut.get();
        }
        else
        {
            std::vector<const CRender3d *> allTris;
            allTris.reserve( triList.size() );
            for( const auto & tri : triList )
                allTris.push_back( &tri );
            RenderStripFixedFunction3d( &allTris, 0, screenH );
        }
    }
}

void RenderStripFixedFunction3d( const std::vector<const CRender3d *> * pTriList, int yMin, int yMax )
{
    for( const auto * pTri : *pTriList )
        RenderTriStripFixedFunction3d( *pTri, yMin, yMax );
}

void RenderTriStripFixedFunction3d( const CRender3d & render, int yMin, int yMax )
{
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

        // Skip scanlines above this strip's range in one shot
        if( leftSlope.y < yMin )
        {
            int skip = yMin - leftSlope.y;
            if( skip > height ) skip = height;
            leftSlope.Advance( skip );
            rightSlope.Advance( skip );
            height -= skip;
        }

        int yIndex = leftSlope.y * (int)screenW;

        while( height-- > 0 )
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
                        if( *pZBuffer < (int32_t)fixZ )
                        {
                            uint texX = (uint)(fixTx >> TEX_SHIFT);
                            uint texY = (uint)(fixTy >> TEX_SHIFT);
                            if( texX >= textureW ) texX = textureW - 1;
                            if( texY >= textureH ) texY = textureH - 1;

                            uint32_t texel = *(pText + texY * textureW + texX);
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
                        if( *pZBuffer < (int32_t)fixZ )
                        {
                            uint texX = (uint)(fixTx >> TEX_SHIFT);
                            uint texY = (uint)(fixTy >> TEX_SHIFT);
                            if( texX >= textureW ) texX = textureW - 1;
                            if( texY >= textureH ) texY = textureH - 1;

                            uint32_t texel = *(pText + texY * textureW + texX);
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
                        fixZ += fixStepZ; fixTx += fixTxStep; fixTy += fixTyStep;
                        ++pDBuffer; ++pZBuffer;
                    }
                }
            }

            leftSlope.Inc();
            rightSlope.Inc();
            yIndex += screenW;
        }
    }
}
