/************************************************************************
*    FILE NAME:       highresolutiontimer.cpp
*
*    DESCRIPTION:     high resolution timer class
************************************************************************/

// Physical component dependency
#include <utilities/highresolutiontimer.h>

#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#else
    #include <time.h>
#endif

/************************************************************************
*    desc:  Get the current high-resolution time in nanoseconds
************************************************************************/
static uint64_t GetPerformanceCounter()
{
    #ifdef _WIN32
        LARGE_INTEGER counter;
        QueryPerformanceCounter( &counter );
        return static_cast<uint64_t>( counter.QuadPart );
    #else
        struct timespec ts;
        clock_gettime( CLOCK_MONOTONIC, &ts );
        return static_cast<uint64_t>( ts.tv_sec ) * 1000000000ULL + static_cast<uint64_t>( ts.tv_nsec );
    #endif
}


/************************************************************************
*    desc:  Get the performance counter frequency (ticks per second)
************************************************************************/
static uint64_t GetPerformanceFrequency()
{
    #ifdef _WIN32
        LARGE_INTEGER freq;
        QueryPerformanceFrequency( &freq );
        return static_cast<uint64_t>( freq.QuadPart );
    #else
        // clock_gettime with CLOCK_MONOTONIC uses nanoseconds
        return 1000000000ULL;
    #endif
}


/************************************************************************
*    desc:  Constructer                                                             
************************************************************************/
CHighResTimer::CHighResTimer()
    : m_inverseTimerFrequency(0.0),
      m_lastTime(0),
      m_elapsedTime(0.0f),
      m_fps(0.0f)
{
    // inverse it so that we can do a simple multiplication instead of division
    m_inverseTimerFrequency = 1000.0 / (double)GetPerformanceFrequency();

    // Init the lastTime variable for the first runthrough
    m_lastTime = GetPerformanceCounter();

}


/************************************************************************
*    desc:  Destructer                                                             
************************************************************************/
CHighResTimer::~CHighResTimer()
{
}


/***************************************************************************
*    desc:  Calc the elapsed time
****************************************************************************/
void CHighResTimer::CalcElapsedTime()
{
    // Get the current performance time
    uint64_t time = GetPerformanceCounter();

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
        
}


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
    uint64_t time = GetPerformanceCounter();

    return (double)(time * m_inverseTimerFrequency);

}
