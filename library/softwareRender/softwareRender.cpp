
/************************************************************************
*    FILE NAME:       softwareRender.cpp
*
*    DESCRIPTION:     Software Rendering device
************************************************************************/

// Physical component dependency
#include <softwareRender/softwareRender.h>

// SDL lib dependencies
#include <SDL.h>

// Standard lib dependencies

// Boost lib dependencies
#include <boost/format.hpp>
#include <boost/make_shared.hpp>
#include <boost/bind.hpp>

// Game lib dependencies
#include <common/matrix.h>
#include <utilities/exceptionhandling.h>
#include <utilities/exceptionhandling.h>
#include <utilities/deletefuncs.h>
#include <softwareRender/srtexture.h>
#include <softwareRender/triangleslope.h>
#include <softwareRender/renderdefs.h>

// Render the triangle
int RenderTri( CRender2d * pRender );

/************************************************************************
*    desc:  Constructor
************************************************************************/
CSoftwareRender::CSoftwareRender() :
    m_pSurface(nullptr),
    m_textIdInc(0),
    m_vboIdInc(0),
    m_iboIdInc(0),
    m_upThreadGroup( new boost::thread_group )
{
    // This has to be created before creating the threads otherwise the threads
    // will fall through and you'll be waiting for threads that don't exist
    m_upAsioWork.reset( new boost::asio::io_service::work(m_asioService) );

    // Create a thread pool that with the same number of threads as cores
    for( size_t i = 0; i < boost::thread::hardware_concurrency() ; ++i )
	    m_upThreadGroup->create_thread(
            boost::bind( &boost::asio::io_service::run, &m_asioService ));

}   // constructor


/************************************************************************
*    desc:  destructor                                                             
************************************************************************/
CSoftwareRender::~CSoftwareRender()
{
    m_asioService.stop();
    m_upThreadGroup->join_all();
    m_upAsioWork.reset();
    m_upThreadGroup.reset();

    NDelFunc::DeleteMapPointers(m_pTextureMap);
    NDelFunc::DeleteMapArrayPointers(m_pVBOMap);
    NDelFunc::DeleteMapArrayPointers(m_pIBOMap);

}	// destructor


/***************************************************************************
*   desc:  Create the SDL window surface
****************************************************************************/
void CSoftwareRender::CreateSurface( SDL_Window * pWindow )
{
    // Create the window surface. We don't own the surface so DON'T FREE it
    if( m_pSurface == nullptr )
    {
        if( pWindow == nullptr )
            throw NExcept::CCriticalException("Game window has not be created!", SDL_GetError() );
        
        m_pSurface = SDL_GetWindowSurface( pWindow );
        if( m_pSurface == nullptr )
            throw NExcept::CCriticalException("Surface Creation error!", SDL_GetError());

        m_halfScreen.w = m_pSurface->w / 2;
        m_halfScreen.h = m_pSurface->h / 2;
    }

}   // CreateSurface


/***************************************************************************
*   desc:  Get the SDL window surface
****************************************************************************/
SDL_Surface * CSoftwareRender::GetSurface()
{
    return m_pSurface;

}   // GetSurface


/***************************************************************************
*   desc:  Display the rendered changes
****************************************************************************/
void CSoftwareRender::Flip( SDL_Window * pWindow )
{
    SDL_UpdateWindowSurface( pWindow );

}   // Flip


/***************************************************************************
*   desc:  Clear the buffers
****************************************************************************/
void CSoftwareRender::Clear()
{
    SDL_FillRect( m_pSurface, NULL, 0 );

}   // Clear


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
            boost::str( boost::format("Unable to find texture Id (%d).\n\n%s\nLine: %s") % Id % __FUNCTION__ % __LINE__ ));
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
            boost::str( boost::format("Unable to find VBO Id (%d).\n\n%s\nLine: %s") % Id % __FUNCTION__ % __LINE__ ));
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
            boost::str( boost::format("Unable to find IBO Id (%d).\n\n%s\nLine: %s") % Id % __FUNCTION__ % __LINE__ ));
    }

    return nullptr;

}   // GetIBO


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

    // Get the number of triangles used
    int triCount = indexCount / TRI;
    int vIndex(0);

    for( int i = 0; i < triCount; ++i )
    {
        // Allocate the render data be used
        CRender2d * pRender2d = new CRender2d( pText, m_pSurface );

        // Copy over the verts for this triangle
        for( int j = 0; j < TRI; ++j )
            pRender2d->m_vec[j] = pTrans[ pIBO[vIndex++] ];

        // Should this be culled?
        if( pRender2d->Cull( m_pSurface->w, m_pSurface->h ) )
            // Can't see this triangle so delete it
            NDelFunc::Delete( pRender2d );
        else
            // Add this triangle to the thread job pool
            PushJob( pRender2d );
            //RenderTri( pRender2d );
    }

    if( !m_pendingJobs.empty() )
    {
        boost::wait_for_all(m_pendingJobs.begin(), m_pendingJobs.end());
        m_pendingJobs.clear();
    }

    NDelFunc::DeleteArray( pTrans );

}   // Render


