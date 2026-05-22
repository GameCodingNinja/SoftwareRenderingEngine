
/************************************************************************
*    FILE NAME:       vertexbuffermanager.h
*
*    DESCRIPTION:     vertex buffer manager class singleton
************************************************************************/  

#ifndef __vertex_buffer_manager_h__
#define __vertex_buffer_manager_h__

// Standard lib dependencies
#include <string>
#include <map>

// Game lib dependencies
#include <common/rect.h>
#include <common/size.h>
#include <common/point.h>
#include <common/defs.h>

// Forward declaration(s)
class CScaledFrame;
class CQuad2D;
template <typename type> class CUV;

class CVertBufMgr
{
public:

    // Get the instance of the singleton class
    static CVertBufMgr & Instance()
    {
        static CVertBufMgr VertBufMgr;
        return VertBufMgr;
    }

    // Create a quad VBO buffers
    float * createQuadVBO( const std::string & group, const std::string & name, const CRect<float> & uv );

    // Create a IBO buffer
    uint * createIBO( const std::string & group, const std::string & name, uint indexData[], int sizeInBytes );

    // Creates and manages the dynamic font indicies IBO
    uint * CreateDynamicFontIBO( const std::string & group, const std::string & name, unsigned short * pIndexData, int maxIndicies );

    // Create a scaled frame
    float * createScaledFrame( const std::string & group,
                              const std::string & name,
                              const CScaledFrame & scaledFrame,
                              const CSize<int> & textSize,
                              const CSize<int> & size );

    void createQuad( const CPoint<float> & vert,
                     const CSize<float> & vSize,
                     const CUV<float> & uv,
                     const CSize<float> & uvSize,
                     const CSize<float> & textSize,
                     const CSize<float> & size,
                     CQuad2D & quadBuf );

    // Delete buffer group
    void deleteBufferGroup( const std::string & group );

private:

    // Constructor
    CVertBufMgr();

    // Destructor
    ~CVertBufMgr();

private:

    // Map containing a group of VBO handles
    std::map< const std::string, std::map< const std::string, float * > > m_vertexBufMapMap;

    // Map containing a group of IBO handles
    std::map< const std::string, std::map< const std::string, uint * > > m_indexBufMapMap;

    // Current dynamic font IBO indices size
    int currentMaxFontIndices;

};

#endif
