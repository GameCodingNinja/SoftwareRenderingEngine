
/************************************************************************
*    FILE NAME:       signalmanager.h
*
*    DESCRIPTION:     Class for handling messaging
************************************************************************/

#ifndef __message_manager_h__
#define __message_manager_h__

// Standard lib dependencies
#include <string>
#include <vector>
#include <map>

// Standard lib dependencies
#include <functional>

// Forward declaration(s)
class CUIControl;

class CSignalManager
{
public:

    // Define the signal type
    using SmartGuiControlSignal = std::function<void(CUIControl*)>;

    // Get the instance of the singleton class
    static CSignalManager & Instance()
    {
        static CSignalManager signalMgr;
        return signalMgr;
    }

    // Cue event
    void CueEvent( int eventType, int eventCode = 0 );

    // Dispatch the Cued events
    void DispatchCuedEvents();

    // Connect to the smart gui signal
    void Connect( const SmartGuiControlSignal & slot );

    // Broadcast smart gui control signal
    void Broadcast( CUIControl * pControl );

private:

    // Constructor
    CSignalManager();

    // Destructor
    ~CSignalManager();

private:

    class CEvent
    {
    public:
        CEvent(int type, int code) : 
            m_eventType(type), m_eventCode(code)
        {}
        int m_eventType;
        int m_eventCode;
    };

    // User even cue
    std::vector<CEvent> m_eventCue;

    // Signal callbacks
    std::vector<SmartGuiControlSignal> m_smartGuiControlSignalVec;

};

#endif  // __message_manager_h__


