
/************************************************************************
*    FILE NAME:       physicsworldmanager.cpp
*
*    DESCRIPTION:     Box2D b2World wrapper class singleton
************************************************************************/

// Physical component dependency
#include <physics/physicsworldmanager.h>

// Boost lib dependencies
#include <boost/format.hpp>

// Game lib dependencies
#include <utilities/exceptionhandling.h>
#include <utilities/xmlParser.h>
#include <utilities/deletefuncs.h>
#include <physics/physicsworld.h>

/************************************************************************
*    desc:  Constructer
************************************************************************/
CPhysicsWorldManager::CPhysicsWorldManager()
{
}   // constructor


/************************************************************************
*    desc:  destructer                                                             
************************************************************************/
CPhysicsWorldManager::~CPhysicsWorldManager()
{
    NDelFunc::DeleteMapPointers( m_pWorldMap );

}	// destructer


/************************************************************************
*    desc:  Load the world
************************************************************************/
void CPhysicsWorldManager::LoadWorld( const std::string & group )
{
    // Make sure the group we are looking has been defined in the list table file
    auto listTableIter = m_listTableMap.find( group );
    if( listTableIter == m_listTableMap.end() )
        throw NExcept::CCriticalException("Obj Data List 2D Load Group Data Error!",
            boost::str( boost::format("Object data list group name can't be found (%s).\n\n%s\nLine: %s") 
                % group % __FUNCTION__ % __LINE__ ));

    // Load the group data if it doesn't already exist
    if( m_pWorldMap.find( group ) == m_pWorldMap.end() )
    {
        LoadFromXML( group, listTableIter->second.back() );
    }
    else
    {
        throw NExcept::CCriticalException("Physics world load Error!",
            boost::str( boost::format("Physics world group has alread been loaded (%s).\n\n%s\nLine: %s")
                % group % __FUNCTION__ % __LINE__ ));
    }

}   // LoadWorld


/************************************************************************
*    desc:  Load the physics worlds from an XML
*
*	 param:	const string & filePath - file to load
************************************************************************/
void CPhysicsWorldManager::LoadFromXML( const std::string & group, const std::string & filePath )
{
    // this open and parse the XML file:
    XMLNode mainNode = XMLNode::openFileHelper( filePath.c_str(), "physics" );

    // Create the world and add it to the map
    CPhysicsWorld * pPhysicsWorld = new CPhysicsWorld();
    m_pWorldMap.insert( std::make_pair(group, pPhysicsWorld) );

    // Have the physics world load the rest of its data
    pPhysicsWorld->LoadFromNode( mainNode );

}	// LoadFromXML


/************************************************************************
*    desc:  Get the physics world
************************************************************************/
CPhysicsWorld * CPhysicsWorldManager::GetWorld( const std::string & group )
{
    // Find the world
    auto iter = m_pWorldMap.find( group );

    // If we found it, return it. If not, we have a problem
    if( iter != m_pWorldMap.end() )
        return iter->second;

    else
        throw NExcept::CCriticalException("Physics World Manager Error!",
                boost::str( boost::format("Physics world doesn't exist (%s).\n\n%s\nLine: %s") 
                            % group.c_str() % __FUNCTION__ % __LINE__ ));

    return NULL;

}	// GetWorld


/************************************************************************
*    desc:  Delete a world
************************************************************************/
void CPhysicsWorldManager::DeleteWorld( const std::string & group )
{
    // Find the world
    auto iter = m_pWorldMap.find( group );

    // If we find it, delete it. If not, we have a problem
    if( iter != m_pWorldMap.end() )
        m_pWorldMap.erase( iter );
    
}	// DeleteWorld
