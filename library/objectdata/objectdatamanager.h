/************************************************************************
*    FILE NAME:       objectdatamanager.h
*
*    DESCRIPTION:     Class that holds a map of all object data used 
*					  for later loading
************************************************************************/

#ifndef __object_data_list_h__
#define __object_data_list_h__

// Physical component dependency
#include <managers/managerbase.h>

// Forward declaration(s)
class iObjectData;

class CObjectDataMgr : public CManagerBase
{
public:

    // Get the instance of the singleton class
    static CObjectDataMgr & Instance()
    {
        static CObjectDataMgr objectData;
        return objectData;
    }

    // Get a specific object's data
    const iObjectData & getData( const std::string & group, const std::string & name ) const;

    // Load all of the meshes and materials of a specific data group
    void LoadGroup( const std::string & group );

    // Free all of the meshes and materials of a specific data group
    void FreeGroup( const std::string & group );

private:

    CObjectDataMgr();
    virtual ~CObjectDataMgr();

    // Load all object information from an xml
    void LoadFromXML( const std::string & group, const std::string & filePath );

    // Load the 2D object data from node
    void Load2D( const std::string & group, const XMLNode & mainNode );

    // Load the 3D object data from node
    void Load3D( const std::string & group, const XMLNode & mainNode );

private:

    // Map in a map of all the objects' data
    std::map<const std::string, std::map<const std::string, iObjectData *> > m_pObjectDataMapMap;

};

#endif
