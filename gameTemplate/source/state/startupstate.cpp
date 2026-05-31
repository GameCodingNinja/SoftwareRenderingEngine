
/************************************************************************
*    FILE NAME:       startupstate.cpp
*
*    DESCRIPTION:     CStartUpState Class State
************************************************************************/

// Physical component dependency
#include "startupstate.h"

// Standard lib dependencies
#include <vector>

// Game lib dependencies
#include <objectdata/objectdatamanager.h>
#include <system/device.h>
#include <utilities/genfunc.h>
#include <utilities/highresolutiontimer.h>
#include <utilities/exceptionhandling.h>
#include <utilities/settings.h>
#include <managers/lightmanager.h>
#include <sound/soundmanager.h>

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
*    desc:  Load the assets
****************************************************************************/
void CStartUpState::Load()
{
    // Load all of the meshes and materials in these groups
    CObjectDataMgr::Instance().LoadListTable( "data/objects/2d/objectDataList/dataListTable.lst" );
    CObjectDataMgr::Instance().LoadListTable( "data/objects/3d/objectDataList/dataListTable.lst" );
    CObjectDataMgr::Instance().LoadGroup( "(title_screen)" );
    CObjectDataMgr::Instance().LoadGroup( "(3d)" );

    CLightMgr::Instance().LoadListTable( "data/objects/lightDataList/dataListTable.lst" );
    CLightMgr::Instance().LoadGroup( "(default)" );

    // Load the sound list table and initial sound group
    CSoundMgr::Instance().LoadListTable( "data/sound/soundListTable.lst" );
    CSoundMgr::Instance().loadGroup( "(menu)" );

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
