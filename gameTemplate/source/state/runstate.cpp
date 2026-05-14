
/************************************************************************
*    FILE NAME:       runstate.cpp
*
*    DESCRIPTION:     CRunState Class State
************************************************************************/

// Physical component dependency
#include "runstate.h"

// Game lib dependencies
#include <gui/menumanager.h>
#include <utilities/highresolutiontimer.h>
#include <utilities/statcounter.h>

/************************************************************************
*    desc:  Constructer                                                             
************************************************************************/
CRunState::CRunState()
    : m_changeState(false)
{
    // Set the game state for this object
    m_gameState = EGS_RUN;

    // Clear out any active trees and set the active one and activate the root menu
    CMenuManager::Instance().ClearActiveTrees();
    CMenuManager::Instance().ActivateTree("pause");

}	// Constructer


/************************************************************************
*    desc:  Handle events
************************************************************************/
void CRunState::HandleEvent( const SDL_Event & rEvent )
{
    CCommonState::HandleEvent( rEvent );

    // Check for the "change state" message
    if( rEvent.type == NMenu::EGE_MENU_BACK_TO_MAIN_MENU )
    {
        m_stateMessage.Clear();
        m_stateMessage.m_nextState = EGS_TITLE_SCREEN;
        m_stateMessage.m_lastState = EGS_RUN;

        // Load the object data
        m_stateMessage.m_groupLoad.push_back("(title_screen)");

        m_nextState = EGS_GAME_LOAD;

        m_changeState = true;
    }

}   // HandleEvent


/***************************************************************************
*    desc:  Check for collision and react to it
****************************************************************************/
bool CRunState::DoStateChange()
{
    return m_changeState;

}   // DoStateChange


/***************************************************************************
*    desc:  Check for collision and react to it
****************************************************************************/
void CRunState::ReactToCollision()
{
    CCommonState::ReactToCollision();

}	// ReactToCollision


/***************************************************************************
*    desc:  Update objects that require them
****************************************************************************/
void CRunState::Update()
{
    CCommonState::Update();

}	// Update


/***************************************************************************
*    desc:  Transform the game objects
****************************************************************************/
void CRunState::Transform()
{
    CCommonState::Transform();

}	// Transform


/***************************************************************************
*    desc:  Do the 2D rendering
****************************************************************************/
void CRunState::PreRender()
{
    CCommonState::PreRender();

}	// PreRender






