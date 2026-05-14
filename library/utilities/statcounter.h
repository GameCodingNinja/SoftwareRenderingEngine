/************************************************************************
*    FILE NAME:       statcounter.h
*
*    DESCRIPTION:     3D stat counter class singleton
************************************************************************/  

#ifndef __stat_counter_h__
#define __stat_counter_h__

// Standard lib dependencies
#include <string>

// Game lib dependencies
#include <utilities/timer.h>

// Forward declaration(s)
struct SDL_Window;

class CStatCounter
{
public:

    // Get the instance of the singleton class
    static CStatCounter & Instance()
    {
        static CStatCounter statCounter;
        return statCounter;
    }

    // Have we run through one cycle.
    void IncCycle( SDL_Window * m_pWindow );

    // Inc the display counter
    void IncDisplayCounter( size_t i = 1 );

    // Inc the collision counter
    void IncCollisionCounter( size_t i = 1 );

    // Inc the script contex counter
    void IncScriptContexCounter();
    void IncActiveScriptContexCounter();

private:

    // Reset the counter
    void ResetCounters();

    // Get the stat string
    const std::string & GetStatString();

    CStatCounter();
    ~CStatCounter();

private:

    // Counter for static visual objects
    size_t m_vObjCounter;

    // Counter for collision objects
    size_t m_cObjCounter;

    // Elapsed time counter
    long double m_elapsedFPSCounter;

    // cycle counter. This counter is never reset
    size_t m_cycleCounter;

    // Angle Script contex counter
    size_t m_scriptContexCounter;
    size_t m_activeContexCounter;

    // Stat string
    std::string m_statStr;

    // Stats display timer
    CTimer m_statsDisplayTimer;

};

#endif  // __stat_counter_h__
