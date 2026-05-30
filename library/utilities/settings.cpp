
/************************************************************************
*    FILE NAME:       settings.cpp
*
*    DESCRIPTION:     game settings class
************************************************************************/

// Physical component dependency
#include <utilities/settings.h>

// Standard lib dependencies
#include <string>
#include <cstring>
#include <cmath>

/************************************************************************
*    DESC:  Constructor
************************************************************************/
CSettings::CSettings() :
    m_size(1024,768),
    m_default_size(1024,768),
    m_nativeAspectRatio(1024.f/768.f),
    m_fullScreen(false),
    m_vSync(true),
    m_viewAngle(45.f),
    m_minZdist(5.f),
    m_maxZdist(1000.f),
    m_projectionType(NDefs::EPT_PERSPECTIVE)
{
}


/************************************************************************
*    DESC:  destructor
************************************************************************/
CSettings::~CSettings()
{
}


/************************************************************************
*    DESC:  Load settings data from xml file
************************************************************************/
void CSettings::loadFromXML( const std::string & filePath )
{
    m_filePath = filePath;

    // this open and parse the XML file:
    m_mainNode = XMLNode::openFileHelper( filePath.c_str(), "settings" );

    if( !m_mainNode.isEmpty() )
    {
        const XMLNode displayListNode = m_mainNode.getChildNode("display");
        if( !displayListNode.isEmpty() )
        {
            // Get the attributes from the "resolution" node
            const XMLNode resolutionNode = displayListNode.getChildNode("resolution");
            if( !resolutionNode.isEmpty() )
            {
                m_size.w = std::atoi(resolutionNode.getAttribute("width"));
                m_size.h = std::atoi(resolutionNode.getAttribute("height"));

                if( resolutionNode.isAttributeSet("fullscreen") )
                    m_fullScreen = ( std::strcmp( resolutionNode.getAttribute("fullscreen"), "true" ) == 0 );
            }

            // Get the attributes from the "defaultHeight" node
            const XMLNode defResNode = displayListNode.getChildNode("defaultHeight");
            if( !defResNode.isEmpty() )
            {
                m_default_size.h = std::atof(defResNode.getAttribute("height"));
            }
        }

        // Set the native aspect ratio once from the startup resolution
        m_nativeAspectRatio = m_size.w / m_size.h;

        calcRatio();

        const XMLNode deviceNode = m_mainNode.getChildNode("device");
        if( !deviceNode.isEmpty() )
        {
            // Get the projection info
            const XMLNode projectionNode = deviceNode.getChildNode("projection");
            if( !projectionNode.isEmpty() )
            {
                if( projectionNode.isAttributeSet("minZDist") )
                    m_minZdist = std::atof(projectionNode.getAttribute("minZDist"));

                if( projectionNode.isAttributeSet("maxZDist") )
                    m_maxZdist = std::atof(projectionNode.getAttribute("maxZDist"));

                if( projectionNode.isAttributeSet("view_angle") )
                    m_viewAngle = std::atof(projectionNode.getAttribute("view_angle"));

                if( projectionNode.isAttributeSet("projectType") &&
                    std::strcmp( projectionNode.getAttribute("projectType"), "orthographic" ) == 0 )
                    m_projectionType = NDefs::EPT_ORTHOGRAPHIC;
            }

            // Convert to radians
            m_viewAngle *= (float)defs_DEG_TO_RAD;

            // Get the VSync setting from the backbuffer node
            const XMLNode backBufferNode = deviceNode.getChildNode("backbuffer");
            if( !backBufferNode.isEmpty() )
            {
                if( backBufferNode.isAttributeSet("VSync") )
                    m_vSync = ( std::strcmp( backBufferNode.getAttribute("VSync"), "true" ) == 0 );
            }
        }
    }
}


/************************************************************************
*    DESC:  Calculate the ratios
************************************************************************/
void CSettings::calcRatio()
{
    // Height and width screen ratio for perspective projection
    m_screenAspectRatio.w = m_size.w / m_size.h;
    m_screenAspectRatio.h = m_size.h / m_size.w;

    // NOTE: The default width is based on the current aspect ratio
    m_default_size.w = (float)(int)std::ceil((m_screenAspectRatio.w * m_default_size.h) + 0.5);

    // Get half the size for use with screen boundaries
    m_default_size_half = m_default_size / 2.f;

    // Screen size divided by two
    m_size_half = m_size / 2.f;

    // Pre-calculate the aspect ratios for orthographic projection
    m_orthoAspectRatio.h = m_size.h / m_default_size.h;
    m_orthoAspectRatio.w = m_size.w / m_default_size.w;

    // Ratio for devices (mice) to correctly calculate movement
    // between the default height and actual height
    m_deviceRatio = m_default_size.h / m_size.h;
}


