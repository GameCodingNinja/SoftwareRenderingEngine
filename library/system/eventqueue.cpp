
/************************************************************************
*    FILE NAME:       eventqueue.cpp
*
*    DESCRIPTION:     Thread-safe event queue singleton
************************************************************************/

// Physical component dependency
#include <system/eventqueue.h>

/************************************************************************
*    desc:  Push an event onto the queue (thread-safe)
************************************************************************/
void CEventQueue::PushEvent(const CEvent& event)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_eventQueue.push(event);

}


/************************************************************************
*    desc:  Pop the next event from the queue
*           Returns true if an event was available
************************************************************************/
bool CEventQueue::PollEvent(CEvent& event)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if( m_eventQueue.empty() )
        return false;

    event = m_eventQueue.front();
    m_eventQueue.pop();

    return true;

}


/************************************************************************
*    desc:  Clear all pending events
************************************************************************/
void CEventQueue::Clear()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_eventQueue = std::queue<CEvent>();

}
