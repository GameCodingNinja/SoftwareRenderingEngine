/************************************************************************
*    FILE NAME:       sound.h
*
*    DESCRIPTION:     Class to hold the sound pointer and type
************************************************************************/  

#ifndef __sound_h__
#define __sound_h__

// Standard lib dependencies
#include <string>

class CSound
{
public:

    enum ESoundType
    {
        EST_NULL=0,
        EST_LOADED,
        EST_STREAM,
        EST_MAX,
    };

    CSound( ESoundType type );
    ~CSound();

    // Load the sound
    void Load( const std::string & file );

    // Play the sound
    void Play( int loopCount = 0, int channel = -1 );

    // Stop the sound
    void Stop();

    // Pause the sound
    void Pause();

    // Resume the sound
    void Resume();

    // Find an open channel and set the class member
    void SetOpenChannel();

    // Free the sound
    void Free();

private:
    
    // Voided pointer to hold the different sound type
    void * m_pVoid;

    // Sound type - loaded or stream
    ESoundType m_type;

    // Channel the sound is currently running on
    int m_channel;
};

#endif  // __sound_h__

