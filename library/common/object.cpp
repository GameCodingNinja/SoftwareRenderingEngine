/************************************************************************
*    FILE NAME:       object.cpp
*
*    DESCRIPTION:     object class
************************************************************************/

// Physical component dependency
#include <common/object.h>

// Game lib dependencies
#include <utilities/xmlparsehelper.h>

/************************************************************************
*    desc:  Constructer                                                             
************************************************************************/
CObject::CObject()
    : m_visible(true),
      m_tranformWorldPos(false)
{
}   // Constructer


/************************************************************************
*    desc:  Destructer                                                             
************************************************************************/
CObject::~CObject()
{
}   // Destructer


/************************************************************************
*    desc:  Load the transform data from node
************************************************************************/
void CObject::LoadTransFromNode( const XMLNode & node )
{
    bool loadedFlag;

    CPoint<CWorldValue> pos = NParseHelper::LoadPosition( node, loadedFlag );
    if( loadedFlag )
        SetPos( pos );

    CPoint<float> rot = NParseHelper::LoadRotation( node, loadedFlag );
    if( loadedFlag )
        SetRot( rot );

    CPoint<float> scale = NParseHelper::LoadScale( node, loadedFlag );
    if( loadedFlag )
        SetScale( scale );

}   // LoadTransFromNode


/************************************************************************
*    desc:  Get the object's world position
*  
*    ret:	const CWorldPoint & - object's world position
************************************************************************/
const CPoint<CWorldValue> & CObject::GetPos() const
{
    return m_pos;

}   // GetPos


/************************************************************************
*    desc:  Get the object's rotation
*  
*    ret:	const CPoint & - pre-translation rotation
************************************************************************/
const CPoint<float> & CObject::GetRot() const
{
    return m_rot;

}   // GetRot


/************************************************************************
*    desc:  Set the object's position
*  
*    param: const CPoint/CPointInt/CWorldPoint & position - pos to set
************************************************************************/
void CObject::SetPos( const CPoint<CWorldValue> & position )
{
    m_parameters.Add( NDefs::TRANSFORM_LOCAL );

    m_pos = position;

}   // SetPos


/************************************************************************
*    desc:  Inc the object's float position
*  
*    param: const CPoint/CPointInt/CWorldPoint & position - pos to inc
************************************************************************/
void CObject::IncPos( const CPoint<CWorldValue> & position )
{
    m_parameters.Add( NDefs::TRANSFORM_LOCAL );

    m_pos += position;

}   // IncPos


/************************************************************************
*    desc:  Set the pre-translation matrix
*  
*    param: const CPoint & rotation - rotation amount
************************************************************************/
void CObject::SetRot( const CPoint<float> & rotation )
{
    m_parameters.Add( NDefs::ROTATE | NDefs::TRANSFORM_LOCAL );

    m_rot = rotation;

}   // SetRot


/************************************************************************
*    desc:  Inc the pre-translation matrix
*  
*    param: const CPoint & rotation - rotation inc
************************************************************************/
void CObject::IncRot( const CPoint<float> & rotation )
{
    m_parameters.Add( NDefs::ROTATE | NDefs::TRANSFORM_LOCAL );

    m_rot += rotation;

    m_rot.Cap(360.0f);

}   // IncRot


/************************************************************************
*    desc:  Set the object's scale
*
*    param: float/CPoint _scale - scale to set
************************************************************************/
void CObject::SetScale( float scale )
{
    m_parameters.Add( NDefs::SCALE | NDefs::TRANSFORM_LOCAL );

    SetScale( CPoint<float>(scale,scale,scale) );

}   // SetScale

void CObject::SetScale( const CPoint<float> & scale )
{
    m_parameters.Add( NDefs::SCALE | NDefs::TRANSFORM_LOCAL );

    m_scale = scale;

}   // SetScale


/************************************************************************
*    desc:  Inc the object's scale
*
*    param: float/CPoint _scale - scale to inc
************************************************************************/
void CObject::IncScale( float scale )
{
    m_parameters.Add( NDefs::SCALE | NDefs::TRANSFORM_LOCAL );

    m_scale += scale;

}   // IncScale 

void CObject::IncScale( const CPoint<float> & scale )
{
    m_parameters.Add( NDefs::SCALE | NDefs::TRANSFORM_LOCAL );

    m_scale += scale;

}   // IncScale


/************************************************************************
*    desc:  Get the object's scale
*  
*    ret:	const CPoint & - sprite scale
************************************************************************/
const CPoint<float> & CObject::GetScale() const
{
    return m_scale;

}   // GetScale


/************************************************************************
*    desc:  Set the object visible
*  
*    param: bool value - value to set to
************************************************************************/
void CObject::SetVisible( bool value )
{
    m_visible = value;

}   // SetVisible


/************************************************************************
*    desc:  Is the object visible
*  
*    ret:	bool - visible flag
************************************************************************/
bool CObject::IsVisible() const
{
    return m_visible;

}   // IsVisible


/************************************************************************
*    desc:  Transform the object in local space
************************************************************************/
void CObject::TransformLocal()
{
    m_tranformWorldPos = false;

    if( m_parameters.IsSet( NDefs::TRANSFORM_LOCAL ) )
    {
        // Reset the matrices
        m_matrix.InitilizeMatrix();

        // Apply the scale
        if( m_parameters.IsSet( NDefs::SCALE ) )
            m_matrix.Scale( m_scale );

        // Apply the rotation
        if( m_parameters.IsSet( NDefs::ROTATE ) )
            m_matrix.Rotate( m_rot );

        m_matrix.Translate( m_pos );

        // Clear the check parameter
        m_parameters.Remove( NDefs::TRANSFORM_LOCAL );

        // Indicate that because of a change, we need to translate the object
        m_parameters.Add( NDefs::TRANSFORM_WORLD_POS );
    }

}   // TransformLocal


/************************************************************************
*    desc:  Transform
************************************************************************/
void CObject::Transform()
{
    // Transform the object in local space
    TransformLocal();

    // No matrix is required so just copy them over
    if( m_parameters.IsSet( NDefs::TRANSFORM_WORLD_POS ) )
    {
        m_trans_pos = m_pos;
        m_trans_matrix = m_matrix;

        m_tranformWorldPos = true;

        m_parameters.Remove( NDefs::TRANSFORM_WORLD_POS );
    }

}   // Transform

void CObject::Transform( const CMatrix & matrix, bool tranformWorldPos )
{
    // Transform the object in local space
    TransformLocal();

    // Translate based on passed in matrix
    if( m_parameters.IsSet( NDefs::TRANSFORM_WORLD_POS ) || tranformWorldPos )
    {
        m_trans_matrix = m_matrix * matrix;

        CPoint<float> trans_pos;
        m_trans_matrix.Transform( trans_pos, CPoint<float>() );
        m_trans_pos = trans_pos;

        m_tranformWorldPos = true;

        m_parameters.Remove( NDefs::TRANSFORM_WORLD_POS );
    }

}   // Transform


/************************************************************************
*    desc:  Get the object's matrix
************************************************************************/
const CMatrix & CObject::GetMatrix() const
{
    return m_trans_matrix;

}   // GetMatrix


/************************************************************************
*    desc:  Was the world position transformed?
************************************************************************/
bool CObject::WasWorldPosTranformed() const
{
    return m_tranformWorldPos;

}   // WasWorldPosTranformed


/************************************************************************
*    desc:  Force the world transform
************************************************************************/
void CObject::ForceWorldTransform()
{
    m_parameters.Add( NDefs::TRANSFORM_WORLD_POS );

}   // ForceWorldTransform