/************************************************************************
*    DESC:  Get/Set game window size
************************************************************************/
const CSize<float> & CSettings::getSize() const
{
    return m_size;
}

void CSettings::setSize( const CSize<float> & size )
{
    m_size = size;
}

/************************************************************************
*    DESC:  Get game window size / 2
************************************************************************/
const CSize<float> & CSettings::getSizeHalf() const
{
    return m_size_half;
}

/************************************************************************
*    DESC:  Get the view angle
************************************************************************/
float CSettings::getViewAngle() const
{
    return m_viewAngle;
}

/************************************************************************
*    DESC:  Get the minimum z distance
************************************************************************/
float CSettings::getMinZdist() const
{
    return m_minZdist;
}

/************************************************************************
*    DESC:  Get the maximum z distance
************************************************************************/
float CSettings::getMaxZdist() const
{
    return m_maxZdist;
}

/************************************************************************
*    DESC:  Get the default size
************************************************************************/
const CSize<float> & CSettings::getDefaultSize() const
{
    return m_default_size;
}

/************************************************************************
*    DESC:  Get the default size in half
************************************************************************/
const CSize<float> & CSettings::getDefaultSizeHalf() const
{
    return m_default_size_half;
}

/************************************************************************
*    DESC:  Height and width screen ratio for orthographic objects
*           The difference between screen and the default size
************************************************************************/
const CSize<float> & CSettings::getOrthoAspectRatio() const
{
    return m_orthoAspectRatio;
}

/************************************************************************
*    DESC:  Height and width screen ratio for perspective projection
************************************************************************/
const CSize<float> & CSettings::getScreenAspectRatio() const
{
    return m_screenAspectRatio;
}

/************************************************************************
*    DESC:  Get the screen ratio (ortho aspect ratio - legacy alias)
************************************************************************/
const CSize<float> & CSettings::getScreenRatio() const
{
    return m_orthoAspectRatio;
}

/************************************************************************
*    DESC:  Get the device ratio
************************************************************************/
float CSettings::getDeviceRatio() const
{
    return m_deviceRatio;
}

/************************************************************************
*    DESC:  Get the native aspect ratio (fixed at startup)
************************************************************************/
float CSettings::getNativeAspectRatio() const
{
    return m_nativeAspectRatio;
}

/************************************************************************
*    DESC:  Get/Set VSync
************************************************************************/
bool CSettings::getVSync() const
{
    return m_vSync;
}

void CSettings::setVSync( bool value )
{
    m_vSync = value;
}

/************************************************************************
*    DESC:  Get/Set full screen
************************************************************************/
bool CSettings::getFullScreen() const
{
    return m_fullScreen;
}

void CSettings::setFullScreen( bool value )
{
    m_fullScreen = value;
}

/************************************************************************
*    DESC:  Get the projection type
************************************************************************/
NDefs::EProjectionType CSettings::getProjectionType() const
{
    return m_projectionType;
}

/************************************************************************
*    DESC:  Save the settings file
************************************************************************/
void CSettings::saveSettings()
{
    if( !m_mainNode.isEmpty() )
    {
        XMLNode displayListNode = m_mainNode.getChildNode("display");

        if( !displayListNode.isEmpty() )
        {
            // Get the attributes from the "resolution" node
            XMLNode resolutionNode = displayListNode.getChildNode("resolution");

            {
                std::string tmpStr = std::to_string(static_cast<int>(m_size.w));
                resolutionNode.updateAttribute(tmpStr.c_str(), "width", "width");

                tmpStr = std::to_string(static_cast<int>(m_size.h));
                resolutionNode.updateAttribute(tmpStr.c_str(), "height", "height");
            }

            {
                std::string tmpStr = m_fullScreen ? "true" : "false";
                resolutionNode.updateAttribute(tmpStr.c_str(), "fullscreen", "fullscreen");
            }
        }

        XMLNode deviceNode = m_mainNode.getChildNode("device");
        if( !deviceNode.isEmpty() )
        {
            XMLNode backBufferNode = deviceNode.getChildNode("backbuffer");
            if( !backBufferNode.isEmpty() )
            {
                std::string tmpStr = m_vSync ? "true" : "false";
                backBufferNode.updateAttribute(tmpStr.c_str(), "VSync", "VSync");
            }
        }

        // Save the settings file
        m_mainNode.writeToFile(m_filePath.c_str(), "utf-8");
    }
}
