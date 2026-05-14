
/************************************************************************
*    FILE NAME:       managerbase.cpp
*
*    DESCRIPTION:     Base class for common manager behaviors
************************************************************************/

// Physical component dependency
#include <managers/managerbase.h>

// Game lib dependencies
#include <utilities/xmlParser.h>

/************************************************************************
*    desc:  Constructer
************************************************************************/
CManagerBase::CManagerBase()
{
}   // constructor


/************************************************************************
*    desc:  destructer                                                             
************************************************************************/
CManagerBase::~CManagerBase()
{
}   // destructer


/************************************************************************
*    desc:  Load the data list tables
*
*	 param:	string & filePath - file path of the object data list XML
************************************************************************/
void CManagerBase::LoadListTable( const std::string & filePath )
{
    if( m_listTableMap.empty() )
    {
        // this open and parse the XML file:
        const XMLNode mainNode = XMLNode::openFileHelper( filePath.c_str(), "listTable" );

        for( int i = 0; i < mainNode.nChildNode(); ++i )
        {
            const XMLNode dataListNode = mainNode.getChildNode( "groupList", i );

            auto mapIter = m_listTableMap.insert( std::make_pair( dataListNode.getAttribute( "groupName" ), 
                                                                                              std::vector<std::string>() ) ).first;

            for( int j = 0; j < dataListNode.nChildNode(); ++j )
            {
                const XMLNode dataNode = dataListNode.getChildNode( "file", j );
                mapIter->second.push_back( dataNode.getAttribute( "path" ) );
            }
        }

        // Load custom data from node
        LoadFromNode( mainNode );
    }

}   // LoadListTable


/************************************************************************
*    desc:  Load custom data from node
************************************************************************/
void CManagerBase::LoadFromNode( const XMLNode & node )
{
    // Base class does nothing
}   // LoadFromNode
