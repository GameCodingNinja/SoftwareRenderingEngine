
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
#include <map>

// Game lib dependencies
#include <common/vertex.h>
#include <common/defs.h>

/************************************************************************
*    Loaded mesh data returned by the OBJ loader
************************************************************************/
struct CMeshGroup
{
    std::string material;
    std::vector<uint> indices;
};

struct CObjMeshData
{
    // Flattened vertex data (one CVertex3d per unique v/vt/vn combo)
    std::vector<CVertex3d> vertices;

    // Material groups — each group has a material name and its indices
    std::vector<CMeshGroup> groups;

    // Material name → texture file path (from .mtl)
    std::map<std::string, std::string> materialTextures;
};

/************************************************************************
*    OBJ mesh file loader namespace
************************************************************************/
namespace NObjMeshLoader
{
    // Load an OBJ mesh file and its associated MTL file.
    // The mtl file is expected to have the same name as the obj file
    // but with a .mtl extension, in the same directory.
    CObjMeshData load( const std::string & filePath );
}

#endif
