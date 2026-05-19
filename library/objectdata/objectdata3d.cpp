
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
}   // constructor


/************************************************************************
*    desc:  Copy Constructer
************************************************************************/
CObjectData3D::CObjectData3D( const CObjectData3D & obj )
{
    *this = obj;

}   // constructor


/************************************************************************
*    desc:  destructer                                                             
************************************************************************/
CObjectData3D::~CObjectData3D()
{
    // NOTE: Nothing should ever be deleted here
}   // destructer


/************************************************************************
*    desc:  Load the object data from the passed in node
************************************************************************/
void CObjectData3D::LoadFromNode( const XMLNode & node, const std::string & group, const std::string & name )
{
    m_name = name;
    m_group = group;

    // Load the scale
    m_scale = NParseHelper::LoadScale( node );

    // Load the size
    m_size = NParseHelper::LoadSize( node );

    // Load the visual data
    m_visualData.LoadFromNode( node );

}   // LoadObjectFromNode


/************************************************************************
*    desc:  Create the objects from data
************************************************************************/
void CObjectData3D::CreateFromData( const std::string & group )
{
    // Create the visuales
    m_visualData.CreateFromData( group, m_size );

    // Calculate the radii
    m_radiusSquared = pow((float)m_size.w / 2, 2) + pow((float)m_size.h / 2, 2);
    m_radius = sqrt( m_radiusSquared );

}   // CreateFromData


/************************************************************************
*    desc:  Access functions for the visual data
************************************************************************/
const CObjectVisualData3D & CObjectData3D::GetVisualData() const 
{
    return m_visualData;

}   // GetVisualData


/************************************************************************
*    desc:  Access functions for the data name
************************************************************************/
const std::string & CObjectData3D::GetName() const 
{
    return m_name;

}   // GetName


/************************************************************************
*    desc:  Access functions for the data group
************************************************************************/
const std::string & CObjectData3D::GetGroup() const 
{
    return m_group;

}   // GetGroup


/************************************************************************
*    desc:  Access functions for the size
************************************************************************/
const CSize<int> & CObjectData3D::GetSize() const 
{
    return m_size;

}   // GetSize


/************************************************************************
*    desc:  Access functions for the scale
************************************************************************/
const CPoint<float> & CObjectData3D::GetScale() const 
{
    return m_scale;

}   // GetScale


/************************************************************************
*    desc:  Access functions for the radius
************************************************************************/
float CObjectData3D::GetRadius() const 
{
    return m_radius;

}   // GetRadius


/************************************************************************
*    desc:  Access functions for the radius squared
************************************************************************/
float CObjectData3D::GetRadiusSquared() const 
{
    return m_radiusSquared;

}   // GetRadiusSquared
