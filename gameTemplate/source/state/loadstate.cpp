
/************************************************************************
*    FILE NAME:       loadstate.cpp
*
*    DESCRIPTION:     Class template
************************************************************************/

// Physical component dependency
#include "loadstate.h"

// SDL lib dependencies
#include <SDL.h>

// Standard lib dependencies
#include <string>
#include <vector>

// Boost lib dependencies
#include <boost/format.hpp>

// Game lib dependencies
#include <system/device.h>
#include <objectdata/objectdatamanager2d.h>
#include <utilities/highresolutiontimer.h>
#include <utilities/settings.h>
#include <utilities/xmlParser.h>
#include <utilities/xmlparsehelper.h>
#include <utilities/genfunc.h>

// Game dependencies
#include "loadscreenanimationinfo.h"
//#include "../game/savefile.h"

/************************************************************************
*    desc:  Constructer                                                             
************************************************************************/
CLoadState::CLoadState( const CStateMessage & stateMsg )
    : m_exitThread(false)
{
    // Set the game state for this object
    m_gameState = EGS_GAME_LOAD;

    // Copy over the message to react to
    m_stateMessage = stateMsg;

}	// Constructer


/***************************************************************************
*    desc:  Animate from thread durring the load
****************************************************************************/
int CLoadState::Animate()
{
    /*try
    {
        // Get window surface
        //SDL_Surface * pScreenSurface = SDL_GetWindowSurface( CDevice::Instance().GetWindow() );
        SDL_Surface * pScreenSurface = CDevice::Instance().GetSurface();
        if( pScreenSurface == NULL )
            throw NExcept::CCriticalException("Surface Creation error!", SDL_GetError());

        // Clear the surface
        SDL_FillRect( pScreenSurface, NULL, 0 );
        SDL_UpdateWindowSurface( CDevice::Instance().GetWindow() );

        // Clear the surface
        SDL_FillRect( pScreenSurface, NULL, 0 );

        // open this file and parse
        XMLNode mainNode = XMLNode::openFileHelper( "data/objects/2d/loadscreens/gameLoadScreen.cfg", "loadscreen" );
        XMLNode bitmapNode = mainNode.getChildNode( "bitmap" );

        const std::string file = bitmapNode.getAttribute( "file" );
        const int frameCount = std::atoi(bitmapNode.getAttribute( "count" ));
        const float fps = std::atoi(bitmapNode.getAttribute( "fps" ));

        CPoint<float> pos = NParseHelper::LoadPosition( bitmapNode );
        CPoint<float> scale = NParseHelper::LoadScale( bitmapNode );

        // Load the bitmaps to display
        std::vector<SDL_Surface *> pSurfaceVec;
        for( int i = 0; i < frameCount; ++i )
        {
            // Create a bitmap surface
            SDL_Surface * pBitmapSurface = SDL_LoadBMP( boost::str( boost::format( file ) % i ).c_str() );
            if( pBitmapSurface == NULL )
                NGenFunc::PostDebugMsg( "Surface Creation error! - " + std::string(SDL_GetError()) );

            // Convert the surface to match the screen surface for faster blitting - not that I think it really matters due to the limited use of blitting
            pSurfaceVec.push_back( SDL_ConvertSurface( pBitmapSurface, pScreenSurface->format, 0 ) );
            if( pSurfaceVec.back() == NULL )
                NGenFunc::PostDebugMsg( "Conversion Creation error! - " + std::string(SDL_GetError()) );

            // Clean up the surface that's no longer needed
            SDL_FreeSurface( pBitmapSurface );
        }

        // Calculate the rect position and scaled size
        SDL_Rect rect;
        rect.w = pSurfaceVec.back()->w * CSettings::Instance().GetScreenRatio().GetH() * scale.x;
        rect.h = pSurfaceVec.back()->h * CSettings::Instance().GetScreenRatio().GetH() * scale.y;
        rect.x = pScreenSurface->w - rect.w - pos.x;
        rect.y = pScreenSurface->h - rect.h - pos.y;

        // Calc fps
        const float time = 1000.0 / fps;
        float timer = -1;

        int counter(0);
        do
        {
            // Get the elapsed time
            CHighResTimer::Instance().CalcElapsedTime();

            timer -= CHighResTimer::Instance().GetElapsedTime();

            if( timer < 0 )
            {
                SDL_BlitScaled( pSurfaceVec[counter], NULL, pScreenSurface, &rect );
                SDL_UpdateWindowSurface( CDevice::Instance().GetWindow() );

                counter = (counter + 1) % frameCount;

                timer = time;
            }

            SDL_Delay( 2 );
        }
        while( !m_exitThread );

        // Clear the surface
        SDL_FillRect( pScreenSurface, NULL, 0 );
        SDL_UpdateWindowSurface( CDevice::Instance().GetWindow() );

        // Clean up
        for( size_t i = 0; i < pSurfaceVec.size(); ++i )
            SDL_FreeSurface( pSurfaceVec[i] );
    }
    catch( NExcept::CCriticalException & ex )
    {
        NGenFunc::PostDebugMsg( ex.GetErrorTitle() + " - " + ex.GetErrorMsg() );
    }
    catch( std::exception const & ex )
    {
        NGenFunc::PostDebugMsg( ex.what() );
    }
    catch(...)
    {
        NGenFunc::PostDebugMsg( "Unknown error when animating loading screen" );
    }*/

    return thread::EXIT_CODE;

}	// Animate


