
/************************************************************************
*    FILE NAME:       playlist.cpp
*
*    DESCRIPTION:     Play list class
************************************************************************/

// Physical component dependency
#include <common/playlist.h>

// Standard lib dependencies
#include <algorithm>

/************************************************************************
*    desc:  Constructer
************************************************************************/
CPlayList::CPlayList( EPlayListType type )
    : m_type(type),
      m_counter(0)
{
}   // constructor


/************************************************************************
*    desc:  destructer                                                             
************************************************************************/
CPlayList::~CPlayList()
{
}   // destructer


/************************************************************************
*    desc:  Set/Get the sound for the playlist                                                             
************************************************************************/
void CPlayList::SetSound( const CSound & sound )
{
    m_soundVec.push_back( sound );

}   // SetSound

CSound & CPlayList::GetSound()
{
    if( (m_type == EST_RANDOM) && (m_counter == 0) )
        std::random_shuffle( m_soundVec.begin(), m_soundVec.end() );

    int counter = m_counter;
    m_counter = (m_counter + 1) % m_soundVec.size();

    return m_soundVec[counter];

}   // GetSound


/************************************************************************
*    desc:  Play the play list
************************************************************************/
void CPlayList::Play( int loopCount, int channel )
{
    if( (m_type == EST_RANDOM) && (m_counter == 0) )
        std::random_shuffle( m_soundVec.begin(), m_soundVec.end() );

    m_soundVec[m_counter].Play( loopCount, channel );

    m_counter = (m_counter + 1) % m_soundVec.size();

}   // Play

