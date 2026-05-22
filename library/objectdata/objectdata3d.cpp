
/************************************************************************
*    FILE NAME:       objectdata3d.cpp
*
*    DESCRIPTION:     Class that holds a 3D object data
************************************************************************/

// Physical component dependency
#include <objectdata/objectdata3d.h>

// Standard lib dependencies
#include <math.h>

// Game lib dependencies
#include <utilities/exceptionhandling.h>
#include <utilities/xmlparsehelper.h>

/************************************************************************
*    desc:  Constructer
************************************************************************/
CObjectData3D::CObjectData3D()
    : m_scale(1,1,1),
      m_radius(0),
      m_radiusSquared(0)
{
}


/************************************************************************
*    desc:  Copy Constructer
************************************************************************/
CObjectData3D::CObjectData3D( const CObjectData3D & obj )
{
    *this = obj;

}


/************************************************************************
*    desc:  destructer                                                             
************************************************************************/
CObjectData3D::~CObjectData3D()
{
    // NOTE: Nothing should ever be deleted here
}


/************************************************************************
*    desc:  Load the object data from the passed in node
************************************************************************/
void CObjectData3D::loadFromNode( const XMLNode & node, const std::string & group, const std::string & name )
{
    m_name = name;
    m_group = group;

    // Load the scale
    m_scale = NParseHelper::loadScale( node );

    // Load the size
    m_size = NParseHelper::loadSize( node );

    // Load the visual data
    m_visualData.loadFromNode( node );

}


/************************************************************************
*    desc:  Create the objects from data
************************************************************************/
void CObjectData3D::createFromData( const std::string & group )
{
    // Create the visuales
    m_visualData.createFromData( group, m_size );

    // Calculate the radii
    m_radiusSquared = pow((float)m_size.w / 2, 2) + pow((float)m_size.h / 2, 2);
    m_radius = sqrt( m_radiusSquared );

}


/************************************************************************
*    desc:  Access functions for the visual data
************************************************************************/
const CObjectVisualData3D & CObjectData3D::getVisualData() const 
{
    return m_visualData;

}


/************************************************************************
*    desc:  Access functions for the data name
************************************************************************/
const std::string & CObjectData3D::GetName() const 
{
    return m_name;

}


/************************************************************************
*    desc:  Access functions for the data group
************************************************************************/
const std::string & CObjectData3D::GetGroup() const 
{
    return m_group;

}


/************************************************************************
*    desc:  Access functions for the size
************************************************************************/
const CSize<int> & CObjectData3D::getSize() const 
{
    return m_size;

}


/************************************************************************
*    desc:  Access functions for the scale
************************************************************************/
const CPoint<float> & CObjectData3D::GetScale() const 
{
    return m_scale;

}


/************************************************************************
*    desc:  Access functions for the radius
************************************************************************/
float CObjectData3D::GetRadius() const 
{
    return m_radius;

}


/************************************************************************
*    desc:  Access functions for the radius squared
************************************************************************/
float CObjectData3D::GetRadiusSquared() const 
{
    return m_radiusSquared;

}
