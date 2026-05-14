
/************************************************************************
*    FILE NAME:       classtemplate.cpp
*
*    DESCRIPTION:     Class template
************************************************************************/

// Physical component dependency
#include "startupstate.h"

// SDL lib dependencies
#include <SDL.h>

// Standard lib dependencies
#include <vector>

// Boost lib dependencies
#include <boost/format.hpp>

// Game lib dependencies
//#include <managers/hudmanager.h>
#include <managers/texturemanager.h>
#include <managers/vertexbuffermanager.h>
#include <managers/shadermanager.h>
#include <managers/fontmanager.h>
#include <managers/soundmanager.h>
#include <managers/actionmanager.h>
#include <managers/signalmanager.h>
#include <objectdata/objectdatamanager2d.h>
#include <script/scriptmanager.h>
#include <script/scriptcolor.h>
#include <script/scriptglobals.h>
#include <scriptstdstring/scriptstdstring.h>
#include <2d/spritescriptcomponent2d.h>
#include <system/device.h>
#include <gui/menumanager.h>
#include <gui/menu.h>
#include <gui/uicontrol.h>
#include <utilities/genfunc.h>
#include <utilities/highresolutiontimer.h>
#include <utilities/exceptionhandling.h>
#include <utilities/settings.h>
#include <utilities/xmlParser.h>

// Game dependencies
#include "loadscreenanimationinfo.h"
//#include "../game/savefile.h"

/************************************************************************
*    desc:  Constructer                                                             
************************************************************************/
CStartUpState::CStartUpState()
{
    // Set the game states for this object
    m_gameState = EGS_STARTUP;
    m_nextState = EGS_TITLE_SCREEN;

}	// Constructer


