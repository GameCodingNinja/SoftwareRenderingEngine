
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

    // Flattened vertex data (one CVertex3d per unique v/vt/vn combo)
    std::vector<CVertex3d> m_vertices;

    // Index data
    std::vector<uint> m_indices;
};

#endif
