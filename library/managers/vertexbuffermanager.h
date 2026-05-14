
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
class CShaderData;
class CScaledFrame;
class CQuad2D;
class CUV;

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
    uint CreateQuadVBO( const std::string & group, const std::string & name, const CRect<float> & uv );

    // Create a IBO buffer
    uint CreateIBO( const std::string & group, const std::string & name, uint indexData[], int sizeInBytes );

    // Creates and manages the dynamic font indicies IBO
    uint CreateDynamicFontIBO( const std::string & group, const std::string & name, GLushort * pIndexData, int maxIndicies );

    // Create a scaled frame
    GLuint CreateScaledFrame( const std::string & group,
                              const std::string & name,
                              const CScaledFrame & scaledFrame,
                              const CSize<int> & textSize,
                              const CSize<int> & size );

    void CreateQuad( const CPoint<float> & vert,
                     const CSize<float> & vSize,
                     const CUV & uv,
                     const CSize<float> & uvSize,
                     const CSize<float> & textSize,
                     const CSize<float> & size,
                     CQuad2D & quadBuf );

    // Function call used to manage what buffer is currently bound.
    void BindBuffers( GLuint vboID, GLuint iboID );

    // Unbind the buffers and reset the flag
    void UnbindBuffers();

    // Delete buffer group
    void DeleteBufferGroupFor2D( const std::string & group );

private:

    // Constructor
    CVertBufMgr();

    // Destructor
    ~CVertBufMgr();

private:

    // Map containing a group of VBO handles
    std::map< const std::string, std::map< const std::string, uint > > m_vertexBuf2DMapMap;

    // Map containing a group of IBO handles
    std::map< const std::string, std::map< const std::string, uint > > m_indexBuf2DMapMap;

    // Current vbo ID
    GLuint m_currentVBOID;

    // Current IBO ID
    GLuint m_currentIBOID;

    // Current dynamic font IBO indices size
    int currentMaxFontIndices;

};

#endif  // __classtemplate_h__


