
/************************************************************************
*    FILE NAME:       vertexbuffermanager.cpp
*
*    DESCRIPTION:     vertex buffer manager class singleton
************************************************************************/

// Physical component dependency
#include <managers/vertexbuffermanager.h>

// Standard lib dependencies
#include <cstring>

// Game lib dependencies
#include <common/quad2d.h>
#include <common/scaledframe.h>
#include <common/uv.h>

/************************************************************************
*    desc:  Constructer
************************************************************************/
CVertBufMgr::CVertBufMgr()
    : currentMaxFontIndices(0)
{
}


/************************************************************************
*    desc:  destructer                                                             
************************************************************************/
CVertBufMgr::~CVertBufMgr()
{
    for( auto & mapMapIter : m_vertexBufMapMap )
        for( auto & mapIter : mapMapIter.second )
            delete[] mapIter.second;

    for( auto & mapMapIter : m_indexBufMapMap )
        for( auto & mapIter : mapMapIter.second )
            delete[] mapIter.second;
}


/************************************************************************
*    desc:  Create a 2D quad VBO buffer
************************************************************************/
float * CVertBufMgr::createQuadVBO( const std::string & group, const std::string & name, const CRect<float> & uv )
{
    // Create the map group if it doesn't already exist
    auto mapMapIter = m_vertexBufMapMap.find( group );
    if( mapMapIter == m_vertexBufMapMap.end() )
            mapMapIter = m_vertexBufMapMap.insert( std::make_pair(group, std::map<const std::string, float *>()) ).first;

    // See if this vertex buffer ID has already been loaded
    auto mapIter = mapMapIter->second.find( name );

    // If it's not found, create the vertex buffer and add it to the list
    if( mapIter == mapMapIter->second.end() )
    {
        // Test VBO data
        // UV x starts from the top left. UV y starts from the bottom right. Clockwise
        // verts start from the bottom left. Counter clockwise
        // Layout: vert(x,y,z), uv(u,v), norm(x,y,z)
        float vertexData[] =
        {
            -0.5f, -0.5f, 0.0,  uv.x1, uv.y1,  0.0, 0.0, 0.0,
             0.5f, -0.5f, 0.0,  uv.x2, uv.y1,  0.0, 0.0, 0.0,
             0.5f,  0.5f, 0.0,  uv.x2, uv.y2,  0.0, 0.0, 0.0,
            -0.5f,  0.5f, 0.0,  uv.x1, uv.y2,  0.0, 0.0, 0.0
        };

        uint size = sizeof(CQuad2D) / sizeof(float);
        float * pVBO = new float[size];
        std::memcpy( pVBO, vertexData, sizeof(CQuad2D) );

        // Insert the new vertex buffer info
        mapIter = mapMapIter->second.insert( std::make_pair(name, pVBO) ).first;
    }

    return mapIter->second;
}


/************************************************************************
*    desc:  Create a mesh VBO buffer
************************************************************************/
float * CVertBufMgr::createMeshVBO( const std::string & group, const std::string & name, float * vertexData, int sizeInBytes )
{
    // Create the map group if it doesn't already exist
    auto mapMapIter = m_vertexBufMapMap.find( group );
    if( mapMapIter == m_vertexBufMapMap.end() )
            mapMapIter = m_vertexBufMapMap.insert( std::make_pair(group, std::map<const std::string, float *>()) ).first;

    // See if this vertex buffer ID has already been loaded
    auto mapIter = mapMapIter->second.find( name );

    // If it's not found, create the vertex buffer and add it to the list
    if( mapIter == mapMapIter->second.end() )
    {
        uint size = sizeInBytes / sizeof(float);
        float * pVBO = new float[size];
        std::memcpy( pVBO, vertexData, sizeInBytes );

        // Insert the new vertex buffer info
        mapIter = mapMapIter->second.insert( std::make_pair(name, pVBO) ).first;
    }

    return mapIter->second;
}


/************************************************************************
*    desc:  Create a IBO buffer
************************************************************************/
uint * CVertBufMgr::createIBO( const std::string & group, const std::string & name, uint indexData[], int sizeInBytes )
{
    // Create the map group if it doesn't already exist
    auto mapMapIter = m_indexBufMapMap.find( group );
    if( mapMapIter == m_indexBufMapMap.end() )
            mapMapIter = m_indexBufMapMap.insert( std::make_pair(group, std::map<const std::string, uint *>()) ).first;

    // See if this intex buffer ID has already been loaded
    auto mapIter = mapMapIter->second.find( name );

    // If it's not found, create the intex buffer and add it to the list
    if( mapIter == mapMapIter->second.end() )
    {
        uint size = sizeInBytes / sizeof(uint);
        uint * pIBO = new uint[size];
        std::memcpy( pIBO, indexData, sizeInBytes );

        // Insert the new intex buffer info
        mapIter = mapMapIter->second.insert( std::make_pair(name, pIBO) ).first;
    }

    return mapIter->second;
}


