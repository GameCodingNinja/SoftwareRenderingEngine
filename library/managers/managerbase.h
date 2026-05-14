
/************************************************************************
*    FILE NAME:       managerbase.h
*
*    DESCRIPTION:     Base class for common manager behaviors
************************************************************************/

#ifndef __manager_base_h__
#define __manager_base_h__

// Standard lib dependencies
#include <string>
#include <vector>
#include <map>

// Forward declaration(s)
struct XMLNode;

class CManagerBase
{
public:

    // Load the data list table
    void LoadListTable( const std::string & filePath );

protected:

    CManagerBase();
    virtual ~CManagerBase();

    // Load custom data from node
    virtual void LoadFromNode( const XMLNode & node );

protected:

    // Holds the list table map
    std::map<const std::string, std::vector<std::string>> m_listTableMap;

};

#endif  // __manager_base_h__


