
/************************************************************************
*    FILE NAME:       softwareRender.cpp
*
*    DESCRIPTION:     Software Rendering device
************************************************************************/

// Physical component dependency
#include <softwareRender/softwareRender.h>

// Standard lib dependencies
#include <cstring>
#include <cfloat>
#include <algorithm>

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
void RenderTriStrip( const CRender2d & render, int yMin, int yMax );

// Render all triangles within a screen strip
void RenderStrip( const std::vector<CRender2d> * pTriList, int yMin, int yMax );

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

}   // constructor


/************************************************************************
*    desc:  destructor                                                             
************************************************************************/
CSoftwareRender::~CSoftwareRender()
{
    NDelFunc::DeleteMapPointers(m_pTextureMap);
    NDelFunc::DeleteMapArrayPointers(m_pVBOMap);
    NDelFunc::DeleteMapArrayPointers(m_pIBOMap);

    delete[] m_surfaceData.zBuffer;

}	// destructor


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

    // Allocate the z-buffer
    delete[] m_surfaceData.zBuffer;
    m_surfaceData.zBuffer = new float[m_surfaceData.w * m_surfaceData.h];
    ClearZBuffer();

}   // SetSurface


/***************************************************************************
*   desc:  Create a texture. The pointer is now owned by this class
****************************************************************************/
uint CSoftwareRender::CreateTexture( uchar * pData, int w, int h )
{
    ++m_textIdInc;

    m_pTextureMap.insert( std::make_pair(m_textIdInc, new CSRTexture( w, h, pData )) );

    return m_textIdInc;

}   // CreateTexture


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

}   // CreateVBO


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

}   // CreateVBO


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

}   // DeleteTexture


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

}   // DeleteVBO


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

}   // DeleteVBO


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

}   // GetTexture


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

}   // GetVBO


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

}   // GetIBO


/***************************************************************************
*   desc:  Enable or disable the z-buffer
****************************************************************************/
void CSoftwareRender::EnableZBuffer( bool enable )
{
    m_surfaceData.zBufferEnabled = enable;

}   // EnableZBuffer


/***************************************************************************
*   desc:  Clear the z-buffer to max depth
****************************************************************************/
void CSoftwareRender::ClearZBuffer()
{
    if( m_surfaceData.zBuffer != nullptr )
        std::fill( m_surfaceData.zBuffer,
                   m_surfaceData.zBuffer + (m_surfaceData.w * m_surfaceData.h),
                   FLT_MAX );

}   // ClearZBuffer


/***************************************************************************
*   desc:  Render
*
*   Perspective Projection: ((trans.vert[0].vert.x / trans.vert[0].vert.z) * m_halfSize.w) + m_halfSize.w + 0.5f;
*   Orthographic Projection: (trans.vert[0].vert.x * m_halfSize.w) + m_halfSize.w + 0.5f;
****************************************************************************/
void CSoftwareRender::Render( const CMatrix & matrix, const uint vertCount, const uint indexCount, uint textId, uint vboId, uint iboId )
{
    CSRTexture * pText = GetTexture( textId );
    CVertex2D * pVert = (CVertex2D *)GetVBO( vboId );
    uint * pIBO = GetIBO( iboId );

    CVertex2D * pTrans = new CVertex2D[vertCount];

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

    // Collect surviving triangles for strip-based rendering
    std::vector<CRender2d> triList;
    int triCount = indexCount / TRI;
    int vIndex(0);

    for( int i = 0; i < triCount; ++i )
    {
        CRender2d render2d( pText, &m_surfaceData );

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
                    CThreadPool::Instance().post( RenderStrip, &triList, yMin, yMax ) );
            }

            for( auto & fut : futures )
                fut.get();
        }
        else
        {
            // Fallback: single-threaded
            RenderStrip( &triList, 0, screenH );
        }
    }

    NDelFunc::DeleteArray( pTrans );

}   // Render


