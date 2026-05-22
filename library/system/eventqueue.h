
/************************************************************************
*    FILE NAME:       eventqueue.h
*
*    DESCRIPTION:     Thread-safe event queue singleton, modeled after
*                     the engine's internal event queue.
************************************************************************/

#ifndef __eventqueue_h__
#define __eventqueue_h__

// Standard lib dependencies
#include <queue>
#include <mutex>

// Game lib dependencies
#include <system/event.h>

class CEventQueue
{
public:

    // Get the instance of the singleton class
    static CEventQueue& Instance()
    {
        static CEventQueue eventQueue;
        return eventQueue;
    }

    // Push an event onto the queue (thread-safe)
    void PushEvent(const CEvent& event);

    // Pop the next event from the queue
    // Returns true if an event was available, false if queue is empty
    bool PollEvent(CEvent& event);

    // Clear all pending events
    void Clear();

private:

    // Constructor
    CEventQueue() = default;

    // Destructor
    ~CEventQueue() = default;

private:

    // Event queue
    std::queue<CEvent> m_eventQueue;

    // Mutex for thread safety
    std::mutex m_mutex;
};

#endif
