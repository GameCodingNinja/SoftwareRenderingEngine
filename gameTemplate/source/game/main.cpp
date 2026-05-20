
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

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
    (void)hInstance;
    (void)hPrevInstance;
    (void)lpCmdLine;
    (void)nCmdShow;

#else

int main( int argc, char* args[] )
{
    (void)argc;
    (void)args;

#endif

    CGame game;

    try
    {
        // Create the game
        game.Create();

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

    return 0;
}








