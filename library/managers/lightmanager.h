
/************************************************************************
*    FILE NAME:       lightmanager.h
*
*    DESCRIPTION:     Light manager class singleton
************************************************************************/

#ifndef __light_manager_h__
#define __light_manager_h__

// Standard lib dependencies
#include <string>
#include <map>
#include <vector>

// Game lib dependencies
#include <common/light.h>

class CLightMgr
{
public:

    // Get the instance of the singleton class
    static CLightMgr & Instance()
    {
        static CLightMgr lightMgr;
        return lightMgr;
    }

    // Load the list table (maps group names to data files)
    void loadListTable( const std::string & filePath );

    // Load a group's lights from a data file
    void loadFromFile( const std::string & group, const std::string & filePath );

    // Add a light to a named group
    void add( const std::string & group, const CLight & light );

    // Get the light vector for a group
    const std::vector<CLight> & get( const std::string & group ) const;

    // Delete a light group
    void deleteGroup( const std::string & group );

    // Clear all groups
    void clear();

private:

    // Constructor
    CLightMgr();

    // Destructor
    ~CLightMgr();

private:

    // Map of named light groups
    std::map<const std::string, std::vector<CLight>> m_lightVecMap;
};

#endif
