
/************************************************************************
*    FILE NAME:       game.cpp
*
*    DESCRIPTION:     CGame class
************************************************************************/

// Physical component dependency
#include "game.h"

// Standard lib dependencies
#include <stdio.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

// Game lib dependencies
#include <utilities/exceptionhandling.h>
#include <utilities/genfunc.h>
#include <utilities/settings.h>
#include <utilities/highresolutiontimer.h>
#include <utilities/statcounter.h>
#include <utilities/threadpool.h>
#include <system/device.h>
#include <system/iwindow.h>
#include <system/iframebuffer.h>
#include <system/eventqueue.h>
#include <softwareRender/softwareRender.h>
#include <managers/cameramanager.h>
#include <managers/shadermanager.h>

// Game dependencies
#include "../state/startupstate.h"
#include "../state/titlescreenstate.h"
#include "softshader.h"

/************************************************************************
*    desc:  Constructer
************************************************************************/
CGame::CGame()
    : m_pWindow(nullptr),
      m_pFrameBuffer(nullptr),
      m_gameRunning(false)
{
    // Load the settings file
    CSettings::Instance().loadFromXML("data/settings/settings.cfg");

    // Register game-side shaders before any object data is loaded
    CShaderMgr::Instance().registerShader( "default",         NShader::shaderDefault );
    CShaderMgr::Instance().registerShader( "colorMod",        NShader::shaderColorMod );
    CShaderMgr::Instance().registerShader( "alphaTest",       NShader::shaderAlphaTest );
    CShaderMgr::Instance().registerShader( "colorModulation", NShader::shaderColorModulation );
    CShaderMgr::Instance().registerShader( "grayscale",       NShader::shaderGrayscale );
    CShaderMgr::Instance().registerShader( "alphaBlend",      NShader::shaderAlphaBlend );
    CShaderMgr::Instance().registerShader( "colorAlphaBlend", NShader::shaderColorAlphaBlend );

    // Load the camera data early because many objects init the default camera in their constructor
    CCameraMgr::Instance().load( "data/objects/camera.lst" );

    // Init the thread pool
    CThreadPool::Instance().init( 2, -1 );

}   // constructor


/************************************************************************
*    desc:  destructer                                                             
************************************************************************/
CGame::~CGame()
{
}	// destructer


/***************************************************************************
*   desc:  Create the game Window
 ****************************************************************************/
void CGame::Create()
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
void CGame::Init()
{
    // Show the window
    CDevice::Instance().ShowWindow( true );

    // Display a black screen
    m_pFrameBuffer->Clear();
    m_pFrameBuffer->Flip();

    // Create the startup state
    spGameState.reset( new CStartUpState );

    // Let the games begin
    StartGame();

}	// Init


/***************************************************************************
*    decs:  Handle the state change
****************************************************************************/
void CGame::DoStateChange()
{
    if( spGameState->DoStateChange() )
    {
        // Get the game state we are moving to
        const EGameState curState = spGameState->GetState();

        // Get the game state we are moving to
        const EGameState nextState = spGameState->GetNextState();

        if( nextState == EGS_TITLE_SCREEN )
            spGameState.reset( new CTitleScreenState );

        else
            throw NExcept::CCriticalException("Error Invalid game state",
                NGenFunc::FormatString("Next state not valid (cur %d, next %d).\n\n%s\nLine: %d", curState, nextState, __FUNCTION__, __LINE__));
    }

}   // DoStateChange */


/***************************************************************************
*   desc:  Poll for game events
****************************************************************************/
void CGame::PollEvents()
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


/************************************************************************
*    desc:  Handle events
************************************************************************/
bool CGame::HandleEvent( const CEvent & rEvent )
{
    if( rEvent.type == EVENT_QUIT )
        return true;

    // Handle resolution change between frames, not during rendering
    if( rEvent.type == EVENT_WINDOW_RESIZE )
    {
        CDevice::Instance().HandleResolutionChange(
            rEvent.resize.width, rEvent.resize.height );

        // Update the local framebuffer pointer
        m_pFrameBuffer = CDevice::Instance().GetFrameBuffer();
    }

    // Handle events
    spGameState->HandleEvent( rEvent );

    return false;

}	// HandleEvent


/***************************************************************************
*   desc:  Main game loop
****************************************************************************/
bool CGame::GameLoop()
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
        render();

        // Inc the cycle
        CStatCounter::Instance().IncCycle( m_pWindow );
    }

    return m_gameRunning;

}   // GameLoop


/************************************************************************
*    desc:  Check for collision and react to it.
************************************************************************/
void CGame::ReactToCollision()
{

}	// ReactToCollision


/***************************************************************************
*    decs:  Update animations
****************************************************************************/
void CGame::Update()
{
    spGameState->Update();

}	// Update


/***************************************************************************
*    desc:  Transform game objects
****************************************************************************/
void CGame::Transform()
{
    spGameState->Transform();

}	// Transform


/***************************************************************************
*   desc:  Do the rendering
****************************************************************************/
void CGame::render()
{
    // Update the software renderer's pixel pointer for the current back buffer
    CSoftwareRender::Instance().setSurface( m_pFrameBuffer );

    m_pFrameBuffer->Clear();

    // Clear the z-buffer for 3D rendering
    CSoftwareRender::Instance().clearZBuffer();

    // Render game content
    spGameState->render();

    // Do the back buffer swap
    m_pFrameBuffer->Flip();

}   // Render


/***************************************************************************
*   desc:  Display error massage
****************************************************************************/
void CGame::DisplayErrorMsg( const std::string & title, const std::string & msg )
{
    NGenFunc::PostDebugMsg( "Error: " + title + " - " + msg );

    #ifdef _WIN32
    MessageBoxA( nullptr, msg.c_str(), title.c_str(), MB_OK | MB_ICONERROR );
    #endif

}   // DisplayErrorMsg


/***************************************************************************
*   desc:  Start the game
****************************************************************************/
void CGame::StartGame()
{
    m_gameRunning = true;

}   // StartGame


/***************************************************************************
*   desc:  Stop the game
****************************************************************************/
void CGame::StopGame()
{
    m_gameRunning = false;

}   // StopGame


/***************************************************************************
*  desc:  Is the game running?
*
*  ret: bool - true or false if game is running
****************************************************************************/
bool CGame::IsGameRunning() const
{
    return m_gameRunning;

}   // IsGameRunning
