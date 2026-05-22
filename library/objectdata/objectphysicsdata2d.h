
/************************************************************************
*    FILE NAME:       objectphysics2d.h
*
*    DESCRIPTION:     Class containing the 2D object's physics data
************************************************************************/

#ifndef __object_physics_data_2d_h__
#define __object_physics_data_2d_h__

// Standard lib dependencies
#include <string>

// Game lib dependencies
//#include <Box2D/Box2D.h>
#include <common/color.h>
#include <common/size.h>

enum EColGenType
{
    ECGT_NULL,
    ECGT_DEFAULT,
    ECGT_TOP_LEFT,
    ECGT_TOP_RIGHT,
    ECGT_BOTTOM_LEFT,
    ECGT_BOTTOM_RIGHT
};

// Forward Declarations
struct XMLNode;

class CObjectPhysicsData2D
{
public:
    
    // Constructer
    CObjectPhysicsData2D();
    ~CObjectPhysicsData2D();

    // Load thes object data from node
    void loadFromNode( const XMLNode & objectNode );

    // Get the file path
    const std::string & getFile() const;

    // Get the type of physics sprite
    //b2BodyType GetType() const;

    // Get the physics world name
    const std::string & getWorld() const;

    // Set-Get the size
    const CSize<float> & getSize() const;

    // Get the mesh top side mod
    float getTopMod() const;

    // Get the mesh bottom side mod
    float getBottomMod() const;

    // Get the mesh left side mod
    float getLeftMod() const;

    // Get the mesh right side mode
    float getRightMod() const;

    // Get the mesh generation type
    EColGenType getGenType() const;

    // Get the density
    float getDensity() const;

    // Get the restitution
    float getRestitution() const;

    // Get the damping
    float getDamping() const;
    float getAngDamping() const;

    // Is the rotation fixed
    bool isRotationFixed() const;

    // Get the color
    const CColor<float> & getColor() const;

private:

    // Mesh file
    std::string m_file;

    // Physics world name
    std::string m_world;

    // Type of physics sprite
    //b2BodyType m_type;

    // Size of the physics mesh
    CSize<float> m_size;

    // Amount to adjust the top, bottom, left, and right side size of the mesh
    float m_topMod, m_bottomMod, m_leftMod, m_rightMod;

    // Type of physics generation used
    EColGenType m_genType;

    // The resistance to move  
    float m_density;

    // The percetange of velocity retained upon colliding with this object
    float m_restitution;

    // The constant decceleration of movement and rotation
    float m_damping, m_angDamping;

    // If we want to prevent the object from rotating due to physicss
    bool m_fixedRotation;

    // Color of the physics (this is only used for testing purposes)
    CColor<float> m_color;

};

#endif
