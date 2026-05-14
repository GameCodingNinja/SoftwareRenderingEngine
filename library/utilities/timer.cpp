
/************************************************************************
*    FILE NAME:       timer.cpp
*
*    DESCRIPTION:     Timer Class
************************************************************************/

// Physical component dependency
#include <utilities/timer.h>

// Game lib dependencies
#include <utilities/highresolutiontimer.h>


/************************************************************************
*    desc:  Constructer
************************************************************************/
CTimer::CTimer(double interval, bool startExpired)
    : timeInterval(interval),
      disabled(false)
{
    if( startExpired )
        SetExpired();
    else
        Reset();

}   // constructor

CTimer::CTimer()
    : expiredTime(CHighResTimer::Instance().GetTime()),
      disabled(false)
{
}   // constructor


/************************************************************************
*    desc:  destructer                                                             
************************************************************************/
CTimer::~CTimer()
{
}	// destructer


/***************************************************************************
*   desc:  Reset the timer to start over
****************************************************************************/
void CTimer::Reset()
{
    expiredTime = timeInterval + CHighResTimer::Instance().GetTime();
    disabled = false;

}   // Reset


/***************************************************************************
*   desc:  Set the time to have expired
****************************************************************************/
void CTimer::SetExpired()
{
    expiredTime = CHighResTimer::Instance().GetTime();

}   // Reset


/***************************************************************************
*   desc:  Set the timer interval
****************************************************************************/
void CTimer::Set( double interval )
{
    timeInterval = interval;
    Reset();

}   // Set


/***************************************************************************
*   desc:  Has the timer expired
*
*   param:  resetOnExpire - Reset the timer if it has expired
*
****************************************************************************/
bool CTimer::Expired( bool resetOnExpire )
{
    // Has the timer been disabled
    if( disabled )
        return false;

    bool result(false);

    if( CHighResTimer::Instance().GetTime() > expiredTime )
    {
        result = true;

        if( resetOnExpire )
            Reset();
    }

    return result;

}   // Expired


/***************************************************************************
*   desc:  Disable this timer
****************************************************************************/
void CTimer::Disable(bool value)
{
    disabled = value;

}   // Disable

