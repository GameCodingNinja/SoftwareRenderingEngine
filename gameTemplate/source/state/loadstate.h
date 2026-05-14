
/************************************************************************
*    FILE NAME:       loadstate.h
*
*    DESCRIPTION:     CRunState Class State
************************************************************************/

#ifndef __load_state_h__
#define __load_state_h__

// Physical component dependency
#include "igamestate.h"

// Game lib dependencies
#include <utilities/thread.h>

class CLoadState : public iGameState
{
public:

    // Constructor
    explicit CLoadState( const CStateMessage & stateMsg );

    // Destructor
    virtual ~CLoadState(){};

    // Is the state done
    bool DoStateChange();

    // Get the next state to load
    virtual EGameState GetNextState();

private:

    // Load during the startup screen
    int Animate();

    // Load during the loading screen
    void Load();

private:

    // Load thread
    thread::CThread<CLoadState> m_loadAnimThread;

    // flag to exit the thread
    bool m_exitThread;

};

#endif  // __game_load_state_h__


