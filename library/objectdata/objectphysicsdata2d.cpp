
/************************************************************************
*    FILE NAME:       objectphysicsdata2d.cpp
*
*    DESCRIPTION:     Class containing the 2D object's physics data
************************************************************************/

// Physical component dependency
#include <objectdata/objectphysicsdata2d.h>

// Boost lib dependencies
#include <boost/format.hpp>

// Game lib dependencies
#include <common/color.h>
#include <utilities/xmlparsehelper.h>

/************************************************************************
*    desc:  Constructer
************************************************************************/
CObjectPhysicsData2D::CObjectPhysicsData2D()
    : //type(b2_staticBody),
    m_topMod(0),
    m_bottomMod(0),
    m_rightMod(0),
    m_leftMod(0),
    m_genType(ECGT_NULL),
    m_density(0),
    m_restitution(0),
    m_damping(0),
    m_angDamping(0),
    m_fixedRotation(false)
{
}   // constructor


/************************************************************************
*    desc:  Destructer                                                             
************************************************************************/
CObjectPhysicsData2D::~CObjectPhysicsData2D()
{
    // NOTE: Nothing should ever be deleted here
}   // Destructer


/************************************************************************
*    desc:  Load thes object data from node
************************************************************************/
void CObjectPhysicsData2D::LoadFromNode( const XMLNode & objectNode )
{
    const XMLNode physicsNode = objectNode.getChildNode( "physics" );

    // Check if the object has any physics data
    if( !physicsNode.isEmpty() )
    {
        if( physicsNode.isAttributeSet("file") )
        {
            // Get the file name
            m_file = physicsNode.getAttribute( "file" );

            // Get the physics world name
            if( physicsNode.isAttributeSet( "world" ) )
                m_world = physicsNode.getAttribute( "world" );

            // Get the physics sprite type
            if( physicsNode.isAttributeSet( "type" ) )
            {
                std::string bodyType = physicsNode.getAttribute( "type" );

                /*if( bodyType == "dynamic" )
                        type = b2_dynamicBody;
                else if( bodyType == "kinematic" )
                        type = b2_kinematicBody;*/
            }

            // If the file is not a file but is asking to generate a file, we check
            // to see what kind of physics mesh it wants to generate
            if( m_file == "rect" )
            {
                // Default physics is a physics mesh that is the size of the visual mesh
                m_genType = ECGT_DEFAULT;

                // The other kinds of physics meshes that can be generate are triangular meshes
                // that take up a specific half of the visual mesh size
                if( physicsNode.isAttributeSet( "genType" ) )
                {
                    std::string genTypeStr = physicsNode.getAttribute( "genType" );

                    if( genTypeStr == "tl" || genTypeStr == "top_left" )
                        m_genType = ECGT_TOP_LEFT;

                    else if( genTypeStr == "tr"  || genTypeStr == "top_right" )
                        m_genType = ECGT_TOP_RIGHT;

                    else if( genTypeStr == "bl" || genTypeStr == "bottom_left" )
                        m_genType = ECGT_BOTTOM_LEFT;

                    else if( genTypeStr == "br" || genTypeStr == "bottom_right" )
                        m_genType = ECGT_BOTTOM_RIGHT;
                }
            }
        }

        // The size mod is how much the physics mesh should be adjusted on each side
        const XMLNode sizeModNode = physicsNode.getChildNode( "sizeMod" );

        // Check if the object has any physics data
        if( !sizeModNode.isEmpty() )
        {
            if( sizeModNode.isAttributeSet("top") )
                m_topMod = atof( sizeModNode.getAttribute( "top" ) );

            if( sizeModNode.isAttributeSet("bottom") )
                m_bottomMod = atof( sizeModNode.getAttribute( "bottom" ) );

            if( sizeModNode.isAttributeSet("left") )
                m_leftMod = atof( sizeModNode.getAttribute( "left" ) );

            if( sizeModNode.isAttributeSet("right") )
                m_rightMod = atof( sizeModNode.getAttribute( "right" ) );

            // If we've set the physics size, let's modify it
            if( !m_size.IsEmpty() )
            {
                m_size.w += m_rightMod + m_leftMod;
                m_size.h += m_topMod + m_bottomMod;
            }
        }

        // The body of the physics sprite used for physics
        const XMLNode bodyNode = physicsNode.getChildNode( "body" );

        if( !bodyNode.isEmpty() )
        {
            // The density is how much the object resists movement
            if( bodyNode.isAttributeSet("density") )
                m_density = atof( bodyNode.getAttribute( "density" ) );

            // The restitution is the percentage of velocity retained upon physics
            if( bodyNode.isAttributeSet("restitution") )
                m_restitution = atof( bodyNode.getAttribute( "restitution" ) );

            // The damping is the constant decceleration of movement and rotation
            if( bodyNode.isAttributeSet("damping") )
                m_damping = atof( bodyNode.getAttribute( "damping" ) );

            // The angular damping is the constant decceleration of rotation
            if( bodyNode.isAttributeSet("angDamping") )
                m_angDamping = atof( bodyNode.getAttribute( "angDamping" ) );

            // Whether the rotation due to physicss is fixed
            if( bodyNode.isAttributeSet("fixedRotation") )
                if( std::string( bodyNode.getAttribute("fixedRotation") ) == "true" )
                    m_fixedRotation = true;
        }

        // Set the color for testing purposes
        m_color = NParseHelper::LoadColor( physicsNode, m_color );
    }

}	// LoadFromNode


