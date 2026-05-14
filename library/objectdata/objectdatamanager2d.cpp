/************************************************************************
*    FILE NAME:       objectdatamanager2d.cpp
*
*    DESCRIPTION:    Class that holds a map of all 2D object data used for
*					 later loading
************************************************************************/

// Physical component dependency
#include <objectdata/objectdatamanager2d.h>

// Boost lib dependencies
#include <boost/format.hpp>

// Game lib dependencies
#include <utilities/exceptionhandling.h>
#include <utilities/xmlParser.h>
#include <utilities/deletefuncs.h>
#include <objectdata/objectdata2d.h>
#include <managers/vertexbuffermanager.h>
#include <managers/texturemanager.h>

/************************************************************************
*    desc:  Constructer                                                             
************************************************************************/
CObjectDataMgr2D::CObjectDataMgr2D()
{
}   // Constructer


/************************************************************************
*    desc:  Destructer                                                             
************************************************************************/
CObjectDataMgr2D::~CObjectDataMgr2D()
{
    // Loop through the map and delete all objects
    for( auto iter = m_pObjectDataMapMap.begin();
         iter != m_pObjectDataMapMap.end();
         ++iter)
    {
        NDelFunc::DeleteMapPointers(iter->second);
    }

}   // Destructer


/************************************************************************
*    desc:  Load all of the meshes and materials of a specific data group
*
*    param: string & group - specified group of meshes and materials to load
************************************************************************/
void CObjectDataMgr2D::LoadGroup( const std::string & group )
{
    // Make sure the group we are looking has been defined in the list table file
    auto listTableIter = m_listTableMap.find( group );
    if( listTableIter == m_listTableMap.end() )
        throw NExcept::CCriticalException("Obj Data List 2D Load Group Data Error!",
            boost::str( boost::format("Object data list group name can't be found (%s).\n\n%s\nLine: %s") 
                % group % __FUNCTION__ % __LINE__ ));

    // Load the group data if it doesn't already exist
    if( m_pObjectDataMapMap.find( group ) == m_pObjectDataMapMap.end() )
    {
        // Create a new group map inside of our map
        m_pObjectDataMapMap.insert( std::make_pair(group, std::map<const std::string, CObjectData2D *>()) );

        for( size_t i = 0; i < listTableIter->second.size(); ++i )
            LoadFromXML( group, listTableIter->second[i] );
    }
    else
    {
        throw NExcept::CCriticalException("Obj Data List 2D load Error!",
            boost::str( boost::format("Object data list group has alread been loaded (%s).\n\n%s\nLine: %s")
                % group % __FUNCTION__ % __LINE__ ));
    }

}   // LoadGroup


/************************************************************************
*    desc:  Load all object information from an xml
*
*	 param:	string & filePath - file path of the object data list XML
************************************************************************/
void CObjectDataMgr2D::LoadFromXML( const std::string & group, const std::string & filePath )
{
    // this open and parse the XML file:
    const XMLNode mainNode = XMLNode::openFileHelper( filePath.c_str(), "objectDataList2D" );

    // Get an iterator to the group
    auto groupMapIter = m_pObjectDataMapMap.find( group );

    //////////////////////////////////////////////
    // Load the default data
    //////////////////////////////////////////////

    // Get the node to the default object data
    const XMLNode defaultNode = mainNode.getChildNode( "default" );

    // Class to hold a data list's default data.
    CObjectData2D defaultData;

    // If there's no default node then we just use the defaults set in the data classes
    if( !defaultNode.isEmpty() )
            defaultData.LoadFromNode( defaultNode, "", "" );


    //////////////////////////////////////////////
    // Load the object data
    //////////////////////////////////////////////

    XMLNode objectListNode = mainNode.getChildNode( "objectList" );

    for( int i = 0; i < objectListNode.nChildNode(); ++i )
    {
        // Get the object data node
        const XMLNode objectNode = objectListNode.getChildNode(i);

        // Get the object's name
        const std::string name = objectNode.getAttribute( "name" );

        // Allocate the object data to the map
        auto iter = groupMapIter->second.insert( std::make_pair(name, new CObjectData2D( defaultData )) );

        // Check for duplicate names
        if( !iter.second )
        {
            throw NExcept::CCriticalException("Object Data Load Group Error!",
                boost::str( boost::format("Duplicate object name (%s - %s).\n\n%s\nLine: %s") % name % group % __FUNCTION__ % __LINE__ ));
        }

        // Load in the object data
        iter.first->second->LoadFromNode( objectNode, group, name );

        // Create it from the data
        iter.first->second->CreateFromData( group );
    }

}   // LoadFromXML


/************************************************************************
*    desc:  Free all of the meshes and materials of a specific data group
*
*    param: string & group - specified group of meshes and materials to load
************************************************************************/
void CObjectDataMgr2D::FreeGroup( const std::string & group )
{
    // Make sure the group we are looking for exists
    auto listTableIter = m_listTableMap.find( group );
    if( listTableIter == m_listTableMap.end() )
        throw NExcept::CCriticalException("Obj Data List 2D Load Group Data Error!",
            boost::str( boost::format("Object data list group name can't be found (%s).\n\n%s\nLine: %s") % group % __FUNCTION__ % __LINE__ ));

    // See if this group is still loaded
    auto groupMapIter = m_pObjectDataMapMap.find( group );
    if( groupMapIter != m_pObjectDataMapMap.end() )
    {
        CTextureMgr::Instance().DeleteTextureGroupFor2D( group );
        CVertBufMgr::Instance().DeleteBufferGroupFor2D( group );

        // Unload the group data
        NDelFunc::DeleteMapPointers(groupMapIter->second);
        m_pObjectDataMapMap.erase( groupMapIter );
    }

}   // FreeGroup


/************************************************************************
*    desc:  Get a specific 2D object's data
*
*    param: string & group - the group the 2D object belongs to
*			string & name  - name of the 2D object
*
*	 ret:	CObjectData2D & - 2D object
************************************************************************/
const CObjectData2D & CObjectDataMgr2D::GetData( const std::string & group, const std::string & name ) const
{
    auto groupMapIter = m_pObjectDataMapMap.find( group );
    if( groupMapIter == m_pObjectDataMapMap.end() )
        throw NExcept::CCriticalException("Obj Data List 2D Get Data Error!",
            boost::str( boost::format("Object data list group can't be found (%s).\n\n%s\nLine: %s") % group % __FUNCTION__ % __LINE__ ));

    auto dataMapIter = groupMapIter->second.find( name );
    if( dataMapIter == groupMapIter->second.end() )
        throw NExcept::CCriticalException("Obj Data List 2D Get Data Error!",
            boost::str( boost::format("Object data list name can't be found (%s).\n\n%s\nLine: (%s - %s)") % group % name % __FUNCTION__ % __LINE__ ));

    return *dataMapIter->second;

}   // GetData
