
/************************************************************************
*    FILE NAME:       objectdata3d.h
*
*    DESCRIPTION:     Class that holds a 3D object data
************************************************************************/

#ifndef __object_data_3d_h__
#define __object_data_3d_h__

// Standard lib dependencies
#include <string>
#include <utility>

// Game lib dependencies
#include <common/point.h>
#include <common/size.h>
#include <objectdata/iobjectdata.h>
#include <objectdata/objectvisualdata3d.h>
#include <utilities/xmlParser.h>

// Forward Declarations
struct XMLNode;

class CObjectData3D : public iObjectData
{
public:

    // Constructor/Destructor
    CObjectData3D();
    CObjectData3D( const CObjectData3D & obj );
    ~CObjectData3D();

    // Load the object data from the passed in node
    void loadFromNode( const XMLNode & node, const std::string & group, const std::string & name );

    // Create the objects from data
    void createFromData( const std::string & group );

    // Access functions for the visual data
    const CObjectVisualData3D & getVisualData() const;

    // Is this 3D data?
    bool Is3D() const override
    { return true; }

    // Access functions for the data name
    const std::string & GetName() const;

    // Access functions for the data group
    const std::string & GetGroup() const;

    // Access functions for the size
    const CSize<int> & getSize() const;
    
    // Access functions for the scale
    const CPoint<float> & GetScale() const;

    // Access functions for the radius
    float GetRadius() const;

    // Access functions for the radius squared
    float GetRadiusSquared() const;

private:

    // Visual data of the object
    CObjectVisualData3D m_visualData;

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

#endif
