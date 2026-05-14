
/************************************************************************
*    FILE NAME:       runstate.h
*
*    DESCRIPTION:     CRunState Class State
************************************************************************/

#ifndef __run_up_state_h__
#define __run_up_state_h__

// Physical component dependency
#include "commonstate.h"

// Forward declaration(s)

class CRunState : public CCommonState
{
public:

    // Constructor
    CRunState();

    // Destructor
    virtual ~CRunState(){};

    // Check for collision and react to it
    void ReactToCollision();

    // Update objects that require them
    void Update();

    // Transform the game objects
    void Transform();

    // Do the rendering
    void PreRender();

    // Is the state done
    bool DoStateChange();

    // Handle events
    virtual void HandleEvent( const SDL_Event & rEvent );

private:

    // Flag to indicate state change
    bool m_changeState;
};

#endif  // __run_up_state_h__


