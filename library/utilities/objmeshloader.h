
/************************************************************************
*    FILE NAME:       objmeshloader.h
*
*    DESCRIPTION:     OBJ mesh file loader for 3D mesh data
************************************************************************/

#ifndef __obj_mesh_loader_h__
#define __obj_mesh_loader_h__

// Standard lib dependencies
#include <string>
#include <vector>

// Game lib dependencies
#include <common/vertex.h>
#include <common/defs.h>

class CObjMeshLoader
{
public:

    // Load an OBJ mesh file
    void load( const std::string & filePath );

    // Flattened vertex data (one CVertex per unique v/vt/vn combo)
    std::vector<CVertex> m_vertices;

    // Index data
    std::vector<uint> m_indices;

    // Pointers to unique vertex positions within m_vertices
    std::vector<CPoint<float>*> m_uniqueVerts;

    // Maps each vertex index to its unique position index
    std::vector<uint> m_vertToUniqueVec;
};

#endif
