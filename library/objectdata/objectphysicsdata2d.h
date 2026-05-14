
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
    void LoadFromNode( const XMLNode & objectNode );

    // Get the file path
    const std::string & GetFile() const;

    // Get the type of physics sprite
    //b2BodyType GetType() const;

    // Get the physics world name
    const std::string & GetWorld() const;

    // Set-Get the size
    const CSize<float> & GetSize() const;

    // Get the mesh top side mod
    float GetTopMod() const;

    // Get the mesh bottom side mod
    float GetBottomMod() const;

    // Get the mesh left side mod
    float GetLeftMod() const;

    // Get the mesh right side mode
    float GetRightMod() const;

    // Get the mesh generation type
    EColGenType GetGenType() const;

    // Get the density
    float GetDensity() const;

    // Get the restitution
    float GetRestitution() const;

    // Get the damping
    float GetDamping() const;
    float GetAngDamping() const;

    // Is the rotation fixed
    bool IsRotationFixed() const;

    // Get the color
    const CColor & GetColor() const;

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
    CColor m_color;

};

#endif  // __object_physics_data_2d_h__
