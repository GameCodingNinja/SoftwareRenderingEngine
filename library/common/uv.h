/************************************************************************
*    FILE NAME:       uv.h
*
*    DESCRIPTION:     uv class
************************************************************************/  

#ifndef __uv_h__
#define __uv_h__

template <typename type>
class CUV
{
public:

    type u, v;

    CUV():u(0),v(0)
    {
    }

    CUV( type _u, type _v )
    {
            u = _u;
            v = _v;
    }

    // Needs to be done like this to avoid recursion
    template <typename U>
    CUV( const CUV<U> &obj ) : u(obj.u), v(obj.v)
    {
    }
};

#endif  // __uv_h__
