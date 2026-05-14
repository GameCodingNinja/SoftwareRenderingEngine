
/************************************************************************
*    FILE NAME:       texture.h
*
*    DESCRIPTION:     Class for holding texture data
************************************************************************/

#ifndef __texture_h__
#define __texture_h__

// Game lib dependencies
#include <common/size.h>
#include <common/defs.h>

class CTexture
{
public:

    // Constructor/Destructor
    CTexture():m_id(0){};
    ~CTexture(){};

    uint m_id;
    CSize<int> m_size;

    uint GetID() const {return m_id;}
    const CSize<int> & GetSize() const {return m_size;}
};

#endif  // __classtemplate_h__


