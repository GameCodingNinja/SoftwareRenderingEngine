
/************************************************************************
*    FILE NAME:       startupstate.h
*
*    DESCRIPTION:     CStartUp Class State
************************************************************************/

#ifndef __start_up_state_h__
#define __start_up_state_h__

// Standard lib dependencies
#include "igamestate.h"

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

    // Load game assets
    void Load();

};

#endif  // __start_up_state_h__
