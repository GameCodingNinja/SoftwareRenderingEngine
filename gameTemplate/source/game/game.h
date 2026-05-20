
/************************************************************************
*    FILE NAME:       game.h
*
*    DESCRIPTION:     CGame class
************************************************************************/

#ifndef __game_h__
#define __game_h__

// Game lib dependencies
#include <system/event.h>

// Standard lib dependencies
#include <string>
#include <memory>

// Forward declaration(s)
class IWindow;
class IFrameBuffer;
class iGameState;

class CGame
{
public:

    // Constructor
    CGame();

    // Destructor
    ~CGame();

    // Create the game Window
    void Create();

    // Main game loop
    bool GameLoop();

    // Display error massage
    void DisplayErrorMsg( const std::string & title, const std::string & msg );

    // Is the game running?
    bool IsGameRunning() const;

private:

    // Game start init
    void Init();

    // Handle events
    bool HandleEvent( const CEvent & rEvent );

    // Handle the state change
    void DoStateChange();

    // Check for collision and react to it
    void ReactToCollision();

    // Update animations, Move sprites, Check for collision
    void Update();

    // Transform game objects
    void Transform();

    // Start the game
    void StartGame();

    // Stop the game
    void StopGame();

    // Do the rendering
    void Render();

    // Poll for game events
    void PollEvents();

private:

    // The window we'll be rendering to
    IWindow * m_pWindow;

    // The frame buffer
    IFrameBuffer * m_pFrameBuffer;

    // flag to indicate the game is running
    bool m_gameRunning;

    // scoped pointer Game State
    std::unique_ptr<iGameState> spGameState;

};

#endif  // __game_h__