/***************************************************************************
*    desc:  Load from thread during the loading screen
****************************************************************************/
void CLoadState::Load()
{
    // Start the animated loading thread
    //m_loadAnimThread.Start( this, &CLoadState::Animate, "loadStateThread" );

    // ----------------- UNLOAD ASSETS SECTION -----------------

    // Unload any data groups
    //for( size_t i = 0; i < m_stateMessage.m_groupUnload.size(); ++i )
        //CObjectDataMgr2D::Instance().FreeGroup( m_stateMessage.m_groupUnload[i] );




    // ------------------ LOAD ASSETS SECTION ------------------

    // Load any data groups
    //for( size_t i = 0; i < m_stateMessage.m_groupLoad.size(); ++i )
        //CObjectDataMgr2D::Instance().LoadGroup( m_stateMessage.m_groupLoad[i] );


    // Tell the thread it's time to finish
    //m_exitThread = true;

    // Wait for the thread to finish
    //m_loadAnimThread.WaitForThread();
    

    /*

    Old DirectX code

    // Free the stages
    CStage2D::Instance().ClearStage();

    // Free all the actor data
    CActorManager::Instance().ClearAll();

    // Clear out the actor data
    CActorDataList::Instance().Clear();

    // Free the instance mesh data
    CInstanceMeshManager::Instance().Clear();

    // Free the hud
    CHudManager::Instance().Clear();

    // Clear all the Mega Textures
    CMegaTextureManager::Instance().Clear();

    // Clear all the generators
    CGeneratorMgr::Instance().Clear();

    // Unload any data groups
    for( size_t i = 0; i < stateMessage.groupUnload.size(); ++i )
        CObjectDataList2D::Instance().FreeDataGroup( stateMessage.groupUnload[i] );


    // Load any data groups
    for( size_t i = 0; i < stateMessage.groupLoad.size(); ++i )
        CObjectDataList2D::Instance().LoadDataGroup( stateMessage.groupLoad[i] );

    // Create any mega textures
    for( size_t i = 0; i < stateMessage.createMegaTexture.size(); ++i )
        CMegaTextureManager::Instance().CreateMegaTexture( 
            stateMessage.createMegaTexture[i].get<0>(),
            stateMessage.createMegaTexture[i].get<1>() );

    // Create any instance Meshes
    for( size_t i = 0; i < stateMessage.createInstanceMesh.size(); ++i )
        CInstanceMeshManager::Instance().CreateInstanceMesh( 
            stateMessage.createInstanceMesh[i] );

    // Load up the actor data list
    if( !stateMessage.actorDataLoad.empty() )
        CActorDataList::Instance().LoadFromXML( stateMessage.actorDataLoad );

    // Create any actors
    for( size_t i = 0; i < stateMessage.createActor.size(); ++i )
        CActorManager::Instance().CreateActor2D( stateMessage.createActor[i] );

    // Create any actor vectors
    for( size_t i = 0; i < stateMessage.createActorVec.size(); ++i )
        CActorManager::Instance().CreateActorVec2D( stateMessage.createActorVec[i] );

    // Load the 2d stage
    if( !stateMessage.stage2DLoad.empty() )
        CStage2D::Instance().LoadStage( stateMessage.stage2DLoad );

    // Create the generators
    if( !stateMessage.generatorLoadPath.empty() )
    {
        // Get the position of the player as the new focus position
        CGeneratorMgr::Instance().CreateGenerators( 
            stateMessage.generatorLoadPath,
            CActorManager::Instance().GetActor2D( "player_ship" )->GetPos().GetIntComponent() );
    }

    // Save the game file
    if( (stateMessage.nextState == EGS_TITLE_SCREEN) &&
        (stateMessage.lastState == EGS_RUN))
    {
        CSaveFile::Instance().Save();
    }*/

}	// Load


/***************************************************************************
*    desc:  Is the state done
****************************************************************************/
bool CLoadState::DoStateChange()
{
    Load();

    return true;

}	// DoStateChange


/***************************************************************************
*    desc:  Get the next state to load
****************************************************************************/
EGameState CLoadState::GetNextState()
{
    return m_stateMessage.m_nextState;

}	// GetNextState