/***************************************************************************
*    desc:  Animate from thread durring the load
****************************************************************************/
int CStartUpState::Animate()
{
    /*try
    {
        // Get window surface
        //SDL_Surface * pScreenSurface = SDL_GetWindowSurface( CDevice::Instance().GetWindow() );
        SDL_Surface * pScreenSurface = CDevice::Instance().GetSurface();
        if( pScreenSurface == nullptr )
            throw NExcept::CCriticalException("Surface Creation error!", SDL_GetError());

        // Clear the surface
        SDL_FillRect( pScreenSurface, NULL, 0 );
        SDL_UpdateWindowSurface( CDevice::Instance().GetWindow() );

        // open this file and parse
        XMLNode mainNode = XMLNode::openFileHelper( "data/objects/2d/loadscreens/startupLoadScreen.cfg", "loadscreen" );

        // Load the bitmaps to display
        std::vector<CLoadScrnAnim> pSurfaceVec;
        for( int i = 0; i < mainNode.nChildNode(); ++i )
        {
            XMLNode bitmapNode = mainNode.getChildNode( "bitmap", i );

            pSurfaceVec.push_back( CLoadScrnAnim() );

            // Create a bitmap surface
            SDL_Surface * pBitmapSurface = SDL_LoadBMP( bitmapNode.getAttribute( "file" ) );
            if( pBitmapSurface == NULL )
                NGenFunc::PostDebugMsg( "Logo Surface Creation error! - " + std::string(SDL_GetError()) );

            // Convert the surface to match the screen surface for faster blitting - not that I think it really matters due to the limited use of blitting
            pSurfaceVec.back().pSurface = SDL_ConvertSurface( pBitmapSurface, pScreenSurface->format, 0 );
            if( pSurfaceVec.back().pSurface == NULL )
                NGenFunc::PostDebugMsg( "Logo Conversion Creation error! - " + std::string(SDL_GetError()) );

            // Clean up the surface that's no longer needed
            SDL_FreeSurface( pBitmapSurface );

            // Get the animation info
            pSurfaceVec.back().fadeTime = std::atoi(bitmapNode.getAttribute( "fadeTime" ));
            pSurfaceVec.back().displayDelay = std::atoi(bitmapNode.getAttribute( "displayDelay" ));
        }

        for( size_t i = 0; i < pSurfaceVec.size(); ++i )
        {
            // Calculate the rect position and scaled size
            SDL_Rect rect;
            rect.w = pSurfaceVec[i].pSurface->w * CSettings::Instance().GetScreenRatio().GetH();
            rect.h = pSurfaceVec[i].pSurface->h * CSettings::Instance().GetScreenRatio().GetH();
            rect.x = (pScreenSurface->w - rect.w) / 2;
            rect.y = (pScreenSurface->h - rect.h) / 2;

            SDL_Delay( 300 );

            FadeTo( pSurfaceVec[i].fadeTime, 0, 255, pSurfaceVec[i].pSurface, pScreenSurface, rect );

            SDL_Delay( pSurfaceVec[i].displayDelay );

            FadeTo( pSurfaceVec[i].fadeTime, 255, 0, pSurfaceVec[i].pSurface, pScreenSurface, rect );

            SDL_Delay( 300 );

            // Clear the surface
            SDL_FillRect( pScreenSurface, NULL, 0 );
            SDL_UpdateWindowSurface( CDevice::Instance().GetWindow() );
        }

        // Clean up
        for( size_t i = 0; i < pSurfaceVec.size(); ++i )
            SDL_FreeSurface( pSurfaceVec[i].pSurface );
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


/************************************************************************
*    desc:  Fade to color                                                             
************************************************************************/
void CStartUpState::FadeTo(
    float time, float current, float final, SDL_Surface * pSource, SDL_Surface * pTarget, SDL_Rect & rect )
{
    float inc = (final - current) / time;
    int last = -1;

    // Get the range caps
    float lowestValue, heighetValue;
    if( current < final )
        lowestValue = current;
    else
        lowestValue = final;

    if( current > final )
        heighetValue = current;
    else
        heighetValue = final;

    do
    {
        // Get the elapsed time
        CHighResTimer::Instance().CalcElapsedTime();

        time -= CHighResTimer::Instance().GetElapsedTime();
        current += inc * CHighResTimer::Instance().GetElapsedTime();

        // Cap it to these values
        if( current < lowestValue )
            current = lowestValue;
        else if( current > heighetValue )
            current = heighetValue;

        // Only blit if it has changed
        if( (int)current != last )
        {
            if( time < 0 )
                current = final;

            SDL_SetSurfaceColorMod( pSource, current, current, current );
            SDL_BlitScaled( pSource, NULL, pTarget, &rect );
            SDL_UpdateWindowSurface( CDevice::Instance().GetWindow() );
        }

        last = current;

        SDL_Delay( 2 );
    }
    while( time > 0 );

}   // FadeTo


/***************************************************************************
*    desc:  Load the assets
****************************************************************************/
void CStartUpState::Load()
{
    // Start the animated loading thread
    //m_loadAnimThread.Start( this, &CStartUpState::Animate, "startupStateThread" );

    // Load in any fonts
    //CFontMgr::Instance().LoadFromXML( "data/textures/fonts/font.lst" );

    // Shaders must always be loaded first because they are accessed from object data
    //CShaderMgr::Instance().LoadFromXML( "data/shaders/shader.cfg" );

    // Load all of the meshes and materials in these groups
    CObjectDataMgr2D::Instance().LoadListTable( "data/objects/2d/objectDataList/dataListTable.lst" );
    //CObjectDataMgr2D::Instance().LoadGroup("(menu)");
    CObjectDataMgr2D::Instance().LoadGroup( "(title_screen)" );

    // Load sound resources for the menu
    /*CSoundMgr::Instance().LoadListTable( "data/sound/soundListTable.lst" );
    CSoundMgr::Instance().LoadGroup("(menu)");

    // Creates the script manager and loads the list table
    CScriptManager::Instance().LoadListTable( "data/objects/2d/scripts/scriptListTable.lst" );

    // Register the script items
    RegisterStdString( CScriptManager::Instance().GetEnginePtr() );
    NScriptGlobals::Register( CScriptManager::Instance().GetEnginePtr() );
    NScriptColor::Register( CScriptManager::Instance().GetEnginePtr() );
    CSpriteScriptComponent2d::Register( CScriptManager::Instance().GetEnginePtr() );

    // Load group specific script items
    CScriptManager::Instance().LoadGroup("(menu)");

    // Create the menu system
    CMenuManager::Instance().LoadFromXML( "data/objects/2d/menu/tree.list" );

    // Init the currently plugged in game controllers
    CDevice::Instance().InitStartupGamepads();

    // Load the action manager
    CActionManager::Instance().LoadActionFromXML( "data/settings/controllerMapping.cfg" );*/

    // Wait for the thread to finish
    //m_loadAnimThread.WaitForThread();

}   // Load


/***************************************************************************
*    desc:  Is the state done
****************************************************************************/
bool CStartUpState::DoStateChange()
{
    // load game assets
    Load();

    return true;

}	// DoStateChange

