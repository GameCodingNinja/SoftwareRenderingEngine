/************************************************************************
*    FILE NAME:       worldcamera.h
*
*    DESCRIPTION:     Singleton class that holds the world camera
*					  position and rotation
************************************************************************/

#ifndef __world_camera_h__
#define __world_camera_h__

// Physical component dependency
#include <common/object.h>

// Game lib dependencies
#include <common/worldvalue.h>
#include <common/point.h>
#include <common/matrix.h>

class CWorldCamera : public CObject
{
public:

    // Get the instance of the singleton class
    static CWorldCamera & Instance()
    {
        static CWorldCamera camera;
        return camera;
    }

    // Set-Get-Inc the camera's world position
    void SetPos( const CPoint<CWorldValue> & pos );
    void IncPos( const CPoint<CWorldValue> & pos );

protected:

    CWorldCamera();
    ~CWorldCamera();

};

#endif  // __world_camera_h__
