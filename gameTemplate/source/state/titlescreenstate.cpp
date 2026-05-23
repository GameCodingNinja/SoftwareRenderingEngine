
/************************************************************************
*    FILE NAME:       titlescreenstate.cpp
*
*    DESCRIPTION:     Class template
************************************************************************/

// Physical component dependency
#include "titlescreenstate.h"

// Game lib dependencies
#include <objectdata/objectdata2d.h>
#include <objectdata/objectdatamanager.h>
#include <system/device.h>
#include <utilities/highresolutiontimer.h>


/************************************************************************
*    desc:  Constructer                                                             
************************************************************************/
CTitleScreenState::CTitleScreenState()
    : m_background( CObjectDataMgr::Instance().getData( "(title_screen)", "background" ) ),
      m_background2( CObjectDataMgr::Instance().getData( "(title_screen)", "background2" ) ),
      m_background3( CObjectDataMgr::Instance().getData( "(title_screen)", "background2" ) ),
      m_enemy( CObjectDataMgr::Instance().getData( "(title_screen)", "enemy" ) ),
      m_logo( CObjectDataMgr::Instance().getData( "(logo)", "logo" ) ),
      m_allowStateChange(false)
{
    // Set the game states for this object
    m_gameState = EGS_TITLE_SCREEN;
    m_nextState = EGS_GAME_LOAD;

    //m_background.setRot( CPoint<float>(0,0,65) );
    //m_background.setPos( CPoint<float>(0,-400,0) );
    //m_background.setPos( CPoint<float>(-400,0,0) );
    //m_background.setScale( CPoint<float>(.5,.5,0) );

    m_background.setPos( CPoint<float>( -50,0,0) );
    m_background2.setPos( CPoint<float>( 300,-300,0) );
    m_background3.setPos( CPoint<float>( 300,300,0) );
    m_enemy.setPos( CPoint<float>( -200,0,0) );
    m_logo.setPos( CPoint<float>( 0, 0, -6) );
    m_logo.setScale( CPoint<float>(2,2, 2) );

    m_background.setRot( CPoint<float>(180,0,0) );

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
    //m_background.incRot( CPoint<float>(0,0,0.2f * elapsed) );

    m_logo.incRot( CPoint<float>(0, 0.1f * elapsed, 0) );

}	// Update


/***************************************************************************
*    desc:  Transform the game objects
****************************************************************************/
void CTitleScreenState::Transform()
{
    CCommonState::Transform();

    m_background.transform();
    m_background2.transform();
    m_background3.transform();
    m_enemy.transform();
    m_logo.transform();

}	// Transform */


/***************************************************************************
*    desc:  2D/3D Render of game content
****************************************************************************/
void CTitleScreenState::render()
{
    // Render common content (menus, UI, etc.)
    CCommonState::render();

    const CMatrix & matrix = CDevice::Instance().GetProjectionMatrix( NDefs::EPT_ORTHOGRAPHIC );

    m_background.render( matrix );
    m_background2.render( matrix );
    m_background3.render( matrix );
    m_enemy.render( matrix );

    //const CMatrix & matrix3d = CDevice::Instance().GetProjectionMatrix( NDefs::EPT_PERSPECTIVE );
    //m_logo.render( matrix3d );

}	// Render


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




