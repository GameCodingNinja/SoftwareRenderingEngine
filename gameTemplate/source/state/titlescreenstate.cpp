
/************************************************************************
*    FILE NAME:       titlescreenstate.cpp
*
*    DESCRIPTION:     Class template
************************************************************************/

// Physical component dependency
#include "titlescreenstate.h"

// Game lib dependencies
#include <objectdata/objectdatamanager2d.h>
#include <system/device.h>
#include <utilities/highresolutiontimer.h>


/************************************************************************
*    desc:  Constructer                                                             
************************************************************************/
CTitleScreenState::CTitleScreenState()
    : m_background( CObjectDataMgr2D::Instance().GetData( "(title_screen)", "background" ) ),
      m_background2( CObjectDataMgr2D::Instance().GetData( "(title_screen)", "background2" ) ),
      m_background3( CObjectDataMgr2D::Instance().GetData( "(title_screen)", "background2" ) ),
      m_allowStateChange(false)
{
    // Set the game states for this object
    m_gameState = EGS_TITLE_SCREEN;
    m_nextState = EGS_GAME_LOAD;

    //m_background.SetRot( CPoint<float>(0,0,65) );
    //m_background.SetPos( CPoint<float>(0,-400,0) );
    //m_background.SetPos( CPoint<float>(-400,0,0) );
    //m_background.SetScale( CPoint<float>(.5,.5,0) );

    m_background.SetPos( CPoint<float>( -50,0,0) );
    m_background2.SetPos( CPoint<float>( 300,-300,0) );
    m_background3.SetPos( CPoint<float>( 300,300,0) );

    //m_background.SetRot( CPoint<float>(0,180,0) );

}	// Constructer


/************************************************************************
*    desc:  Handle events
************************************************************************/
void CTitleScreenState::HandleEvent( const CEvent & rEvent )
{
    CCommonState::HandleEvent( rEvent );

    // Check for the "change state" message
    //if( rEvent.type == NMenu::EGE_MENU_GAME_STATE_CHANGE )
        //m_allowStateChange = true;

}   // HandleEvent


/***************************************************************************
*    desc:  Update objects that require them
****************************************************************************/
void CTitleScreenState::Update()
{
    CCommonState::Update();

    const float elapsed = CHighResTimer::Instance().GetElapsedTime();
    m_background.IncRot( CPoint<float>(0,0,0.2f * elapsed) );

}	// Update


/***************************************************************************
*    desc:  Transform the game objects
****************************************************************************/
void CTitleScreenState::Transform()
{
    CCommonState::Transform();

    m_background.Transform();
    m_background2.Transform();
    m_background3.Transform();

}	// Transform */


/***************************************************************************
*    desc:  2D/3D Render of game content
****************************************************************************/
void CTitleScreenState::PreRender()
{
    CCommonState::PreRender();

    const CMatrix & matrix = CDevice::Instance().GetProjectionMatrix( NDefs::EPT_ORTHOGRAPHIC );
    m_background.Render( matrix );
    m_background2.Render( matrix );
    m_background3.Render( matrix );

}	// Render2D


/***************************************************************************
*    desc:  Is the state done
****************************************************************************/
bool CTitleScreenState::DoStateChange()
{
    if( m_allowStateChange )
    {
        m_stateMessage.Clear();
        m_stateMessage.m_nextState = EGS_RUN;
        m_stateMessage.m_lastState = EGS_TITLE_SCREEN;

        // Unload the object data
        m_stateMessage.m_groupUnload.push_back("(title_screen)");

        return true;
    }

    return false;

}	// DoStateChange */




