
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
#include <objectdata/objectdatamanager2d.h>
#include <system/device.h>
#include <utilities/genfunc.h>
#include <utilities/highresolutiontimer.h>
#include <utilities/exceptionhandling.h>
#include <utilities/settings.h>

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
    CObjectDataMgr2D::Instance().LoadListTable( "data/objects/2d/objectDataList/dataListTable.lst" );
    CObjectDataMgr2D::Instance().LoadGroup( "(title_screen)" );

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
