
/************************************************************************
*    FILE NAME:       titlescreenstate.h
*
*    DESCRIPTION:     CRunState Class State
************************************************************************/

#ifndef __title_screen_state_h__
#define __title_screen_state_h__

// Physical component dependency
#include "commonstate.h"

// Game lib dependencies
#include <2d/sprite2d.h>

class CTitleScreenState : public CCommonState
{
public:

    // Constructor
    CTitleScreenState();

    // Destructor
    virtual ~CTitleScreenState(){};

    // Transform the game objects
    void Transform();

    // Update objects that require them
    void Update();

    // 2D/3D Render of game content
    void PreRender();

    // Is the state done
    bool DoStateChange();

    // Handle events
    virtual void HandleEvent( const SDL_Event & rEvent );

private:

    // title screen background
    CSprite2D m_background;
    CSprite2D m_background2;
    CSprite2D m_background3;

    // Allow for the state change
    bool m_allowStateChange;
};

#endif  // __title_screen_state_h__


