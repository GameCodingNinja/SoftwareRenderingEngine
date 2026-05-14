
/************************************************************************
*    FILE NAME:       scaledframe.h
*
*    DESCRIPTION:     Class for holding scaled frame data
************************************************************************/

#ifndef __scaled_frame_h__
#define __scaled_frame_h__

// Game lib dependencies
#include <common/size.h>

class CScaledFrame
{
public:

    // Constructor/Destructor
    CScaledFrame()
            : m_centerQuad(true)
    {}
    ~CScaledFrame(){};

    CSize<float> m_frame;

    bool m_centerQuad;
};

#endif  // __scaled_frame_h__


