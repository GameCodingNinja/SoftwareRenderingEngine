/************************************************************************
*    FILE NAME:       highresolutiontimer.h
*
*    DESCRIPTION:     high resolution timer class
************************************************************************/

// Physical component dependency
#include <utilities/highresolutiontimer.h>

// SDL lib dependencies
#include <SDL.h>

/************************************************************************
*    desc:  Constructer                                                             
************************************************************************/
CHighResTimer::CHighResTimer()
    : m_inverseTimerFrequency(0.0),
      m_lastTime(Uint64(0.0)),
      m_elapsedTime(0.0f),
      m_fps(0.0f)
{
    // inverse it so that we can do a simple multiplication instead of division
    m_inverseTimerFrequency = 1000.0 / (double)SDL_GetPerformanceFrequency();

    // Init the lastTime variable for the first runthrough
    m_lastTime = SDL_GetPerformanceCounter();

}   // Constructer


/************************************************************************
*    desc:  Destructer                                                             
************************************************************************/
CHighResTimer::~CHighResTimer()
{
}   // Destructer


/***************************************************************************
*    desc:  Calc the elapsed time
****************************************************************************/
void CHighResTimer::CalcElapsedTime()
{
    // Get the current performance time
    Uint64 time = SDL_GetPerformanceCounter();

    // Set the elapsed time
    m_elapsedTime = (float)((time - m_lastTime) * m_inverseTimerFrequency);

    // Set the fps
    m_fps = 1000.0f / m_elapsedTime;

    // Catch any hickups - cap to about 10 fps
    if( m_elapsedTime > 100.0f )
    {
        m_elapsedTime = 100.0f;
    }

    // Reset the last time
    m_lastTime = time;
        
}  // GetPerformanceTime


/***************************************************************************
*    desc:  Get the elapsed time
****************************************************************************/
float CHighResTimer::GetElapsedTime()
{
    return m_elapsedTime;
}


/***************************************************************************
*    desc:  get the elapsed time
****************************************************************************/
float CHighResTimer::GetFPS()
{
    return m_fps;
}


/***************************************************************************
*    desc:  get the raw time
****************************************************************************/
double CHighResTimer::GetTime()
{
    // Get the current performance time
    Uint64 time = SDL_GetPerformanceCounter();

    return (double)(time * m_inverseTimerFrequency);

}   // GetTime