/************************************************************************
*    desc:  Create a dynamic font IBO buffer
************************************************************************/
uint * CVertBufMgr::CreateDynamicFontIBO( const std::string & group, const std::string & name, unsigned short * pIndexData, int maxIndicies )
{
    return nullptr;
}


/************************************************************************
*    desc:  Create a scaled frame
************************************************************************/
float * CVertBufMgr::createScaledFrame( const std::string & group,
                                     const std::string & name,
                                     const CScaledFrame & scaledFrame,
                                     const CSize<int> & textSize,
                                     const CSize<int> & size )
{
    // Create the map group if it doesn't already exist
    auto mapMapIter = m_vertexBufMapMap.find( group );
    if( mapMapIter == m_vertexBufMapMap.end() )
            mapMapIter = m_vertexBufMapMap.insert( std::make_pair(group, std::map<const std::string, float *>()) ).first;

    // See if this vertex buffer ID has already been loaded
    auto mapIter = mapMapIter->second.find( name );

    // If it's not found, create the vertex buffer and add it to the list
    if( mapIter == mapMapIter->second.end() )
    {
        // Offsets to center the mesh
        const CPoint<float> center((size.w / 2.f), (size.h / 2.f));
        const CSize<float> frameLgth( (float)size.w - (scaledFrame.m_frame.w * 2.f), (float)size.h - (scaledFrame.m_frame.h * 2.f) );
        const CSize<float> uvLgth( textSize.w - (scaledFrame.m_frame.w * 2.f), textSize.h - (scaledFrame.m_frame.h * 2.f) );

        CQuad2D quadBuf[8];
        CVertex vertBuf[16];

        // Left frame
        createQuad( CPoint<float>(-center.x, center.y-scaledFrame.m_frame.h),
                    CSize<float>(scaledFrame.m_frame.w, -frameLgth.h),
                    CUV<float>(0, scaledFrame.m_frame.h),
                    CSize<float>(scaledFrame.m_frame.w, uvLgth.h),
                    textSize, size, quadBuf[0] );

        // top left
        createQuad( CPoint<float>(-center.x, center.y),
                    CSize<float>(scaledFrame.m_frame.w, -scaledFrame.m_frame.h),
                    CUV<float>(0, 0),
                    CSize<float>(scaledFrame.m_frame.w, scaledFrame.m_frame.h),
                    textSize, size, quadBuf[1] );

        // top
        createQuad( CPoint<float>(-(center.x-scaledFrame.m_frame.w), center.y),
                    CSize<float>(frameLgth.w, -scaledFrame.m_frame.h),
                    CUV<float>(scaledFrame.m_frame.w, 0),
                    CSize<float>(uvLgth.w, scaledFrame.m_frame.h),
                    textSize, size, quadBuf[2] );

        // top right
        createQuad( CPoint<float>(center.x-scaledFrame.m_frame.w, center.y),
                    CSize<float>(scaledFrame.m_frame.w, -scaledFrame.m_frame.h),
                    CUV<float>(scaledFrame.m_frame.w + uvLgth.w,0),
                    CSize<float>(scaledFrame.m_frame.w, scaledFrame.m_frame.h),
                    textSize, size, quadBuf[3] );

        // right frame
        createQuad( CPoint<float>(center.x-scaledFrame.m_frame.w, center.y-scaledFrame.m_frame.h),
                    CSize<float>(scaledFrame.m_frame.w, -frameLgth.h),
                    CUV<float>(scaledFrame.m_frame.w + uvLgth.w, scaledFrame.m_frame.h),
                    CSize<float>(scaledFrame.m_frame.w, uvLgth.h),
                    textSize, size, quadBuf[4] );

        // bottom right
        createQuad( CPoint<float>(center.x-scaledFrame.m_frame.w, -(center.y-scaledFrame.m_frame.h)),
                    CSize<float>(scaledFrame.m_frame.w, -scaledFrame.m_frame.h),
                    CUV<float>(scaledFrame.m_frame.w + uvLgth.w, scaledFrame.m_frame.h + uvLgth.h),
                    CSize<float>(scaledFrame.m_frame.w, scaledFrame.m_frame.h),
                    textSize, size, quadBuf[5] );

        // bottom frame
        createQuad( CPoint<float>(-(center.x-scaledFrame.m_frame.w), -(center.y-scaledFrame.m_frame.h)),
                    CSize<float>(frameLgth.w, -scaledFrame.m_frame.h),
                    CUV<float>(scaledFrame.m_frame.w, scaledFrame.m_frame.h + uvLgth.h),
                    CSize<float>(uvLgth.w, scaledFrame.m_frame.h),
                    textSize, size, quadBuf[6] );

        // bottom left
        createQuad( CPoint<float>(-center.x, -(center.y-scaledFrame.m_frame.h)),
                    CSize<float>(scaledFrame.m_frame.w, -scaledFrame.m_frame.h),
                    CUV<float>(0, scaledFrame.m_frame.h + uvLgth.h),
                    CSize<float>(scaledFrame.m_frame.w, scaledFrame.m_frame.h),
                    textSize, size, quadBuf[7] );

        // Piece together the needed unique verts
        vertBuf[0] = quadBuf[0].vert[0];
        vertBuf[1] = quadBuf[0].vert[1];
        vertBuf[2] = quadBuf[0].vert[2];
        vertBuf[3] = quadBuf[0].vert[3];
        vertBuf[4] = quadBuf[1].vert[1];
        vertBuf[5] = quadBuf[1].vert[2];
        vertBuf[6] = quadBuf[2].vert[1];
        vertBuf[7] = quadBuf[2].vert[3];
        vertBuf[8] = quadBuf[3].vert[1];
        vertBuf[9] = quadBuf[3].vert[3];
        vertBuf[10] = quadBuf[4].vert[0];
        vertBuf[11] = quadBuf[4].vert[3];
        vertBuf[12] = quadBuf[5].vert[0];
        vertBuf[13] = quadBuf[5].vert[3];
        vertBuf[14] = quadBuf[6].vert[0];
        vertBuf[15] = quadBuf[7].vert[0];

        uint vboSize = sizeof(CVertex) * 16 / sizeof(float);
        float * pVBO = new float[vboSize];
        std::memcpy( pVBO, (float*)vertBuf, sizeof(CVertex) * 16 );

        // Insert the new vertex buffer info
        mapIter = mapMapIter->second.insert( std::make_pair(name, pVBO) ).first;
    }

    return mapIter->second;
}


