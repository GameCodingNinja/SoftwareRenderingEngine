
/************************************************************************
*    FILE NAME:       cameramanager.cpp
*
*    DESCRIPTION:     camera manager class singleton
************************************************************************/

// Physical component dependency
#include <managers/cameramanager.h>

// Game lib dependencies
#include <utilities/genfunc.h>
#include <utilities/xmlParser.h>

// Standard lib dependencies
#include <algorithm>

/************************************************************************
*    DESC:  Constructor
************************************************************************/
CCameraMgr::CCameraMgr()
{
    m_defCamera.setPos( 0, 0, 100 );
    m_defCamera.transform();
}


/************************************************************************
*    DESC:  Destructor
************************************************************************/
CCameraMgr::~CCameraMgr()
{
}


/************************************************************************
*    DESC:  Load the camera data
************************************************************************/
void CCameraMgr::load( const std::string & filePath )
{
    // Open and parse the XML file:
    const XMLNode node = XMLNode::openFileHelper(filePath.c_str(), "cameraList");

    if( !node.isEmpty() )
    {
        const XMLNode defaultNode = node.getChildNode("default");
        if( !defaultNode.isEmpty() )
        {
            m_defCamera.loadFromNode( defaultNode );
            m_defCamera.init();
        }

        for( int i = 0; i < node.nChildNode( "camera" ); ++i )
        {
            XMLNode cameraNode = node.getChildNode( "camera", i );

            // Get the id of the camera
            const std::string id = cameraNode.getAttribute( "id" );

            auto iter = m_cameraMap.find( id );
            if( iter == m_cameraMap.end() )
                m_cameraMap.emplace( id, cameraNode );
            else
                NGenFunc::PostDebugMsg( NGenFunc::FormatString("Camera id already exists (%s - %s).", id.c_str(), filePath.c_str()) );
        }
    }
}


/************************************************************************
*    DESC:  Get the default camera
************************************************************************/
CCamera & CCameraMgr::getDefault()
{
    return m_defCamera;
}


/************************************************************************
*    DESC:  Get the camera
************************************************************************/
CCamera & CCameraMgr::get( const std::string & id )
{
    auto iter = m_cameraMap.find( id );
    if( iter == m_cameraMap.end() )
    {
        NGenFunc::PostDebugMsg( NGenFunc::FormatString("Camera id does not exist (%s). Using default camera instead.", id.c_str()) );
        return m_defCamera;
    }
    
    iter->second.transform();
    return iter->second;
}


/************************************************************************
*    DESC:  Add camera to transform list
************************************************************************/

void CCameraMgr::addToTransListVec( const std::vector<std::string> & cameraIdVec )
{
    for( auto & iter : cameraIdVec )
        addToTransList( iter );
}

void CCameraMgr::addToTransList( const std::string & id )
{
    auto mapIter = m_cameraMap.find( id );
    if( mapIter != m_cameraMap.end() )
    {
        // See if the camera pointer is already in the vector
        auto vecIter = std::find( m_transVec.begin(), m_transVec.end(), &mapIter->second );
        if( vecIter == m_transVec.end() )
            m_transVec.push_back( &mapIter->second );
        else
            NGenFunc::PostDebugMsg( NGenFunc::FormatString("Camera is already in transform vector (%s)!", id.c_str()) );
    }
    else
        NGenFunc::PostDebugMsg( NGenFunc::FormatString("Camera id does not exist to add to transform list (%s).", id.c_str()) );
}


/************************************************************************
*    DESC:  Remove camera from transform list
************************************************************************/
void CCameraMgr::removeFromTransList( const std::string & id )
{
    auto mapIter = m_cameraMap.find( id );
    if( mapIter != m_cameraMap.end() )
    {
        // See if the camera pointer is already in the vector
        auto vecIter = std::find( m_transVec.begin(), m_transVec.end(), &mapIter->second );
        if( vecIter != m_transVec.end() )
            m_transVec.erase( vecIter );
        else
            NGenFunc::PostDebugMsg( NGenFunc::FormatString("Camera is not in transform vector (%s)!", id.c_str()) );
    }
    else
        NGenFunc::PostDebugMsg( NGenFunc::FormatString("Camera id does not exist to remove from transform list (%s).", id.c_str()) );
}


/************************************************************************
*    DESC:  Transform the active camera's in the list
************************************************************************/
void CCameraMgr::transform()
{
    for( auto iter : m_transVec )
        iter->transform();
}


/************************************************************************
*    DESC:  Clear all data
************************************************************************/
void CCameraMgr::clear()
{
    m_transVec.clear();
    m_cameraMap.clear();
}


/************************************************************************
*    DESC:  Clear the transform list
************************************************************************/
void CCameraMgr::clearTransList()
{
    m_transVec.clear();
}


/************************************************************************
*    DESC:  Rebuild all camera's projection matrixes
************************************************************************/
void CCameraMgr::rebuildProjectionMatrix()
{
    m_defCamera.init();

    for( auto & iter : m_cameraMap )
        iter.second.init();
}
