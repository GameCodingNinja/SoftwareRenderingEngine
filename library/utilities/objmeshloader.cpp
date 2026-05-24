
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
#include <map>

// Game lib dependencies
#include <utilities/exceptionhandling.h>
#include <utilities/genfunc.h>


/************************************************************************
*    desc:  Load an OBJ mesh file
************************************************************************/
void CObjMeshLoader::load( const std::string & filePath )
{
    std::ifstream file( filePath );
    if( !file.is_open() )
        throw NExcept::CCriticalException("OBJ Loader Error!",
            NGenFunc::FormatString("Error loading OBJ file (%s).\n\n%s\nLine: %d",
                filePath, __FUNCTION__, __LINE__));

    // Temp storage for raw OBJ data
    std::vector<CPoint<float>> positions;
    std::vector<CUV<float>> texCoords;
    std::vector<CPoint<float>> normals;

    // Map from "v/vt/vn" key string to vertex index for deduplication
    std::map<std::string, uint> vertexMap;

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
        else if( prefix == "f" )
        {
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
                    m_indices.push_back( it->second );
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
                    CVertex vertex;
                    vertex.vert = positions[indices[0] - 1];
                    vertex.uv   = texCoords[indices[1] - 1];
                    vertex.norm  = normals[indices[2] - 1];

                    uint newIndex = static_cast<uint>(m_vertices.size());
                    m_vertices.push_back( vertex );
                    vertexMap[triplet] = newIndex;
                    m_indices.push_back( newIndex );
                }
            }
        }
    }
}
