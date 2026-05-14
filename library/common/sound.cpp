
/************************************************************************
*    FILE NAME:       sound.cpp
*
*    DESCRIPTION:     Class to hold the sound pointer and type
************************************************************************/

// Physical component dependency
#include <common/sound.h>

// SDL lib dependencies
#include <SDL_mixer.h>

// Standard lib dependencies

// Boost lib dependencies
#include <boost/format.hpp>

// Game lib dependencies
#include <utilities/exceptionhandling.h>
#include <utilities/genfunc.h>


/************************************************************************
*    desc:  Constructer
************************************************************************/
CSound::CSound( ESoundType type )
    : m_pVoid(nullptr),
      m_type(type),
      m_channel(-1)
{
}   // constructor


/************************************************************************
*    desc:  destructer                                                             
************************************************************************/
CSound::~CSound()
{
}	// destructer


/************************************************************************
*    desc:  Free the sound                                                             
************************************************************************/
void CSound::Free()
{
    if( m_type == EST_LOADED )
        Mix_FreeChunk( (Mix_Chunk *)m_pVoid );

    else if( m_type == EST_STREAM )
        Mix_FreeMusic( (Mix_Music *)m_pVoid );

}	// Free


/************************************************************************
*    desc:  Load the sound                                                             
************************************************************************/
void CSound::Load( const std::string & file )
{
    if( m_type == EST_LOADED )
        m_pVoid = Mix_LoadWAV( file.c_str() );

    else if( m_type == EST_STREAM )
        m_pVoid = Mix_LoadMUS( file.c_str() );

    if( m_pVoid == nullptr )
        throw NExcept::CCriticalException("Sound load Error!",
            boost::str( boost::format("Error loading sound (%s).\n\n%s\nLine: %s")
                % file % __FUNCTION__ % __LINE__ ));

}   // Load


/************************************************************************
*    desc:  Play the sound
*    NOTE: Loop and channel default to -1
************************************************************************/
void CSound::Play( int loopCount, int channel )
{
    if( m_type == EST_LOADED )
        m_channel = Mix_PlayChannel( channel, (Mix_Chunk *)m_pVoid, loopCount );

    else if( m_type == EST_STREAM )
        Mix_PlayMusic( (Mix_Music *)m_pVoid, loopCount );

}   // Play


/************************************************************************
*    desc:  Stop the sound
************************************************************************/
void CSound::Stop()
{
    if( m_type == EST_LOADED )
    {
        if( (m_channel > -1) && Mix_Playing( m_channel ) )
            Mix_HaltChannel( m_channel );
    }
    else if( m_type == EST_STREAM )
    {
        if( Mix_PlayingMusic() )
            Mix_HaltMusic();
    }

}   // Stop


/************************************************************************
*    desc:  Pause the sound
************************************************************************/
void CSound::Pause()
{
    if( m_type == EST_LOADED )
    {
        if( (m_channel > -1) && Mix_Playing( m_channel ) )
            Mix_Pause( m_channel );
    }
    else if( m_type == EST_STREAM )
    {
        if( Mix_PlayingMusic() )
            Mix_PauseMusic();
    }

}   // Pause


/************************************************************************
*    desc:  Resume the sound
************************************************************************/
void CSound::Resume()
{
    if( m_type == EST_LOADED )
    {
        if( (m_channel > -1) && Mix_Paused( m_channel ) )
            Mix_Resume( m_channel );
    }
    else if( m_type == EST_STREAM )
    {
        if( Mix_PausedMusic() )
            Mix_ResumeMusic();
    }

}   // Pause


/************************************************************************
*    desc:  Find an open channel and set the class member
************************************************************************/
void CSound::SetOpenChannel()
{
    int i;
    for( i = 0; i < MIX_CHANNELS; ++i )
    {
        if ( !Mix_Playing(i) )
            break;
    }

    if( i == MIX_CHANNELS )
    {
        NGenFunc::PostDebugMsg( "No free channels available" );
    }
    else
    {
        m_channel = i;
    }

}   // SetOpenChannel