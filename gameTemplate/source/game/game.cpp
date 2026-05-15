
/************************************************************************
*    FILE NAME:       game.cpp
*
*    DESCRIPTION:     CGame class
************************************************************************/

// Physical component dependency
#include "game.h"

// Game lib dependencies
#include <system/device.h>
#include <utilities/genfunc.h>
#include <utilities/exceptionhandling.h>
#include <utilities/settings.h>

// Game dependencies
#include "../state/startupstate.h"
#include "../state/titlescreenstate.h"

/************************************************************************
*    desc:  Constructer
************************************************************************/
CGame::CGame()
{
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

        if( nextState == EGS_TITLE_SCREEN )
            spGameState.reset( new CTitleScreenState );

        else
            throw NExcept::CCriticalException("Error Invalid game state",
                NGenFunc::FormatString("Next state not valid (cur %d, next %d).\n\n%s\nLine: %d", curState, nextState, __FUNCTION__, __LINE__));
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
