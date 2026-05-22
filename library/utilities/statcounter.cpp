
/************************************************************************
*    FILE NAME:       statcounter.cpp
*
*    DESCRIPTION:     3D stat counter class singleton
************************************************************************/

// Physical component dependency
#include <utilities/statcounter.h>

// Game lib dependencies
#include <system/iwindow.h>
#include <utilities/genfunc.h>
#include <utilities/highresolutiontimer.h>

/************************************************************************
*    desc:  Constructer                                                             
************************************************************************/
CStatCounter::CStatCounter()
    : m_cycleCounter(0),
      m_scriptContexCounter(0),
      m_activeContexCounter(0),
      m_statsDisplayTimer(500)
{
    ResetCounters();

}


/************************************************************************
*    desc:  Destructer                                                             
************************************************************************/
CStatCounter::~CStatCounter()
{

}


/************************************************************************
*    desc:  Have we run through one cycle.
*
*	 ret: bool - true if one cycle has been completed
************************************************************************/
void CStatCounter::IncCycle( IWindow * pWindow )
{
    // These counters are incremeented each game loop cycle so they can
    // be placed here in this function because this function is also called
    // each game loop cycle
    m_elapsedFPSCounter += CHighResTimer::Instance().GetFPS();

    ++m_cycleCounter;

    // update the stats every 500 miliseconds
    if( m_statsDisplayTimer.Expired(CTimer::RESTART_ON_EXPIRE) )
    {
        // The call also resets the counters
        pWindow->SetTitle( GetStatString() );

        // Now that the stats are displayed, we can reset out counters.
        ResetCounters();
    }

}


/************************************************************************
*    desc:  Reset the counter                                                             
************************************************************************/
void CStatCounter::ResetCounters()
{
    m_cObjCounter = 0;
    m_vObjCounter = 0;
    m_elapsedFPSCounter = 0.0;
    m_cycleCounter = 0;
    m_activeContexCounter = 0;

}


/************************************************************************
*    desc:  Get the stat string                                                             
************************************************************************/
const std::string & CStatCounter::GetStatString()
{
    m_statStr = NGenFunc::FormatString("fps: %d - scx: %d of %d - vis: %d",// - xObjs: %2% - cObjs: %3% - Shadows: %4% - PlayerPos: %5%x %6%y - Vid Mem Used: %7% megs
                (int)(m_elapsedFPSCounter / (double)m_cycleCounter),
                (m_activeContexCounter / m_cycleCounter),
                m_scriptContexCounter,
                (m_vObjCounter / m_cycleCounter)
                //% (cObjCounter / m_cycleCounter)
                //% (shadowCounter / m_cycleCounter)
                //% (playerPos.x)
                //% (playerPos.y) 
                );

    return m_statStr;

}


/************************************************************************
*    desc:  Inc the display counter                                                             
************************************************************************/
void CStatCounter::incDisplayCounter( size_t i )
{
    m_vObjCounter += i;

}


/************************************************************************
*    desc:  Inc the collision counter                                                             
************************************************************************/
void CStatCounter::IncCollisionCounter( size_t i )
{
    m_cObjCounter += i;

}


/************************************************************************
*    desc:  Inc the script contex counter                                                             
************************************************************************/
void CStatCounter::IncScriptContexCounter()
{
    ++m_scriptContexCounter;

}


/************************************************************************
*    desc:  Inc the active script contex counter                                                             
************************************************************************/
void CStatCounter::IncActiveScriptContexCounter()
{
    ++m_activeContexCounter;

}
