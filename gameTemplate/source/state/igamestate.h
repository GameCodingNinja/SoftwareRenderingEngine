
/************************************************************************
*    FILE NAME:       igamestate.h
*
*    DESCRIPTION:     gamestate interface Class
************************************************************************/

#ifndef __i_game_state_h__
#define __i_game_state_h__

// STL lib dependencies
#include <vector>
#include <string>

// Standard lib dependencies
#include <tuple>

// Game lib dependencies
#include <system/event.h>

enum EGameState
{
    EGS_NULL,
    EGS_STARTUP,
    EGS_TITLE_SCREEN,
    EGS_GAME_LOAD,
    EGS_RUN
};


class CStateMessage
{
public:

    CStateMessage() : m_nextState(EGS_NULL), m_lastState(EGS_NULL)
    {}

    std::vector<std::string> m_groupLoad;
    std::vector<std::string> m_groupUnload;

    std::vector<std::string> m_createActor;
    std::vector<std::string> m_createActorVec;

    std::vector< std::tuple<std::string, int> > m_createMegaTexture;

    std::vector<std::string> m_createInstanceMesh;

    std::string m_stage2DLoad;

    std::string m_hudLoad;

    std::string m_actorDataLoad;

    std::string m_generatorLoadPath;

    EGameState m_nextState;
    EGameState m_lastState;

    void Clear()
    {
        m_groupLoad.clear();
        m_groupUnload.clear();
        m_createActor.clear();
        m_createActorVec.clear();
        m_createMegaTexture.clear();
        m_createInstanceMesh.clear();
        m_stage2DLoad.clear();
        m_hudLoad.clear();
        m_actorDataLoad.clear();
        m_generatorLoadPath.clear();
        
        m_nextState = EGS_NULL;
        m_lastState = EGS_NULL;
    }
};


class iGameState
{
public:

    iGameState(const iGameState&) = delete;
    iGameState& operator=(const iGameState&) = delete;

    // Constructor
    iGameState() : m_gameState(EGS_NULL), m_nextState(EGS_NULL){};

    // Constructor
    iGameState( const CStateMessage & stateMsg ){};

    // Destructor
    virtual ~iGameState(){};

    // Handle events
    virtual void HandleEvent( const CEvent & rEvent ){};

    // Act upon what the user is doing
    virtual void GetUserInput( bool hasFocus ){};

    // Check for collision and react to it
    virtual void ReactToCollision(){};

    // Update objects that require them
    virtual void Update(){};

    // Transform the game objects
    virtual void Transform(){};

    // 2D/3D Render of game content
    virtual void render(){};

    // Is the state done
    virtual bool DoStateChange()
    { return false; };

    // Get the currect game state
    EGameState GetState()
    { return m_gameState; }

    // Get the next game state
    virtual EGameState GetNextState()
    { return m_nextState; }

    // Get the message
    const CStateMessage & GetStateMessage()
    { return m_stateMessage; }

protected:

    // Message to send to next state
    CStateMessage m_stateMessage;

    // This objects current game state value
    EGameState m_gameState;

    // This objects next game state value
    EGameState m_nextState;

};

#endif  // __i_game_state_h__