/***************************************************************************
*   desc:  Push the job onto the worker threads
****************************************************************************/
void CSoftwareRender::PushJob( CRender2d * pRender2d )
{
    typedef boost::packaged_task<int> task_t;
    boost::shared_ptr<task_t> task = boost::make_shared<task_t>(boost::bind(&RenderTri, pRender2d));
    boost::shared_future<int> fut(task->get_future());
    m_pendingJobs.push_back(fut);
    m_asioService.post(boost::bind(&boost::packaged_task<int>::operator(), task));

}   // PushJob


/***************************************************************************
*   desc:  Render the triangle
****************************************************************************/
int RenderTri( CRender2d * pRender )
{
    // Define all the variables up here for speed reasons.
    int xStart, xEnd, width, height, slopeCount(TRI);
    uint fixStepU, fixStepV, fixU, fixV;
    float u, v, stepU, stepV, step;
    uint * pDBuffer;

    // Fixed point shift amount needed for UV
    const uint UV_SHIFT(20);

    // Setup local variables for faster access to data
    uint screenW( pRender->m_pSurface->w );
    uint screenH( pRender->m_pSurface->h );
    uint textureW( pRender->m_pText->m_size.w );
    uint textureH( pRender->m_pText->m_size.h );
    uint * pPixels = (uint *)pRender->m_pSurface->pixels;
    uint * pText = (uint *)pRender->m_pText->m_pData;

    // Calculate if we need uv plotting correction. .5 is needed for odd sizes
    float uOffset( (textureW % 2) ? 0.5f : 0.f );
    float vOffset( (textureH % 2) ? 0.5f : 0.f );

    // Create the range check variables
    uint uvOffsetMax = pRender->m_pText->m_size.w * pRender->m_pText->m_size.h;
    uint uvOffset;

    uint scrnOffsetMax = pRender->m_pSurface->w * pRender->m_pSurface->h;
    uint scrnOffset;

    // Loop to find the top vert of the triangle to extablish vertex order
    int vTop(0);
    for( int i = 1; i < TRI; ++i )
        if( pRender->m_vec[i].vert.y < pRender->m_vec[vTop].vert.y )
            vTop = i;

    // Init the slope class for managing the scan lines
    CTriangleSlope leftSlope( pRender->m_vec, vTop, CTriangleSlope::EST_LEFT );
    CTriangleSlope rightSlope( pRender->m_vec, vTop, CTriangleSlope::EST_RIGHT );
    
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

            // Make sure the height and the segment offset
            // don't go off the bottom of the screen
            if( (leftSlope.y + height) >= (int)screenH )
            {
                // Reset the height because it will exceed
                // the height of the buffer
                height = screenH - leftSlope.y;

                // Even if there are more edges, we can't see
                // them so set the edge count to zero because 
                // this is that last render height we can do
                slopeCount = 0;
            }
        }
        else
        {
            // Number of scan lines to fill
            height = rightSlope.m_length;

            // Make sure the height and the segment offset
            // don't go off the bottom of the screen
            if( (rightSlope.y + height) >= (int)screenH )
            {
                // Reset the height because it will exceed
                // the height of the buffer
                height = screenH - rightSlope.y;

                // Even if there are more edges, we can't see
                // them so set the edge count to zero because 
                // this is that last render height we can do
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
            // Calculate initial values
            xStart = leftSlope.m_slope.vert.x;
            xEnd = rightSlope.m_slope.vert.x;
            width = xEnd - xStart;

            // Make sure we are within the bounds of the screen
            if( ( width > 0 ) && ( xEnd > 0 ) && ( xStart < (int)screenW ) && (leftSlope.y < (int)screenH) )
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
                
                //u += uOffset;
                //v += vOffset;

                while( width-- > 0 )
                {
                    //uvOffset = (int(v)  * textureW) + int(u);
                    uvOffset = ((fixV >> UV_SHIFT) * textureW) + (fixU >> UV_SHIFT);

                    // Rotation can cause reading and writing outside of the range of our buffers
                    // Do this check to insure we are within range
                    if( (uvOffset < uvOffsetMax) && (scrnOffset < scrnOffsetMax) )
                        *pDBuffer = *(pText + uvOffset);

                    ++pDBuffer;
                    ++scrnOffset;
                    //u += stepU;
                    //v += stepV;
                    fixU += fixStepU;
                    fixV += fixStepV;
                }
            }

            leftSlope.Inc();
            rightSlope.Inc();
        }
    }

    // Clean up
    NDelFunc::Delete( pRender );

    // A return value is needed for rendering in a thread
    // even though this case doesn't need one.
    return 2;

}   // Render