/************************************************************************
*    desc:  Create a quad
************************************************************************/
void CVertBufMgr::createQuad( 
    const CPoint<float> & vert,
    const CSize<float> & vSize,
    const CUV<float> & uv,
    const CSize<float> & uvSize,
    const CSize<float> & textSize,
    const CSize<float> & size,
    CQuad2D & quadBuf )
{
    // Check if the width or height is odd. If so, we offset 
    // by 0.5 for proper orthographic rendering
    float additionalOffsetX = 0;
    if( (int)size.w % 2 != 0 )
        additionalOffsetX = 0.5f;

    float additionalOffsetY = 0;
    if( (int)size.h % 2 != 0 )
        additionalOffsetY = 0.5f;

    // Calculate the third vertex of the first face
    quadBuf.vert[0].vert.x = vert.x + additionalOffsetX;
    quadBuf.vert[0].vert.y = vert.y + additionalOffsetY + vSize.h;
    quadBuf.vert[0].uv.u = uv.u / textSize.w;
    quadBuf.vert[0].uv.v = (uv.v + uvSize.h) / textSize.h;

    // Calculate the second vertex of the first face
    quadBuf.vert[1].vert.x = vert.x + additionalOffsetX + vSize.w;
    quadBuf.vert[1].vert.y = vert.y + additionalOffsetY;
    quadBuf.vert[1].uv.u = (uv.u + uvSize.w) / textSize.w;
    quadBuf.vert[1].uv.v = uv.v / textSize.h;

    // Calculate the first vertex of the first face
    quadBuf.vert[2].vert.x = vert.x + additionalOffsetX;
    quadBuf.vert[2].vert.y = vert.y + additionalOffsetY;
    quadBuf.vert[2].uv.u = uv.u / textSize.w;
    quadBuf.vert[2].uv.v = uv.v / textSize.h;

    // Calculate the second vertex of the second face
    quadBuf.vert[3].vert.x = vert.x + additionalOffsetX + vSize.w;
    quadBuf.vert[3].vert.y = vert.y + additionalOffsetY + vSize.h;
    quadBuf.vert[3].uv.u = (uv.u + uvSize.w) / textSize.w;
    quadBuf.vert[3].uv.v = (uv.v + uvSize.h) / textSize.h;
}


/************************************************************************
*    desc:  Delete buffer group
************************************************************************/
void CVertBufMgr::deleteBufferGroup( const std::string & group )
{
    {
        auto mapMapIter = m_vertexBufMapMap.find( group );
        if( mapMapIter != m_vertexBufMapMap.end() )
        {
            for( auto & mapIter : mapMapIter->second )
                delete[] mapIter.second;

            m_vertexBufMapMap.erase( mapMapIter );
        }
    }

    {
        auto mapMapIter = m_indexBufMapMap.find( group );
        if( mapMapIter != m_indexBufMapMap.end() )
        {
            for( auto & mapIter : mapMapIter->second )
                delete[] mapIter.second;

            m_indexBufMapMap.erase( mapMapIter );
        }
    }
}