/***************************************************************************
*   desc:  Render all triangles within a horizontal screen strip
****************************************************************************/
void RenderStrip( const std::vector<CRender2d> * pTriList, int yMin, int yMax )
{
    for( const auto & tri : *pTriList )
        RenderTriStrip( tri, yMin, yMax );

}   // RenderStrip


/***************************************************************************
*   desc:  Render a single triangle, only writing scanlines in [yMin, yMax)
****************************************************************************/
void RenderTriStrip( const CRender2d & render, int yMin, int yMax )
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
    uint fixStepU, fixStepV, fixU, fixV;
    float u, v, stepU, stepV, step;
    float z, stepZ;
    uint * pDBuffer;
    float * pZBuffer;

    // Fixed point shift amount needed for UV
    const uint UV_SHIFT(20);

    // Setup local variables for faster access to data
    uint screenW( render.m_pSurface->w );
    uint textureW( render.m_pText->m_size.w );
    uint textureH( render.m_pText->m_size.h );
    uint * pPixels = (uint *)render.m_pSurface->pixels;
    float * pZBuf = render.m_pSurface->zBuffer;
    bool zEnabled = render.m_pSurface->zBufferEnabled;
    uint * pText = (uint *)render.m_pText->m_pData;

    // Calculate if we need uv plotting correction. .5 is needed for odd sizes
    float uOffset( (textureW % 2) ? 0.5f : 0.f );
    float vOffset( (textureH % 2) ? 0.5f : 0.f );

    // Create the range check variables
    uint uvOffsetMax = render.m_pText->m_size.w * render.m_pText->m_size.h;
    uint uvOffset;

    uint scrnOffsetMax = render.m_pSurface->w * render.m_pSurface->h;
    uint scrnOffset;

    // Loop to find the top vert of the triangle to extablish vertex order
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
                    scrnOffset = (leftSlope.y * screenW) + xStart;
                    pDBuffer = pPixels + scrnOffset;

                    // Init the fix point varaibles for speedy rendering
                    fixStepU = stepU * float( 1 << UV_SHIFT );
                    fixStepV = stepV * float( 1 << UV_SHIFT );
                    fixU = (u + uOffset) * float( 1 << UV_SHIFT );
                    fixV = (v + vOffset) * float( 1 << UV_SHIFT );

                    if( zEnabled )
                    {
                        z = leftSlope.m_slope.vert.z;
                        stepZ = (rightSlope.m_slope.vert.z - z) / width;

                        if( xStart == 0 && leftSlope.m_slope.vert.x < 0 )
                            z += stepZ * (-leftSlope.m_slope.vert.x);

                        pZBuffer = pZBuf + scrnOffset;

                        while( width-- > 0 )
                        {
                            uvOffset = ((fixV >> UV_SHIFT) * textureW) + (fixU >> UV_SHIFT);

                            if( (uvOffset < uvOffsetMax) && (scrnOffset < scrnOffsetMax) && z < *pZBuffer )
                            {
                                *pDBuffer = *(pText + uvOffset);
                                *pZBuffer = z;
                            }

                            ++pDBuffer;
                            ++pZBuffer;
                            ++scrnOffset;
                            fixU += fixStepU;
                            fixV += fixStepV;
                            z += stepZ;
                        }
                    }
                    else
                    {
                        while( width-- > 0 )
                        {
                            uvOffset = ((fixV >> UV_SHIFT) * textureW) + (fixU >> UV_SHIFT);

                            // Rotation can cause reading and writing outside of the range of our buffers
                            // Do this check to insure we are within range
                            if( (uvOffset < uvOffsetMax) && (scrnOffset < scrnOffsetMax) )
                                *pDBuffer = *(pText + uvOffset);

                            ++pDBuffer;
                            ++scrnOffset;
                            fixU += fixStepU;
                            fixV += fixStepV;
                        }
                    }
                }
            }

            leftSlope.Inc();
            rightSlope.Inc();
        }
    }

}   // RenderTriStrip
