
/************************************************************************
*    FILE NAME:       srtexture.cpp
*
*    DESCRIPTION:     Software rendering texture class
************************************************************************/

// Physical component dependency
#include <softwareRender/srtexture.h>

// Game lib dependencies
#include <soil/stb_image_aug.h>

/************************************************************************
*    desc:  Constructer
************************************************************************/
CSRTexture::CSRTexture() :
    m_pData(nullptr)
{
}   // constructor

CSRTexture::CSRTexture( uint w, uint h, uchar * pData ) :
    m_size( w, h ),
    m_pData( pData )
{
}   // constructor


/************************************************************************
*    desc:  destructer                                                             
************************************************************************/
CSRTexture::~CSRTexture()
{
    if( m_pData != nullptr )
        stbi_image_free( m_pData );

}	// destructer
