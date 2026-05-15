
/************************************************************************
*    FILE NAME:       basegame.cpp
*
*    DESCRIPTION:     base game class
************************************************************************/

// Physical component dependency
#include <system/basegame.h>

// Standard lib dependencies
#include <stdio.h>

// Game lib dependencies
#include <utilities/exceptionhandling.h>
#include <utilities/genfunc.h>
#include <utilities/settings.h>
#include <utilities/highresolutiontimer.h>
#include <utilities/statcounter.h>
#include <system/device.h>
#include <system/iwindow.h>
#include <system/iframebuffer.h>
#include <system/eventqueue.h>

/************************************************************************
*    desc:  Constructer
************************************************************************/
CBaseGame::CBaseGame()
    : m_pWindow(nullptr),
      m_pFrameBuffer(nullptr),
      m_gameRunning(false)
{
}   // constructor


/************************************************************************
*    desc:  destructer                                                             
************************************************************************/
CBaseGame::~CBaseGame()
{
}   // destructer


/***************************************************************************
*   desc:  Create the game Window
 ****************************************************************************/
void CBaseGame::Create()
{
    // Create the window
    CDevice::Instance().Create();

    // Get local copy of the window handle
    m_pWindow = CDevice::Instance().GetNativeWindow();
    m_pFrameBuffer = CDevice::Instance().GetFrameBuffer();

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
    m_pFrameBuffer->Clear();
    m_pFrameBuffer->Flip();

}   // Init


/***************************************************************************
*   desc:  Poll for game events
****************************************************************************/
void CBaseGame::PollEvents()
{
    // Poll native events
    m_pWindow->PollEvents();

    // Handle events on queue
    CEvent event;
    while( CEventQueue::Instance().PollEvent(event) )
    {
        // let the game handle the event
        // turns true on quit
        if( HandleEvent( event ) )
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
    m_pFrameBuffer->Clear();

    // Do the pre render
    PreRender();

    // Do the post render
    PostRender();

    // Do the back buffer swap
    m_pFrameBuffer->Flip();

}   // Render


/***************************************************************************
*   desc:  Display error massage
****************************************************************************/
void CBaseGame::DisplayErrorMsg( const std::string & title, const std::string & msg )
{
    NGenFunc::PostDebugMsg( "Error: " + title + " - " + msg );

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
