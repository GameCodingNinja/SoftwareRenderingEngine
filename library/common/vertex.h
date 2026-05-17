/************************************************************************
*    FILE NAME:       vertex.h
*
*    DESCRIPTION:     Vertex class
************************************************************************/  

#ifndef __vertex_h__
#define __vertex_h__

// Game lib dependencies
#include <common/point.h>
#include <common/uv.h>

class CVertex
{
public:

    // Verts
    CPoint<float> vert;

    // uv
    CUV uv;
};

#endif  // __vertex_h__
