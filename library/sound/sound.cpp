
/************************************************************************
*    FILE NAME:       sound.cpp
*
*    DESCRIPTION:     Class to hold the sound data and playback state
************************************************************************/

// Physical component dependency
#include <sound/sound.h>

// Game lib dependencies
#include <sound/mixengine.h>
#include <sound/codecfactory.h>
#include <sound/wavcodec.h>
#include <utilities/exceptionhandling.h>
#include <utilities/genfunc.h>
#include <utilities/xmlParser.h>

// Standard lib dependencies
#include <cstring>


/************************************************************************
*    DESC:  Constructor
************************************************************************/
CSound::CSound( ESoundType type, NDefs::EMixBus bus ) :
    m_type( type ),
    m_bus( bus ),
    m_channel( -1 ),
    m_volume( MAX_SOUND_VOLUME )
{
}

CSound::CSound() :
    m_type( EST_NULL ),
    m_bus( NDefs::SFX ),
    m_channel( -1 ),
    m_volume( MAX_SOUND_VOLUME )
{
}


/************************************************************************
*    DESC:  Copy Constructor — shares the PCM data via shared_ptr
************************************************************************/
CSound::CSound( const CSound & sound ) :
    m_type( sound.m_type ),
    m_bus( sound.m_bus ),
    m_spWavData( sound.m_spWavData ),
    m_channel( sound.m_channel ),
    m_volume( sound.m_volume )
{
}


/************************************************************************
*    DESC:  Destructor
************************************************************************/
CSound::~CSound()
{
    // Don't free the sound here because copies share the data.
    // free() is called explicitly by CSoundMgr::freeGroup().
}


/************************************************************************
*    DESC:  Free the sound data and stop playback
************************************************************************/
void CSound::free()
{
    stop();
    m_spWavData.reset();
}


/************************************************************************
*    DESC:  Load the sound from an XML node
************************************************************************/
void CSound::loadFromNode( const XMLNode & node )
{
    const char * file = node.getAttribute( "file" );

    // Load via the codec factory (auto-detects WAV, OGG, MP3, FLAC)
    m_spWavData = std::make_shared<SWavData>( NCodecFactory::load( file ) );

    if( !m_spWavData )
        throw NExcept::CCriticalException( "Sound Load Error!",
            NGenFunc::FormatString( "Error loading sound (%s).\n\n%s\nLine: %d",
                file, __FUNCTION__, __LINE__ ) );

    // Resample to match the device sample rate if they differ
    NAudioResample::resample( *m_spWavData, CMixEngine::Instance().getSpec().sampleRate );

    // Set the volume if defined
    if( node.isAttributeSet( "volume" ) )
        setVolume( std::atoi( node.getAttribute( "volume" ) ) );

    // Set the bus if defined (overrides the constructor default)
    if( node.isAttributeSet( "bus" ) )
        m_bus = NDefs::StringToBus( node.getAttribute( "bus" ) );
}


/************************************************************************
*    DESC:  Play the sound
************************************************************************/
void CSound::play( int loopCount )
{
    if( m_spWavData )
    {
        const float vol = static_cast<float>( m_volume ) / MAX_SOUND_VOLUME;
        m_channel = CMixEngine::Instance().playSound( *m_spWavData, m_bus, loopCount, vol );

        if( m_channel < 0 )
        {
            NGenFunc::PostDebugMsg( NGenFunc::FormatString(
                "Sound play error: no channel available.\n\n%s\nLine: %d",
                __FUNCTION__, __LINE__ ) );
        }
    }
}


/************************************************************************
*    DESC:  Stop the sound
************************************************************************/
void CSound::stop()
{
    if( m_channel >= 0 )
    {
        CMixEngine::Instance().stopChannel( m_channel );
        m_channel = -1;
    }
}


/************************************************************************
*    DESC:  Pause the sound
************************************************************************/
void CSound::pause()
{
    if( m_channel >= 0 )
        CMixEngine::Instance().pauseChannel( m_channel );
}


/************************************************************************
*    DESC:  Resume the sound
************************************************************************/
void CSound::resume()
{
    if( m_channel >= 0 )
        CMixEngine::Instance().resumeChannel( m_channel );
}


/************************************************************************
*    DESC:  Set/Get the volume (0–128)
************************************************************************/
void CSound::setVolume( int volume )
{
    m_volume = static_cast<int16_t>( std::clamp( volume, 0, static_cast<int>(MAX_SOUND_VOLUME) ) );

    // Update the live channel volume if playing
    if( m_channel >= 0 )
    {
        const float vol = static_cast<float>( m_volume ) / MAX_SOUND_VOLUME;
        CMixEngine::Instance().setChannelVolume( m_channel, vol );
    }
}

int CSound::getVolume() const
{
    return m_volume;
}


/************************************************************************
*    DESC:  Is the sound playing?
************************************************************************/
bool CSound::isPlaying() const
{
    if( m_channel >= 0 )
        return CMixEngine::Instance().isChannelPlaying( m_channel );

    return false;
}


/************************************************************************
*    DESC:  Is the sound paused?
************************************************************************/
bool CSound::isPaused() const
{
    if( m_channel >= 0 )
        return CMixEngine::Instance().isChannelPaused( m_channel );

    return false;
}


/************************************************************************
*    DESC:  Equality operator — compare by shared data pointer
************************************************************************/
bool CSound::operator == ( const CSound & sound ) const
{
    return ( m_spWavData == sound.m_spWavData );
}

bool CSound::operator != ( const CSound & sound ) const
{
    return ( m_spWavData != sound.m_spWavData );
}