/************************************************************************
*    desc:  Get the file path
************************************************************************/
const std::string & CObjectPhysicsData2D::GetFile() const 
{
    return m_file;
}


/************************************************************************
*    desc:  Get the type of physics sprite
************************************************************************/
/*b2BodyType CObjectPhysicsData2D::GetType() const 
{
    return type;
}*/


/************************************************************************
*    desc:  Get the physics world name
************************************************************************/
const std::string & CObjectPhysicsData2D::GetWorld() const 
{
    return m_world;
}


/************************************************************************
*    desc:  Get the size
************************************************************************/
const CSize<float> & CObjectPhysicsData2D::GetSize() const
{
    return m_size;
}


/************************************************************************
*    desc:  Get the top mod
************************************************************************/
float CObjectPhysicsData2D::GetTopMod() const
{
    return m_topMod;
}


/************************************************************************
*    desc:  Get the bottom mod
************************************************************************/
float CObjectPhysicsData2D::GetBottomMod() const
{
    return m_bottomMod;
}


/************************************************************************
*    desc:  Get the left mod
************************************************************************/
float CObjectPhysicsData2D::GetLeftMod() const
{
    return m_leftMod;
}


/************************************************************************
*    desc:  Get the left mod
************************************************************************/
float CObjectPhysicsData2D::GetRightMod() const
{
    return m_rightMod;
}


/************************************************************************
*    desc:  Get the mass
************************************************************************/
EColGenType CObjectPhysicsData2D::GetGenType() const
{
    return m_genType;
}


/************************************************************************
*    desc:  Get the density
************************************************************************/
float CObjectPhysicsData2D::GetDensity() const
{
    return m_density;
}


/************************************************************************
*    desc:  Get the restitution
************************************************************************/
float CObjectPhysicsData2D::GetRestitution() const
{
    return m_restitution;
}


/************************************************************************
*    desc:  Get the damping
************************************************************************/
float CObjectPhysicsData2D::GetDamping() const
{
    return m_damping;
}


/************************************************************************
*    desc:  Get the angular damping
************************************************************************/
float CObjectPhysicsData2D::GetAngDamping() const
{
    return m_angDamping;
}


/************************************************************************
*    desc:  Is the rotation fixed
************************************************************************/
bool CObjectPhysicsData2D::IsRotationFixed() const
{
    return m_fixedRotation;
}


/************************************************************************
*    desc:  Get the color
************************************************************************/
const CColor & CObjectPhysicsData2D::GetColor() const 
{
    return m_color;
}

