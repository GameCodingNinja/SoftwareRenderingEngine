
/************************************************************************
*    FILE NAME:       lightmanager.h
*
*    DESCRIPTION:     Light manager class singleton
************************************************************************/

#ifndef __light_manager_h__
#define __light_manager_h__

// Physical component dependency
#include <managers/managerbase.h>

// Standard lib dependencies
#include <string>
#include <map>
#include <vector>

// Game lib dependencies
#include <common/light.h>

class CLightMgr : public CManagerBase
{
public:

    // Get the instance of the singleton class
    static CLightMgr & Instance()
    {
        static CLightMgr lightMgr;
        return lightMgr;
    }

    // Load all lights for a specific group
    void LoadGroup( const std::string & group );

    // Free a group's lights
    void FreeGroup( const std::string & group );

    // Get the light vector for a group
    const std::vector<CLight> & get( const std::string & group ) const;

    // Clear all groups
    void clear();

private:

    // Constructor
    CLightMgr();

    // Destructor
    ~CLightMgr();

    // Load lights from an XML data file
    void LoadFromXML( const std::string & group, const std::string & filePath );

private:

    // Map of named light groups
    std::map<const std::string, std::vector<CLight>> m_lightVecMap;
};

#endif
