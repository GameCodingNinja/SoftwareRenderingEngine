
/************************************************************************
*    FILE NAME:       basegame.h
*
*    DESCRIPTION:     base game class
************************************************************************/

#ifndef __base_game_h__
#define __base_game_h__

// SDL lib dependencies
#include <SDL.h>

// Standard lib dependencies
#include <string>

// Boost lib dependencies

// Game lib dependencies
#include <common/defs.h>

// Game dependencies

// Forward declaration(s)

class CBaseGame
{
public:

    // Constructor
    CBaseGame();

    // Destructor
    virtual ~CBaseGame();

    // Create the game Window
    void Create();

    // Main game loop
    bool GameLoop();

    // Display error massage
    void DisplayErrorMsg( const std::string & title, const std::string & msg );

    // Is the game running?
    bool IsGameRunning() const;

protected:

    // Game start init
    virtual void Init();

    // Handle events
    virtual bool HandleEvent( const SDL_Event & event ) = 0;

    // Handle the state change
    virtual void DoStateChange() = 0;

    // Check for collision and react to it
    virtual void ReactToCollision() = 0;

    // Update animations, Move sprites, Check for collision
    virtual void Update() = 0;

    // Transform game objects
    virtual void Transform() = 0;

    // Render of game content
    virtual void PreRender() = 0;

    // Post Render of game content
    virtual void PostRender() = 0;

    // Start the game
    void StartGame();

    // Stop the game
    void StopGame();

private:

    // Do the rendering
    void Render();

    // Poll for game events
    void PollEvents();

protected:

    // The window we'll be rendering to
    SDL_Window * m_pWindow;

    // OpenGL context
    //SDL_GLContext m_context;

    // flag to indicate the game is running
    bool m_gameRunning;

    // Clear buffer mask
    uint m_clearBufferMask;

};

#endif  // __base_game_h__


