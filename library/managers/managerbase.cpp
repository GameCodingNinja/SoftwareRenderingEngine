
/************************************************************************
*    FILE NAME:       managerbase.cpp
*
*    DESCRIPTION:     Base class for common manager behaviors
************************************************************************/

// Physical component dependency
#include <managers/managerbase.h>

// Game lib dependencies
#include <utilities/xmlParser.h>
#include <utilities/exceptionhandling.h>
#include <utilities/genfunc.h>

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
    // this open and parse the XML file:
    const XMLNode mainNode = XMLNode::openFileHelper( filePath.c_str(), "listTable" );

    for( int i = 0; i < mainNode.nChildNode(); ++i )
    {
        const XMLNode dataListNode = mainNode.getChildNode( "groupList", i );

        const std::string group = dataListNode.getAttribute( "groupName" );

        auto iter = m_listTableMap.insert( std::make_pair( group, std::vector<std::string>() ) );

        // Check for duplicate group names
        if( !iter.second )
        {
            throw NExcept::CCriticalException("List Table Load Error!",
                NGenFunc::FormatString("Duplicate group name (%s).\n\n%s\nLine: %d", group, __FUNCTION__, __LINE__));
        }

        for( int j = 0; j < dataListNode.nChildNode(); ++j )
        {
            const XMLNode dataNode = dataListNode.getChildNode( "file", j );
            iter.first->second.push_back( dataNode.getAttribute( "path" ) );
        }
    }

}   // LoadListTable
