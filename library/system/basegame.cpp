
/************************************************************************
*    FILE NAME:       basegame.cpp
*
*    DESCRIPTION:     base game class
************************************************************************/

// Physical component dependency
#include <system/basegame.h>

// SDL lib dependencies
#include <SDL.h>

// Standard lib dependencies
#include <stdio.h>

// Game lib dependencies
#include <utilities/exceptionhandling.h>
#include <utilities/settings.h>
#include <utilities/highresolutiontimer.h>
#include <utilities/statcounter.h>
#include <system/device.h>
#include <softwareRender/softwareRender.h>

/************************************************************************
*    desc:  Constructer
************************************************************************/
CBaseGame::CBaseGame()
    : m_pWindow(nullptr),
      //m_context(nullptr),
      m_gameRunning(false)
{
}   // constructor


/************************************************************************
*    desc:  destructer                                                             
************************************************************************/
CBaseGame::~CBaseGame()
{
    // Destroy the OpenGL contex
    //if( m_context != nullptr )
        //SDL_GL_DeleteContext( m_context );

    // Destroy window
    if( m_pWindow != nullptr )
        SDL_DestroyWindow( m_pWindow );

    // Quit SDL subsystems
    SDL_Quit();

}   // destructer


/***************************************************************************
*   desc:  Create the game Window
 ****************************************************************************/
void CBaseGame::Create()
{
    // Create the window and OpenGL context
    CDevice::Instance().Create();

    // Get local copies of the device handles
    m_pWindow = CDevice::Instance().GetWindow();
    //m_context = CDevice::Instance().GetContext();

    // Game start init
    Init();

}   // Create


/************************************************************************
*    desc:  Init the game
************************************************************************/
void CBaseGame::Init()
{
    // Show the window
    CDevice::Instance().ShowWindow( true );

    // Display a black screen
    CSoftwareRender::Instance().Clear();
    CSoftwareRender::Instance().Flip( m_pWindow );

}   // Init


/***************************************************************************
*   desc:  Poll for game events
****************************************************************************/
void CBaseGame::PollEvents()
{
    // Event handler
    SDL_Event msgEvent;

    // Handle events on queue
    while( SDL_PollEvent( &msgEvent ) )
    {
        // let the game handle the event
        // turns true on quit
        if( HandleEvent( msgEvent ) )
        {
            // Stop the game
            m_gameRunning = false;

            // Hide the window to give the impression of a quick exit
            CDevice::Instance().ShowWindow( false );

            break;
        }
    }
}   // PollEvents


/***************************************************************************
*   desc:  Main game loop
****************************************************************************/
bool CBaseGame::GameLoop()
{
    // Handle the state change
    DoStateChange();

    // Poll for game events
    PollEvents();

    // First thing we need to do is get our elapsed time
    CHighResTimer::Instance().CalcElapsedTime();

    if( m_gameRunning )
    {
        // Check for collision and react to it
        ReactToCollision();

        // Update animations, Move sprites, Check for collision
        Update();

        // Transform game objects
        Transform();

        // Do the rendering
        Render();

        // Inc the cycle
        CStatCounter::Instance().IncCycle( m_pWindow );
    }

    return m_gameRunning;

}   // GameLoop


/***************************************************************************
*   desc:  Do the rendering
****************************************************************************/
void CBaseGame::Render()
{
    //glClear( m_clearBufferMask );
    CSoftwareRender::Instance().Clear();

    // Do the pre render
    PreRender();

    // Do the post render
    PostRender();

    // Apparently it's a good practice to do this at the end of a render cycle
    //CShaderMgr::Instance().UnbindShaderProgram();
    //CTextureMgr::Instance().UnbindTexture();
    //CVertBufMgr::Instance().UnbindBuffers();

    // Do the back buffer swap
    CSoftwareRender::Instance().Flip( m_pWindow );
    //SDL_GL_SwapWindow( m_pWindow );

}   // Render


/***************************************************************************
*   desc:  Display error massage
****************************************************************************/
void CBaseGame::DisplayErrorMsg( const std::string & title, const std::string & msg )
{
    printf("Error: %s, %s", title.c_str(), msg.c_str() );

    SDL_ShowSimpleMessageBox( SDL_MESSAGEBOX_ERROR, title.c_str(), msg.c_str(), m_pWindow );

}   // DisplayErrorMsg


/***************************************************************************
*   desc:  Start the game
****************************************************************************/
void CBaseGame::StartGame()
{
    m_gameRunning = true;

}   // StartGame


/***************************************************************************
*   desc:  Stop the game
****************************************************************************/
void CBaseGame::StopGame()
{
    m_gameRunning = false;

}   // StopGame


/***************************************************************************
*  desc:  Is the game running?
*
*  ret: bool - true or false if game is running
****************************************************************************/
bool CBaseGame::IsGameRunning() const
{
    return m_gameRunning;

}   // IsGameRunning

