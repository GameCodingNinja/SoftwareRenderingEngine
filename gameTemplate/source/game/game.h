
/************************************************************************
*    FILE NAME:       gametemplate.h
*
*    DESCRIPTION:     CGame class
************************************************************************/

#ifndef __game_h__
#define __game_h__

// Physical component dependency
#include <system/basegame.h>

// Standard lib dependencies

// Boost lib dependencies
#include <boost/scoped_ptr.hpp>

// Game lib dependencies

// Game dependencies

// Forward declaration(s)
class CSprite2D;
class CUIControl;
class iGameState;

class CGame : public CBaseGame
{
public:

    // Constructor
    CGame();

    // Destructor
    virtual ~CGame();

protected:

    // Game start init
    virtual void Init();

    // Handle events
    bool HandleEvent( const SDL_Event & rEvent );

    // Handle the state change
    virtual void DoStateChange();

    // Check for collision and react to it
    virtual void ReactToCollision();

    // Update animations, Move sprites, Check for collision
    virtual void Update();

    // Transform game objects
    virtual void Transform();

    // 2D/3D Render of game content
    virtual void PreRender();

    // 2D/3D Render of post game content
    virtual void PostRender();

private:

    // Callback for when a smart gui control is created
    void SmartGuiControlCreate( CUIControl * pUIControl );

protected:

    // scoped pointer Game State
    boost::scoped_ptr<iGameState> spGameState;

};

static int SDLCALL FilterEvents( void * userdata, SDL_Event * pEvent );

#endif  // __game_h__


