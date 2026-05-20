/************************************************************************
*    FILE NAME:       camera.cpp
*
*    DESCRIPTION:     Class that holds the camera position and rotation
************************************************************************/

// Physical component dependency
#include <common/camera.h>

// Game lib dependencies
#include <utilities/settings.h>
#include <utilities/xmlParser.h>

// Standard lib dependencies
#include <cstring>
#include <cstdlib>
#include <cmath>

/************************************************************************
*    DESC:  Constructor
************************************************************************/
CCamera::CCamera() :
    m_projType(NDefs::EPT_PERSPECTIVE),
    m_angle(CSettings::Instance().getViewAngle()),
    m_minZDist(CSettings::Instance().getMinZdist()),
    m_maxZDist(CSettings::Instance().getMaxZdist()),
    m_projWidth(CSettings::Instance().getDefaultSize().w),
    m_projHeight(CSettings::Instance().getDefaultSize().h),
    m_cullType(CULL_NULL)
{
    init();
}

CCamera::CCamera( const XMLNode & node ) :
    m_projType(NDefs::EPT_PERSPECTIVE),
    m_angle(CSettings::Instance().getViewAngle()),
    m_minZDist(CSettings::Instance().getMinZdist()),
    m_maxZDist(CSettings::Instance().getMaxZdist()),
    m_projWidth(CSettings::Instance().getDefaultSize().w),
    m_projHeight(CSettings::Instance().getDefaultSize().h),
    m_cullType(CULL_NULL)
{
    loadFromNode( node );
    init();
}

CCamera::CCamera( float minZDist, float maxZDist ) :
    m_projType(NDefs::EPT_ORTHOGRAPHIC),
    m_angle(0),
    m_minZDist(minZDist),
    m_maxZDist(maxZDist),
    m_projWidth(CSettings::Instance().getDefaultSize().w),
    m_projHeight(CSettings::Instance().getDefaultSize().h),
    m_cullType(CULL_NULL)
{
    init();
}

CCamera::CCamera( float angle, float minZDist, float maxZDist ) :
    m_projType(NDefs::EPT_PERSPECTIVE),
    m_angle(angle),
    m_minZDist(minZDist),
    m_maxZDist(maxZDist),
    m_projWidth(CSettings::Instance().getDefaultSize().w),
    m_projHeight(CSettings::Instance().getDefaultSize().h),
    m_cullType(CULL_NULL)
{
    init();
}

/************************************************************************
*    DESC:  destructor
************************************************************************/
CCamera::~CCamera()
{
}

/************************************************************************
*    DESC:  Load the camera data from an XML node
************************************************************************/
void CCamera::loadFromNode( const XMLNode & node )
{
    if( node.isAttributeSet("minZDist") )
        m_minZDist = std::atof(node.getAttribute("minZDist"));

    if( node.isAttributeSet("maxZDist") )
        m_maxZDist = std::atof(node.getAttribute("maxZDist"));

    if( node.isAttributeSet("view_angle") )
        m_angle = std::atof(node.getAttribute("view_angle")) * defs_DEG_TO_RAD;

    if( node.isAttributeSet("projWidth") )
        m_projWidth = std::atof(node.getAttribute("projWidth"));

    if( node.isAttributeSet("projHeight") )
        m_projHeight = std::atof(node.getAttribute("projHeight"));

    if( node.isAttributeSet("projectType") )
    {
        if( std::strcmp( node.getAttribute("projectType"), "orthographic" ) == 0 )
            m_projType = NDefs::EPT_ORTHOGRAPHIC;

        else if( std::strcmp( node.getAttribute("projectType"), "perspective" ) == 0 )
            m_projType = NDefs::EPT_PERSPECTIVE;
    }

    if( node.isAttributeSet("cull") )
    {
        std::string cullStr = node.getAttribute("cull");
        if( cullStr == "CULL_FULL" )
            m_cullType = CULL_FULL;
        else if( cullStr == "cull_x_only" )
            m_cullType = CULL_X_ONLY;
        else if( cullStr == "cull_y_only" )
            m_cullType = CULL_Y_ONLY;
    }

    // Load the transform data
    loadTransFromNode( node );

    if( m_parameters.IsSet( TRANSFORM ) )
        invertPos();
}

/************************************************************************
*    DESC:  Init the camera
************************************************************************/
void CCamera::init( NDefs::EProjectionType projType, float angle, float minZDist, float maxZDist )
{
    m_projType = projType;
    m_angle = angle;
    m_minZDist = minZDist;
    m_maxZDist = maxZDist;
    
    init();
}

void CCamera::init()
{
    // Create the projection matrix
    createProjectionMatrix();
    
    // Do the initial transform
    CObject::transform();
    
    // Calculate the final matrix
    calcFinalMatrix();
}

/************************************************************************
*    DESC:  Create the projection matrix
************************************************************************/
void CCamera::createProjectionMatrix()
{
    if( m_projType == NDefs::EPT_PERSPECTIVE )
    {
        float aspectRatio = CSettings::Instance().getSize().w /
                            CSettings::Instance().getSize().h;

        m_projectionMatrix.perspectiveFovRH(
            m_angle,
            aspectRatio,
            m_minZDist,
            m_maxZDist );
    }
    else
    {
        m_projectionMatrix.orthographicRH(
            m_projWidth,
            m_projHeight,
            m_minZDist,
            m_maxZDist );
    }
}

/************************************************************************
*    DESC:  Get the projected matrix
************************************************************************/  
const CMatrix & CCamera::getProjectionMatrix() const
{
    return m_projectionMatrix;
}

