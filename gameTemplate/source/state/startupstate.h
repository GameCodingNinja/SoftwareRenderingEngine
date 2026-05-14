
/************************************************************************
*    FILE NAME:       startupstate.h
*
*    DESCRIPTION:     CStartUp Class State
************************************************************************/

#ifndef __start_up_state_h__
#define __start_up_state_h__

// Standard lib dependencies
#include "igamestate.h"

// Game lib dependencies
#include <utilities/thread.h>

// Forward declaration(s)
struct SDL_Surface;

class CStartUpState : public iGameState
{
public:

    // Constructor
    CStartUpState();

    // Destructor
    virtual ~CStartUpState(){};

    // Is the state done
    bool DoStateChange();

private:

    // Load during the startup screen
    int Animate();

    // Load game assets
    void Load();

    // Fade to color
    void FadeTo( float time, float current, float final, SDL_Surface * pSource, SDL_Surface * pTarget, SDL_Rect & rect );

private:

    // Load thread
    thread::CThread<CStartUpState> m_loadAnimThread;

};

#endif  // __start_up_state_h__


