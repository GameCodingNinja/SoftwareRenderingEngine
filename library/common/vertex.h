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

class CVertex2d
{
public:

    // Verts
    CPoint<float> vert;

    // uv
    CUV<float> uv;
};

class CVertex3d
{
public:

    // Verts
    CPoint<float> vert;

    // Normal
    CPoint<float> norm;

    // uv
    CUV<float> uv;
};

#endif
