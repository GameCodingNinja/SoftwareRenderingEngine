
/************************************************************************
*    FILE NAME:       objectdata2d.cpp
*
*    DESCRIPTION:     Class that holds a 2D object data for
************************************************************************/

// Physical component dependency
#include <objectdata/objectdata2d.h>

// Standard lib dependencies
#include <math.h>

// Boost lib dependencies

// Game lib dependencies
#include <utilities/exceptionhandling.h>
#include <utilities/xmlparsehelper.h>

/************************************************************************
*    desc:  Constructer
************************************************************************/
CObjectData2D::CObjectData2D()
    : m_scale(1,1,1),
      m_radius(0),
      m_radiusSquared(0)
{
}   // constructor


/************************************************************************
*    desc:  Copy Constructer
************************************************************************/
CObjectData2D::CObjectData2D( const CObjectData2D & obj )
{
    *this = obj;

}   // constructor


/************************************************************************
*    desc:  destructer                                                             
************************************************************************/
CObjectData2D::~CObjectData2D()
{
    // NOTE: Nothing should ever be deleted here
}   // destructer


/************************************************************************
*    desc:  Load the object data from the passed in node
************************************************************************/
void CObjectData2D::LoadFromNode( const XMLNode & node, const std::string & group, const std::string & name )
{
    m_name = name;
    m_group = group;

    // Load the scale
    m_scale = NParseHelper::LoadScale( node );

    // Load the size
    m_size = NParseHelper::LoadSize( node );

    // Load the visual data
    m_visualData.LoadFromNode( node );

    // Load the physics data
    m_physicsData.LoadFromNode( node );

}   // LoadObjectFromNode


/************************************************************************
*    desc:  Create the objects from data
************************************************************************/
void CObjectData2D::CreateFromData( const std::string & group )
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
const CObjectVisualData2D & CObjectData2D::GetVisualData() const 
{
    return m_visualData;

}   // GetVisualData


/************************************************************************
*    desc:  Access functions for the physics data
************************************************************************/
const CObjectPhysicsData2D & CObjectData2D::GetPhysicsData() const 
{
    return m_physicsData;

}   // GetPhysicsData


/************************************************************************
*    desc:  Access functions for the data name
************************************************************************/
const std::string & CObjectData2D::GetName() const 
{
    return m_name;

}   // GetName


/************************************************************************
*    desc:  Access functions for the data group
************************************************************************/
const std::string & CObjectData2D::GetGroup() const 
{
    return m_group;

}   // GetGroup


/************************************************************************
*    desc:  Access functions for the size
************************************************************************/
const CSize<int> & CObjectData2D::GetSize() const 
{
    return m_size;

}   // GetSize


/************************************************************************
*    desc:  Access functions for the scale
************************************************************************/
const CPoint<float> & CObjectData2D::GetScale() const 
{
    return m_scale;

}   // GetScale


/************************************************************************
*    desc:  Access functions for the radius
************************************************************************/
float CObjectData2D::GetRadius() const 
{
    return m_radius;

}   // GetRadius


/************************************************************************
*    desc:  Access functions for the radius squared
************************************************************************/
float CObjectData2D::GetRadiusSquared() const 
{
    return m_radiusSquared;

}   // GetRadiusSquared


/************************************************************************
*    desc:  Is the generation type font
************************************************************************/
bool CObjectData2D::IsGenTypeFont() const 
{
    return m_visualData.GetGenerationType() == NDefs::EGT_FONT;

}   // IsGenTypeFont
