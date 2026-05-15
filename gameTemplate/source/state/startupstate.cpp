
/************************************************************************
*    FILE NAME:       startupstate.cpp
*
*    DESCRIPTION:     CStartUpState Class State
************************************************************************/

// Physical component dependency
#include "startupstate.h"

// SDL lib dependencies
#include <SDL.h>

// Standard lib dependencies
#include <vector>

// Game lib dependencies
#include <objectdata/objectdatamanager2d.h>
#include <system/device.h>
#include <utilities/genfunc.h>
#include <utilities/highresolutiontimer.h>
#include <utilities/exceptionhandling.h>
#include <utilities/settings.h>

// Game dependencies
#include "loadscreenanimationinfo.h"

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
