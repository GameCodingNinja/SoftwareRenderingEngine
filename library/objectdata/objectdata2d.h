/************************************************************************
*    FILE NAME:       objectdata2d.h
*
*    DESCRIPTION:     Class that holds a 2D object data for
************************************************************************/

#ifndef __object_data_2d_h__
#define __object_data_2d_h__

// Standard lib dependencies
#include <string>
#include <utility>

// Game lib dependencies
#include <common/point.h>
#include <objectdata/objectvisualdata2d.h>
#include <objectdata/objectphysicsdata2d.h>
#include <utilities/xmlParser.h>
//#include <2d/objectscriptdata2d.h>

// Forward Declarations
struct XMLNode;

class CObjectData2D
{
public:

    // Constructor/Destructor
    CObjectData2D();
    CObjectData2D( const CObjectData2D & obj );
    ~CObjectData2D();

    // Load the object data from the passed in node
    void LoadFromNode( const XMLNode & node, const std::string & group, const std::string & name );

    // Create the objects from data
    void CreateFromData( const std::string & group );

    // Access functions for the visual data
    const CObjectVisualData2D & GetVisualData() const;

    // Access functions for the physics data
    const CObjectPhysicsData2D & GetPhysicsData() const;

    // Access functions for the data name
    const std::string & GetName() const;

    // Access functions for the data group
    const std::string & GetGroup() const;

    // Access functions for the size
    const CSize<int> & GetSize() const;
    
    // Access functions for the scale
    const CPoint<float> & GetScale() const;

    // Access functions for the radius
    float GetRadius() const;

    // Access functions for the radius squared
    float GetRadiusSquared() const;

    // Is the generation type font
    bool IsGenTypeFont() const;

private:

    // Visual data of the object
    CObjectVisualData2D m_visualData;

    // Physics data of the object
    CObjectPhysicsData2D m_physicsData;

    // The name of the object data
    std::string m_name;

    // The group the object data is in
    std::string m_group;

    // The initial size of the object
    CSize<int> m_size;

    // The initial scale of the object
    CPoint<float> m_scale;

    // Square rooted and un-square rooted radius
    float m_radius;
    float m_radiusSquared;
};

#endif  // __object_data_2d_h__
