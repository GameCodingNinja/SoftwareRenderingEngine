/************************************************************************
*    FILE NAME:       objectdatamanager2d.h
*
*    DESCRIPTION:     Class that holds a map of all 2D object data used 
*					  for later loading
************************************************************************/

#ifndef __object_data_list_2d_h__
#define __object_data_list_2d_h__

// Physical component dependency
#include <managers/managerbase.h>

// Forward declaration(s)
class CObjectData2D;

class CObjectDataMgr2D : public CManagerBase
{
public:

    // Get the instance of the singleton class
    static CObjectDataMgr2D & Instance()
    {
        static CObjectDataMgr2D objectData;
        return objectData;
    }

    // Get a specific stage object's data
    const CObjectData2D & GetData( const std::string & group, const std::string & name ) const;

    // Load all of the meshes and materials of a specific data group
    void LoadGroup( const std::string & group );

    // Free all of the meshes and materials of a specific data group
    void FreeGroup( const std::string & group );

private:

    CObjectDataMgr2D();
    virtual ~CObjectDataMgr2D();

    // Load all object information from an xml
    void LoadFromXML( const std::string & group, const std::string & filePath );

    // Load an object from the node
    void LoadObjectFromNode( const XMLNode & objectNode, 
                             CObjectData2D & objectData );

private:

    // Map in a map of all the objects' data
    std::map<const std::string, std::map<const std::string, CObjectData2D *> > m_pObjectDataMapMap;

};

#endif  // __object_data_list_2d_h__