/************************************************************************
*    DESC:  Set the position but invert for camera perspective
************************************************************************/  
void CCamera::setPos( const CPoint<float> & position )
{
    CObject::setPos( -position );
}

void CCamera::setPos( float x, float y, float z )
{
    CObject::setPos( -x, -y, -z );
}

/************************************************************************
*    DESC:  Increment the position but invert for camera perspective
************************************************************************/  
void CCamera::incPos( const CPoint<float> & position )
{
    CObject::incPos( -position );
}

void CCamera::incPos( float x, float y, float z )
{
    CObject::incPos( -x, -y, -z );
}

/************************************************************************
*    DESC:  Transform
************************************************************************/
void CCamera::transform()
{
    const bool wasTransformed( m_parameters.IsSet( TRANSFORM ) );
    
    CObject::transform();
    
    if( wasTransformed )
        calcFinalMatrix();
}

/************************************************************************
*    DESC:  Calculate the final matrix
************************************************************************/
void CCamera::calcFinalMatrix()
{
    m_finalMatrix.initilizeMatrix();
    m_finalMatrix.mergeMatrix( m_matrix );
    m_finalMatrix.mergeMatrix( m_projectionMatrix );
}

/************************************************************************
*    DESC:  Get the final matrix
************************************************************************/  
const CMatrix & CCamera::getFinalMatrix() const
{
    return m_finalMatrix;
}

/************************************************************************
*    DESC:  Convert to orthographic screen coordinates
************************************************************************/  
CPoint<float> CCamera::toOrthoCoord( const CPoint<float> & position )
{
    CPoint<float> pos;

    const CSize<float> & ratio = CSettings::Instance().getScreenRatio();
    const CSize<float> & sizeHalf = CSettings::Instance().getSizeHalf();

    pos.x = (position.x - sizeHalf.w) / (ratio.w * m_scale.x);
    pos.y = (position.y - sizeHalf.h) / (ratio.h * m_scale.y);

    return pos;
}

/************************************************************************
*    DESC:  Apply the rotation
************************************************************************/
void CCamera::applyRotation( CMatrix & matrix )
{
    m_rotMatrix.initilizeMatrix();
    m_rotMatrix.rotate( m_rot );

    // Since the rotation has already been done, multiply it into the matrix
    matrix.multiply3x3( m_rotMatrix );
}

/************************************************************************
*    DESC:  Get the rotation matrix
************************************************************************/
const CMatrix & CCamera::getRotMatrix() const
{
    return m_rotMatrix;
}

/************************************************************************
*    DESC:  Get the cull type
************************************************************************/
CCamera::ECullType CCamera::getCullType() const
{
    return m_cullType;
}

/************************************************************************
*    DESC:  Check if the radius is in the view frustum
************************************************************************/
bool CCamera::inView( const CPoint<float> & transPos, float radius )
{
    const CSize<float> & defSizeHalf = CSettings::Instance().getDefaultSizeHalf();

    if( m_projType == NDefs::EPT_ORTHOGRAPHIC )
    {
        // Check the right and left sides of the screen
        if( std::abs(-getTransPos().x - (m_scale.x * transPos.x)) > (defSizeHalf.w + (m_scale.x * radius)) )
            return false;

        // Check the top and bottom sides of the screen
        if( std::abs(-getTransPos().y - (m_scale.y * transPos.y)) > (defSizeHalf.h + (m_scale.y * radius)) )
            return false;
    }
    else
    {
        float aspectW = CSettings::Instance().getSize().w / CSettings::Instance().getSize().h;
        float aspectH = 1.0f;

        // Check the right and left sides of the screen
        if( std::abs(-getTransPos().x - (m_scale.x * transPos.x)) > ((std::abs(transPos.z) * aspectW) + (m_scale.x * radius)) )
            return false;

        // Check the top and bottom sides of the screen
        if( std::abs(-getTransPos().y - (m_scale.y * transPos.y)) > ((std::abs(transPos.z) * aspectH) + (m_scale.y * radius)) )
            return false;
    }

    return true;
}

/************************************************************************
*    DESC:  Check if the radius is in the view frustum of the Y
************************************************************************/
bool CCamera::inViewY( const CPoint<float> & transPos, float radius )
{
    const CSize<float> & defSizeHalf = CSettings::Instance().getDefaultSizeHalf();

    if( m_projType == NDefs::EPT_ORTHOGRAPHIC )
    {
        if( std::abs(-getTransPos().y - (m_scale.y * transPos.y)) > (defSizeHalf.h + (m_scale.y * radius)) )
            return false;
    }
    else
    {
        if( std::abs(-getTransPos().y - (m_scale.y * transPos.y)) > ((std::abs(transPos.z)) + (m_scale.y * radius)) )
            return false;
    }

    return true;
}

/************************************************************************
*    DESC:  Check if the radius is in the view frustum of the X
************************************************************************/
bool CCamera::inViewX( const CPoint<float> & transPos, float radius )
{
    const CSize<float> & defSizeHalf = CSettings::Instance().getDefaultSizeHalf();

    if( m_projType == NDefs::EPT_ORTHOGRAPHIC )
    {
        if( std::abs(-getTransPos().x - (m_scale.x * transPos.x)) > (defSizeHalf.w + (m_scale.x * radius)) )
            return false;
    }
    else
    {
        float aspectW = CSettings::Instance().getSize().w / CSettings::Instance().getSize().h;

        if( std::abs(-getTransPos().x - (m_scale.x * transPos.x)) > ((std::abs(transPos.z) * aspectW) + (m_scale.x * radius)) )
            return false;
    }

    return true;
}
