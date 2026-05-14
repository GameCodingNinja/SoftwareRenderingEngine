
/************************************************************************
*    FILE NAME:       physicsworld.h
*
*    DESCRIPTION:     Box2D b2World wrapper class singleton
************************************************************************/

#ifndef __physics_world_manager_h__
#define __physics_world_manager_h__

// Physical component dependency
#include <managers/managerbase.h>

// Standard lib dependencies
#include <string>
#include <map>

// Forward declaration(s)
class CPhysicsWorld;

class CPhysicsWorldManager : public CManagerBase
{
public:

    // Get the instance of the singleton class
    static CPhysicsWorldManager & Instance()
    {
        static CPhysicsWorldManager worldManager;
        return worldManager;
    }

    // Load the world
    void LoadWorld( const std::string & group );

    // Get the physics world
    CPhysicsWorld * GetWorld( const std::string & group );

    // Delete a world
    void DeleteWorld( const std::string & group );

private:

    // Constructor
    CPhysicsWorldManager();

    // Destructor
    ~CPhysicsWorldManager();

    // Load the physics worlds from an XML
    void LoadFromXML( const std::string & group, const std::string & LoadFromXML );

private:

    // Map of physics worlds
    std::map<const std::string, CPhysicsWorld *> m_pWorldMap;

};

#endif  // __physics_world_manager_h__