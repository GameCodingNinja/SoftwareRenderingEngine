
/************************************************************************
*    FILE NAME:       main.cpp
*
*    DESCRIPTION:     main class
************************************************************************/

// Game lib dependencies
#include <utilities/exceptionhandling.h>
#include <utilities/settings.h>
#include <system/device.h>

// Game dependencies
#include "game.h"

// Game lib dependencies
#include <common/worldvalue.h>

// Game-specific sector size
const int CWorldValue::SECTOR_SIZE(1024);
const int CWorldValue::HALF_SECTOR_SIZE( CWorldValue::SECTOR_SIZE / 2 );

int main( int argc, char* args[] )
{
    CGame game;

    try
    {
        // Load the settings file
        CSettings::Instance().LoadFromXML("data/settings/settings.cfg");

        // Create the game
        game.Create();

        // Enable text input
        SDL_StartTextInput();

        // Call the game loop
        while( game.GameLoop() )
        { }
    }
    catch( NExcept::CCriticalException & ex )
    {
        game.DisplayErrorMsg( ex.GetErrorTitle(), ex.GetErrorMsg() );
    }
    catch( std::exception const & ex )
    {
        game.DisplayErrorMsg( "Standard Exception", ex.what() );
    }
    catch(...)
    {
        game.DisplayErrorMsg( "Unknown Error", "Something bad happened and I'm not sure what it was." );
    }

    // Disable text input
    SDL_StopTextInput();

    return 0;

}   // main








