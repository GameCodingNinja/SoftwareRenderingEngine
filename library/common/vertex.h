/************************************************************************
*    FILE NAME:       vertex.h
*
*    DESCRIPTION:     Vertex classes for 2D and 3D rendering
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
    CUV<float> uv;

    // Normal (Gouraud: stores lit color/W, Phong: stores normal/W)
    CPoint<float> norm;
};

#endif
