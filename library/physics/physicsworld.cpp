
/************************************************************************
*    FILE NAME:       physicsworld.cpp
*
*    DESCRIPTION:     Wrapper class for Box2D's b2World
************************************************************************/

// Physical component dependency
#include <physics/physicsworld.h>

// Game lib dependencies
#include <utilities/xmlParser.h>
#include <utilities/highresolutiontimer.h>
#include <utilities/mathfunc.h>

/************************************************************************
*    desc:  Constructer
************************************************************************/
CPhysicsWorld::CPhysicsWorld()
             : world( b2Vec2(0,0) ),
               active(false),
               beginStep(false),
               timer(0),
               stepTime(0),
               timeRatio(0),
               velStepCount(6),
               posStepCount(2)
{
    SetFPS(60);

}   // constructor


/************************************************************************
*    desc:  destructer                                                             
************************************************************************/
CPhysicsWorld::~CPhysicsWorld()
{
    Clear();

}	// destructer


/************************************************************************
*    desc:  Load the physics world from the passed in XML node
*
*	 param:	const XMLNode & node - node to get data from
************************************************************************/
void CPhysicsWorld::LoadFromNode( const XMLNode & node )
{
    // Get the world's settings, if any are set
    XMLNode settingsNode = node.getChildNode( "settings" );

    if( !settingsNode.isEmpty() )
    {
        if( settingsNode.isAttributeSet( "active" ) )
            if( settingsNode.getAttribute( "active" ) == "true" )
                active = true;
        
        if( settingsNode.isAttributeSet( "fps" ) )
            SetFPS( atof( settingsNode.getAttribute( "fps" ) ) );
    }

    // Get the world's gravity, if any are set
    XMLNode gravityNode = node.getChildNode( "gravity" );

    if( !gravityNode.isEmpty() )
    {
        b2Vec2 gravity;
        gravity.x = atof( gravityNode.getAttribute( "x" ) );
        gravity.y = atof( gravityNode.getAttribute( "y" ) );

        world.SetGravity( gravity );
    }

}	// LoadFromXML


/************************************************************************
*    desc:  Get the world
*
*	 ret:	const b2World & - Box2D world
************************************************************************/
const b2World & CPhysicsWorld::GetWorld() const
{
    return world;

}	// GetWorld


/************************************************************************
*    desc:  Create a physics body
*
*	 param:	const b2BodyDef * pDef - body definition
*
*	 ret:	const b2World & - Box2D world
************************************************************************/
b2Body * CPhysicsWorld::CreateBody( const b2BodyDef * pDef )
{
    b2Body * pTmpBody = world.CreateBody( pDef );
    pBodySet.insert( pTmpBody );

    return pTmpBody;

}	// CreateBody


/************************************************************************
*    desc:  Destroy a physics body
*
*	 param:	b2Body * pBody - body to destroy
************************************************************************/
void CPhysicsWorld::DestroyBody( b2Body * pBody )
{
    auto iter = pBodySet.find( pBody );

    if( iter != pBodySet.end() )
    {
        // Set the collision sprite's body to null, just in case
        //((CCollisionSprite2D *)(*iter)->GetUserData())->SetBody( NULL );

        // Destroy the body
        world.DestroyBody( pBody );

        // Remove the body from the set
        pBodySet.erase( iter );
    }

}	// DestroyBody


/************************************************************************
*    desc:  Perform physics simulation
************************************************************************/
void CPhysicsWorld::Step()
{
    // If the world isn't active, we leave
    if( !active )
        return;

    // We check to see if we can begin another step
    if( beginStep )
    {
        // If so, update all of the collision sprites in the world with their
        // new pre and post positions
        for( auto iter = pBodySet.begin();
             iter != pBodySet.end();
             ++iter )
        {
            //((CCollisionSprite2D *)(*iter)->GetUserData())->SetPrePostData();
        }

        // Begin the physics world step
        world.Step( stepTime / 1000.f, velStepCount, posStepCount );
    }
    
}	// Step


/************************************************************************
*    desc:  Update the step timer
************************************************************************/
void CPhysicsWorld::UpdateStepTimer()
{
    // If the world isn't active, we leave
    if( !active )
        return;

    // Increment the timer
    timer += CHighResTimer::Instance().GetElapsedTime();

    // If the timer becomes greater than the step time, we can begin another step
    if( timer > stepTime )
    {
        timer = NMathFunc::Modulus( timer, stepTime );
        timeRatio = timer / stepTime;
        beginStep = true;
    }
    else
    {
        timeRatio = timer / stepTime;
        beginStep = false;
    }

}	// UpdateStepTimer


/************************************************************************
*    desc:  Whether we will begin a step this frame
*
*	 ret:	bool - result
************************************************************************/
bool CPhysicsWorld::BeginStep() const
{
    return beginStep;

}	// BeginStep


/************************************************************************
*    desc:  Get the focus
*
*	 ret:	const CPointInt & - focus of the physics world
************************************************************************/
const CPoint<int> & CPhysicsWorld::GetFocus() const
{
    return focus;

}	// GetFocus


/************************************************************************
*    desc:  Set the fps to run the simulation at
*
*	 param:	float fps - fps to set to
************************************************************************/
void CPhysicsWorld::SetFPS( float fps )
{
    // Make sure we don't have a negative or zero fps
    if( fps > 0.f )
    {
        // Calculate the step paramaters
        stepTime = 1.f / fps * 1000;
        
        // Set the timer so that we'll begin a step next time we call Update
        timer = stepTime + 1;
    }

}	// SetFPS


/************************************************************************
*    desc:  The the time ratio
*
*	 ret:	float - ratio
************************************************************************/
float CPhysicsWorld::GetTimeRatio() const
{
    return timeRatio;

}	// GetTimeRatio


/************************************************************************
*    desc:  Set the activity of the physics world
*
*	 param:	bool value - value to set
************************************************************************/
void CPhysicsWorld::SetActive( bool value )
{
    active = value;

}	// SetActive


/************************************************************************
*    desc:  Is the physics world active
*
*	 ret:	bool - result
************************************************************************/
bool CPhysicsWorld::IsActive() const
{
    return active;

}	// IsActive


/************************************************************************
*    desc:  Clear the physics world
************************************************************************/
void CPhysicsWorld::Clear()
{
    auto iter = pBodySet.begin();

    while( iter != pBodySet.end() )
    {
        // Set the collision sprite's body to null, just in case
        //((CCollisionSprite2D *)(*iter)->GetUserData())->SetBody( NULL );
        
        // Destroy the body
        world.DestroyBody( *iter );

        // Remove the body from the set
        iter = pBodySet.erase( iter );
    }

}	// Clear