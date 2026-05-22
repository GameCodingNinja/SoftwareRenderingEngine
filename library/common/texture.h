
/************************************************************************
*    FILE NAME:       texture.h
*
*    DESCRIPTION:     Class for holding texture data.
*                     Non-owning wrapper around stb image memory.
*                     Pixel memory is freed by CTextureMgr::deleteTextureGroup().
************************************************************************/

#ifndef __texture_h__
#define __texture_h__

// Game lib dependencies
#include <common/size.h>
#include <common/defs.h>

class CTexture
{
public:

    // Texture size
    CSize<int> m_size;

    // Pointer to texture data (non-owning, freed by CTextureMgr)
    uchar * m_pData = nullptr;

    const CSize<int> & getSize() const { return m_size; }
};

#endif
