
/************************************************************************
*    FILE NAME:       objmeshloader.cpp
*
*    DESCRIPTION:     OBJ mesh file loader for 3D mesh data
************************************************************************/

// Physical component dependency
#include <utilities/objmeshloader.h>

// Standard lib dependencies
#include <fstream>
#include <sstream>

// Game lib dependencies
#include <utilities/exceptionhandling.h>
#include <utilities/genfunc.h>

namespace NObjMeshLoader
{
    /************************************************************************
    *    desc:  Parse the MTL file and populate materialTextures map
    ************************************************************************/
    /************************************************************************
    *    desc:  Resolve a relative path against a base directory
    ************************************************************************/
    static std::string resolvePath( const std::string & baseDir, const std::string & relPath )
    {
        // Split the combined path into components and resolve ".."
        std::string combined = baseDir + "/" + relPath;
        std::vector<std::string> parts;
        std::istringstream stream( combined );
        std::string part;

        while( std::getline( stream, part, '/' ) )
        {
            if( part == ".." && !parts.empty() )
                parts.pop_back();
            else if( !part.empty() && part != "." )
                parts.push_back( part );
        }

        std::string result;
        for( size_t i = 0; i < parts.size(); ++i )
        {
            if( i > 0 ) result += "/";
            result += parts[i];
        }

        return result;
    }


    static void parseMtlFile(
        const std::string & mtlPath,
        std::map<std::string, std::string> & materialTextures )
    {
        std::ifstream file( mtlPath );
        if( !file.is_open() )
            throw NExcept::CCriticalException("OBJ Loader Error!",
                NGenFunc::FormatString("Error loading MTL file (%s).\n\n%s\nLine: %d",
                    mtlPath, __FUNCTION__, __LINE__));

        // Get the directory of the MTL file for resolving relative texture paths
        std::string mtlDir;
        auto lastSlash = mtlPath.rfind( '/' );
        if( lastSlash != std::string::npos )
            mtlDir = mtlPath.substr( 0, lastSlash );

        std::string line;
        std::string currentMaterial;

        while( std::getline( file, line ) )
        {
            std::istringstream iss( line );
            std::string prefix;
            iss >> prefix;

            if( prefix == "newmtl" )
            {
                iss >> currentMaterial;
            }
            else if( prefix == "map_Kd" )
            {
                std::string texPath;
                iss >> texPath;

                // Resolve relative path against the MTL file's directory
                if( !mtlDir.empty() && texPath.find("../") != std::string::npos )
                    texPath = resolvePath( mtlDir, texPath );

                materialTextures[currentMaterial] = texPath;
            }
        }
    }

    /************************************************************************
    *    desc:  Load an OBJ mesh file and its associated MTL file
    ************************************************************************/
    CObjMeshData load( const std::string & filePath )
    {
        std::ifstream file( filePath );
        if( !file.is_open() )
            throw NExcept::CCriticalException("OBJ Loader Error!",
                NGenFunc::FormatString("Error loading OBJ file (%s).\n\n%s\nLine: %d",
                    filePath, __FUNCTION__, __LINE__));

        CObjMeshData meshData;

        // Temp storage for raw OBJ data
        std::vector<CPoint<float>> positions;
        std::vector<CUV<float>> texCoords;
        std::vector<CPoint<float>> normals;

        // Map from "v/vt/vn" key string to vertex index for deduplication
        std::map<std::string, uint> vertexMap;

        // Current material group index (-1 means none created yet)
        int currentGroup = -1;

        std::string line;

        while( std::getline( file, line ) )
        {
            std::istringstream iss( line );
            std::string prefix;
            iss >> prefix;

            if( prefix == "v" )
            {
                float x, y, z;
                iss >> x >> y >> z;
                positions.emplace_back( x, y, z );
            }
            else if( prefix == "vt" )
            {
                float u, v;
                iss >> u >> v;
                texCoords.emplace_back( u, v );
            }
            else if( prefix == "vn" )
            {
                float x, y, z;
                iss >> x >> y >> z;
                normals.emplace_back( x, y, z );
            }
            else if( prefix == "usemtl" )
            {
                std::string matName;
                iss >> matName;

                // Create a new material group
                CMeshGroup group;
                group.material = matName;
                meshData.groups.push_back( group );
                currentGroup = static_cast<int>(meshData.groups.size()) - 1;
            }
            else if( prefix == "f" )
            {
                // If no usemtl was encountered, create a default group
                if( currentGroup < 0 )
                {
                    CMeshGroup group;
                    group.material = "default";
                    meshData.groups.push_back( group );
                    currentGroup = 0;
                }

                // Parse 3 face vertex triplets (triangle faces only)
                for( int i = 0; i < 3; ++i )
                {
                    std::string triplet;
                    iss >> triplet;

                    // Use the triplet string as the deduplication key
                    auto it = vertexMap.find( triplet );
                    if( it != vertexMap.end() )
                    {
                        // Reuse existing vertex index
                        meshData.groups[currentGroup].indices.push_back( it->second );
                    }
                    else
                    {
                        // Parse v/vt/vn indices
                        std::istringstream tripletStream( triplet );
                        std::string token;
                        int indices[3];
                        int idx = 0;

                        while( std::getline( tripletStream, token, '/' ) && idx < 3 )
                        {
                            indices[idx] = std::stoi( token );
                            ++idx;
                        }

                        // OBJ indices are 1-based
                        CVertex3d vertex;
                        vertex.vert = positions[indices[0] - 1];
                        vertex.uv   = texCoords[indices[1] - 1];
                        vertex.norm  = normals[indices[2] - 1];

                        uint newIndex = static_cast<uint>(meshData.vertices.size());
                        meshData.vertices.push_back( vertex );
                        vertexMap[triplet] = newIndex;
                        meshData.groups[currentGroup].indices.push_back( newIndex );
                    }
                }
            }
        }

        // Derive MTL file path by replacing .obj extension with .mtl
        std::string mtlPath = filePath;
        auto dotPos = mtlPath.rfind( '.' );
        if( dotPos != std::string::npos )
            mtlPath = mtlPath.substr( 0, dotPos ) + ".mtl";
        else
            mtlPath += ".mtl";

        // Parse the MTL file
        parseMtlFile( mtlPath, meshData.materialTextures );

        return meshData;
    }
}
