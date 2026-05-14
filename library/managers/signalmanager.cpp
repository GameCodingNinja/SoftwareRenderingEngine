
/************************************************************************
*    FILE NAME:       signalmanager.cpp
*
*    DESCRIPTION:     Class for handling messaging
************************************************************************/

// Physical component dependency
#include <managers/signalmanager.h>

// SDL/OpenGL lib dependencies
#include <SDL.h>

// Standard lib dependencies

// Game lib dependencies
#include <utilities/exceptionhandling.h>
#include <utilities/genfunc.h>
#include <common/defs.h>
#include <gui/uicontrol.h>

/************************************************************************
*    desc:  Constructer
************************************************************************/
CSignalManager::CSignalManager()
{
}   // constructor


/************************************************************************
*    desc:  destructer                                                             
************************************************************************/
CSignalManager::~CSignalManager()
{
}	// destructer


/************************************************************************
*    desc:  Cue event
************************************************************************/
void CSignalManager::CueEvent( int eventType, int eventCode )
{
    m_eventCue.push_back(CEvent( eventType, eventCode ));

}   // CueEvent


/************************************************************************
*    desc:  Dispatch the Cued events
************************************************************************/
void CSignalManager::DispatchCuedEvents()
{
    // Cue up all the events
    for( auto it = m_eventCue.begin(); it != m_eventCue.end(); ++it )
        NGenFunc::DispatchEvent( it->m_eventType, it->m_eventCode );

    // Now that we are done, clear the cue
    m_eventCue.clear();

}   // DispatchCuedEvents


/************************************************************************
*    desc:  Connect to the smart gui control signal
************************************************************************/
void CSignalManager::Connect( const SmartGuiControlSignal & slot )
{
    m_smartGuiControlSignalVec.push_back(slot);

}	// Connect


/************************************************************************
*    desc:  Broadcast smart gui control signal
************************************************************************/
void CSignalManager::Broadcast( CUIControl * pControl )
{
    for( auto & slot : m_smartGuiControlSignalVec )
        slot(pControl);

}	// Broadcast

