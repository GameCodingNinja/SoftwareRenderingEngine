
/************************************************************************
*    FILE NAME:       timer.h
*
*    DESCRIPTION:     Timer Class
************************************************************************/

#ifndef __timer_h__
#define __timer_h__

// Boost lib dependencies
#include <boost/noncopyable.hpp>

class CTimer : public boost::noncopyable
{
public:

    enum
    { START_EXPIRED = 1,
      RESTART_ON_EXPIRE = 1, };

    // Constructor
    CTimer();
    CTimer( double interval, bool startExpired = false );

    // Destructor
    ~CTimer();

    // Set the time to have expired
    void SetExpired();

    // Has the timer expired
    bool Expired( bool resetOnExpire = false );

    // Reset the timer to start over
    void Reset();

    // Set the timer interval
    void Set(double interval);

    // Set the timer interval
    void Disable(bool value = true);

private:

    // Expired time
    double expiredTime;

    // time interval
    double timeInterval;

    // Disabled flag
    bool disabled;

};

#endif  // __classtemplate_h__
