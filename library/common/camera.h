/************************************************************************
*    FILE NAME:       camera.h
*
*    DESCRIPTION:     class that holds the camera position and rotation
************************************************************************/

#pragma once

// Physical component dependency
#include <common/object.h>

// Game lib dependencies
#include <common/matrix.h>
#include <common/defs.h>

// Forward Declarations
struct XMLNode;

class CCamera : public CObject
{
public:

    enum ECullType
    {
        CULL_NULL,
        CULL_FULL,
        CULL_X_ONLY,
        CULL_Y_ONLY
    };

    // Constructor
    CCamera();
    CCamera( const XMLNode & node );
    CCamera( float minZDist, float maxZDist );
    CCamera( float angle, float minZDist, float maxZDist );

    // Destructor
    virtual ~CCamera();

    // Load the camera data from an XML node
    void loadFromNode( const XMLNode & node );

    // Init the camera
    void init( NDefs::EProjectionType projType, float angle, float minZDist, float maxZDist );
    void init();

    // Create the projection matrix
    void createProjectionMatrix();

    // Set/Inc the position but invert for camera perspective
    void setPos( const CPoint<float> & position );
    void setPos( float x = 0, float y = 0, float z = 0 );

    void incPos( const CPoint<float> & position );
    void incPos( float x = 0, float y = 0, float z = 0 );

    // Get the projected matrix
    const CMatrix & getProjectionMatrix() const;

    // Transform - One call for those objects that don't have parents
    void transform() final;

    // Get the final matrix
    const CMatrix & getFinalMatrix() const;

    // Convert to orthographic screen coordinates
    CPoint<float> toOrthoCoord( const CPoint<float> & position );

    // Get the rotation matrix
    const CMatrix & getRotMatrix() const final;

    //  Get the cull type
    ECullType getCullType() const;

    // Check if the radius is in the view frustum
    bool inView( const CPoint<float> & transPos, float radius );
    bool inViewY( const CPoint<float> & transPos, float radius );
    bool inViewX( const CPoint<float> & transPos, float radius );

protected:
    
    // Apply the rotation
    void applyRotation( CMatrix & matrix ) final;

private:

    // Calculate the final matrix
    void calcFinalMatrix();

private:

    // Custom projection matrix
    CMatrix m_projectionMatrix;

    // Final combined matrix (view * projection)
    CMatrix m_finalMatrix;

    // Matrix for rotations only
    // Basically used for normal calculations
    CMatrix m_rotMatrix;

    // The projection type
    NDefs::EProjectionType m_projType;

    // Projection settings
    float m_angle;
    float m_minZDist;
    float m_maxZDist;

    // Custom orthographic projection dimensions
    // Allows per-camera viewport size (e.g. minimap)
    // Defaults to the global settings default size
    float m_projWidth;
    float m_projHeight;

    // Cull type
    ECullType m_cullType;
};
