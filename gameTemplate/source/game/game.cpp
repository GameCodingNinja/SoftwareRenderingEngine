
/************************************************************************
*    FILE NAME:       game.cpp
*
*    DESCRIPTION:     CGame class
************************************************************************/

// Physical component dependency
#include "game.h"

// Boost lib dependencies
#include <boost/bind.hpp>
#include <boost/format.hpp>

// Game lib dependencies
#include <system/device.h>
#include <managers/signalmanager.h>
#include <gui/menumanager.h>
#include <gui/uicontrol.h>
#include <utilities/exceptionhandling.h>
#include <utilities/settings.h>

// Game dependencies
#include "../state/startupstate.h"
#include "../state/titlescreenstate.h"
#include "../state/loadstate.h"
#include "../state/runstate.h"
#include "../smartGUI/smartconfirmbtn.h"
#include "../smartGUI/smartresolutionbtn.h"
#include "../smartGUI/smartapplysettingsbtn.h"
#include "../smartGUI/smartfullscreencheckbox.h"
#include "../smartGUI/smartvsynccheckbox.h"
#include "../smartGUI/smartdeadzoneslider.h"

/************************************************************************
*    desc:  Constructer
************************************************************************/
CGame::CGame()
{
    CSignalManager::Instance().Connect( boost::bind(&CGame::SmartGuiControlCreate, this, _1) );

}   // constructor


/************************************************************************
*    desc:  destructer                                                             
************************************************************************/
CGame::~CGame()
{
}	// destructer


/************************************************************************
*    desc:  Init the game
************************************************************************/
void CGame::Init()
{
    CBaseGame::Init();

    // Setup the message filtering
    SDL_SetEventFilter(FilterEvents, 0);

    // Create the startup state
    spGameState.reset( new CStartUpState );

    // Let the games begin
    StartGame();

}	// Init


/************************************************************************
*    desc:  Callback for when a smart gui control is created
************************************************************************/
void CGame::SmartGuiControlCreate( CUIControl * pUIControl )
{
    if( pUIControl->GetExecutionAction() == "confirmation_menu" )
        pUIControl->SetSmartGui( new CSmartConfirmBtn( pUIControl ) );

    else if( pUIControl->GetName() == "resolution_btn_lst" )
        pUIControl->SetSmartGui( new CSmartResolutionBtn( pUIControl ) );

    else if( pUIControl->GetName() == "settings_apply_btn" )
        pUIControl->SetSmartGui( new CSmartApplySettingsBtn( pUIControl ) );

    else if( pUIControl->GetName() == "full_screen_check_box" )
        pUIControl->SetSmartGui( new CSmartScrnCheckBox( pUIControl ) );

    else if( pUIControl->GetName() == "v-sync_check_box" )
            pUIControl->SetSmartGui( new CSmartVSyncCheckBox( pUIControl ) );

    else if( pUIControl->GetName() == "settings_dead_zone_slider" )
            pUIControl->SetSmartGui( new CSmartDeadZoneSlider( pUIControl ) );

}   // SmartGuiControlCreate


/***************************************************************************
*    decs:  Handle the state change
****************************************************************************/
void CGame::DoStateChange()
{
    if( spGameState->DoStateChange() )
    {
        // Get the game state we are moving to
        const EGameState curState = spGameState->GetState();

        // Get the game state we are moving to
        const EGameState nextState = spGameState->GetNextState();

        // Get the message to the next state
        const CStateMessage stateMessage = spGameState->GetStateMessage();
        
        if( nextState == EGS_TITLE_SCREEN )
            spGameState.reset( new CTitleScreenState );

        else if( nextState == EGS_GAME_LOAD )
            spGameState.reset( new CLoadState( stateMessage ) );

        else if( nextState == EGS_RUN )
            spGameState.reset( new CRunState );

        else
            throw NExcept::CCriticalException("Error Invalid game state",
                boost::str( boost::format("Next state not valid (cur %d, next %d).\n\n%s\nLine: %s") % curState % nextState % __FUNCTION__ % __LINE__ ));
    }

}   // DoStateChange */


/************************************************************************
*    desc:  Handle events
************************************************************************/
bool CGame::HandleEvent( const SDL_Event & rEvent )
{
    if( rEvent.type == SDL_QUIT )
        return true;

    // Filter out these events. Can't do this through the normal event filter
    if( (rEvent.type >= SDL_JOYAXISMOTION) && (rEvent.type <= SDL_JOYBUTTONUP) )
        return false;

    else if( rEvent.type == SDL_CONTROLLERDEVICEADDED )
        CDevice::Instance().AddGamepad( rEvent.cdevice.which );

    else if( rEvent.type == SDL_CONTROLLERDEVICEREMOVED )
        CDevice::Instance().RemoveGamepad( rEvent.cdevice.which );

    // Handle events
    spGameState->HandleEvent( rEvent );

    return false;

}	// HandleEvent


/************************************************************************
*    desc:  Check for collision and react to it.
************************************************************************/
void CGame::ReactToCollision()
{

}	// ReactToCollision


/***************************************************************************
*    decs:  Update animations
****************************************************************************/
void CGame::Update()
{
    spGameState->Update();

}	// Update


/***************************************************************************
*    desc:  Transform game objects
****************************************************************************/
void CGame::Transform()
{
    spGameState->Transform();

}	// Transform


/***************************************************************************
*    decs:  3D/2D Render of game content
****************************************************************************/
void CGame::PreRender()
{
    spGameState->PreRender();

}	// GameRender3D


/***************************************************************************
*    decs:  3D/2D Render of content after post process effects
****************************************************************************/
void CGame::PostRender()
{
    spGameState->PostRender();

}	// PostGameRender2D


/***************************************************************************
*    decs:  Filter out events we don't want
****************************************************************************/
static int SDLCALL FilterEvents( void * userdata, SDL_Event * pEvent )
{
    // Return 0 to indicate that the event should be removed from the event queue

    // Do our own deadzone filtering
    if( pEvent->type == SDL_CONTROLLERAXISMOTION )
    {
        // Analog stick max values -32768 to 32767
        const int deadZone = CSettings::Instance().GetGamePadStickDeadZone() * 
            NDefs::ANALOG_PERCENTAGE_CONVERTION;

        if( ((pEvent->caxis.axis >= SDL_CONTROLLER_AXIS_LEFTX) &&
            (pEvent->caxis.axis <= SDL_CONTROLLER_AXIS_RIGHTY)) &&
            (((pEvent->caxis.value >= 0) && (pEvent->caxis.value < deadZone)) ||
            ((pEvent->caxis.value <= 0) && (pEvent->caxis.value > -deadZone))) )
            return 0;
    }

    // Return 1 to indicate that the event should stay in the event queue
    return 1;

}   // FilterEvents
