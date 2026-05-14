/************************************************************************
*    FILE NAME:       object.h
*
*    DESCRIPTION:     object class
************************************************************************/

#ifndef __object_h__
#define __object_h__

// Game lib dependencies
#include <common/point.h>
#include <common/worldvalue.h>
#include <common/matrix.h>
#include <common/bitmask.h>

// Forward declaration(s)
class CMatrix;
struct XMLNode;

class CObject
{
public:
    
    CObject();
    virtual ~CObject();

    // Load the transform data from node
    void LoadTransFromNode( const XMLNode & node );

    // Set-Get the object's position
    void SetPos( const CPoint<CWorldValue> & position );
    const CPoint<CWorldValue> & GetPos() const;

    // Increment the object's position
    void IncPos( const CPoint<CWorldValue> & position );

    // Set-Get the object's rotation
    void SetRot( const CPoint<float> & rotation );
    const CPoint<float> & GetRot() const;

    // Increment the object's rotation
    void IncRot( const CPoint<float> & rotation );

    // Set the object's scale
    void SetScale( float scale );
    void SetScale( const CPoint<float> & scale );

    // Increment the object's scale
    void IncScale( float scale );
    void IncScale( const CPoint<float> & scale );

    // Get the object's scale
    const CPoint<float> & GetScale() const;

    // Set the object's visibility
    void SetVisible( bool value = true );

    // Is the object visible
    bool IsVisible() const;

    // Transform - One call for those objects that don't have parents
    void Transform();
    void Transform( const CMatrix & matrix, bool tranformWorldPos = false );

    // Get the object's matrix
    const CMatrix & GetMatrix() const;

    // Was the world position transformed?
    bool WasWorldPosTranformed() const;

    // Force the world transform
    void ForceWorldTransform();

private:

    // Transform the object in local space
    void TransformLocal();

protected:

    // Translated position
    CPoint<CWorldValue> m_trans_pos;

    // Visibility flag
    bool m_visible;

    // translated matrix class
    CMatrix m_trans_matrix;

private:

    // Bitmask letting us know if the object needs
    // to be transformed and/or projected
    CBitmask m_parameters;

    // local matrix
    CMatrix m_matrix;

    // Local position
    CPoint<CWorldValue> m_pos;

    // Local Rotation
    CPoint<float> m_rot;

    // Local scale
    CPoint<float> m_scale;

    // Flag to indicate the need to transform the world position
    bool m_tranformWorldPos;
};

#endif  // __object_2d_h__
