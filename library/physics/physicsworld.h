
/************************************************************************
*    FILE NAME:       physicsworld.h
*
*    DESCRIPTION:     Wrapper class for Box2D's b2World
************************************************************************/

#ifndef __physics_world_h__
#define __physics_world_h__

// Standard lib dependencies
#include <set>

// Game lib dependencies
#include <Box2D/Box2D.h>
#include <common/point.h>

// Forward declaration(s)
struct XMLNode;

class CPhysicsWorld
{
public:

    // Constructor
    CPhysicsWorld();

    // Destructor
    virtual ~CPhysicsWorld();

    // Load the physics world from the passed in node
    void LoadFromNode( const XMLNode & node );

    // Get the physics world
    const b2World & GetWorld() const;

    // Create and destroy a physics body
    b2Body * CreateBody( const b2BodyDef * pDef );
    void DestroyBody( b2Body * pBody );

    // Perform physics simulation
    void Step();

    // Update the step timer
    void UpdateStepTimer();

    // Whether we will begin a step this frame
    bool BeginStep() const;

    // Get the focus
    const CPoint<int> & GetFocus() const;

    // Set the fps to run the simulation at
    void SetFPS( float fps );

    // The the time ratio
    float GetTimeRatio() const;

    // Set-Get the activity of the physics world
    void SetActive( bool value );
    bool IsActive() const;

    // Clear the physics world
    void Clear();

private:

    // Box2D world
    b2World world;

    // World focus point
    CPoint<int> focus;

    // All bodies that are handled by this physics world
    std::set<b2Body *> pBodySet;

    // If we're actively running simulations
    bool active;

    // If we're going to start a step this frame
    bool beginStep;

    // Timer to let us know when to do another step
    float timer;

    // The ammount of time to simulate in milliseconds
    float stepTime;

    // The ratio of time between steps
    float timeRatio;

    // The number of velocity and position steps to calculate
    int velStepCount;
    int posStepCount;

};

#endif  // __physics_world_h__