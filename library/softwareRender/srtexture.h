
/************************************************************************
*    FILE NAME:       srtexture.h
*
*    DESCRIPTION:     Software rendering texture class
************************************************************************/

#ifndef __sr_texture_h__
#define __sr_texture_h__

// Game lib dependencies
#include <common/size.h>
#include <common/defs.h>

class CSRTexture
{
public:

    // Constructor
    CSRTexture();

    // Constructor
    CSRTexture( uint w, uint h, uchar * pData );

	// Destructor
	~CSRTexture();

    // Get the width
    int GetW()
    { return m_size.w; }

    // Get the height
    int GetH()
    { return m_size.h; }

    // Set width
    void SetWidth( int value )
    { m_size.w = value; }

    // Set height
    void SetHeight( int value )
    { m_size.h = value; }

    // get the data
    uchar * GetData()
    { return m_pData; }

    // Width and height of texture
    CSize<int> m_size;

    // Pointer to texture data
    uchar * m_pData;

};

#endif  // __sr_texture_h__


