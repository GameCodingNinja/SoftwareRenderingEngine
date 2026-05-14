
/************************************************************************
*    FILE NAME:       settings.cpp
*
*    DESCRIPTION:     game settings class
************************************************************************/

// Physical component dependency
#include <utilities/settings.h>

// SDL lib dependencies
#include <SDL.h>

// Boost lib dependencies
#include <boost/lexical_cast.hpp>

// Game lib dependencies
#include <common/defs.h>

/************************************************************************
*    desc:  Constructer                                                             
************************************************************************/
CSettings::CSettings()
    : m_size(1024,768),
      m_default_size(1280,768),
      m_fullScreen(false),
      m_vSync(false),
      m_major(2),
      m_minor(1),
      m_profile(SDL_GL_CONTEXT_PROFILE_CORE),
      m_viewAngle(45.f),
      m_minZdist(5.f),
      m_maxZdist(1000.f),
      m_gamepadStickDeadZone(2500),
      m_createStencilBuffer(false),
      m_stencilBufferBitSize(0),
      m_clearStencilBuffer(false),
      m_enableDepthBuffer(false),
      m_clearTargetBuffer(true)
{
}   // constructor


/************************************************************************
*    desc:  destructer                                                             
************************************************************************/
CSettings::~CSettings()
{
}	// destructer


/************************************************************************
*    desc:  Load settings data from xml file
*  
*    param: string & _filePath - path to file
*
*    ret: string - class name
************************************************************************/
void CSettings::LoadFromXML( const std::string & filePath )
{
    m_filePath = filePath;

    // this open and parse the XML file:
    m_mainNode = XMLNode::openFileHelper( filePath.c_str(), "settings" );

    if( !m_mainNode.isEmpty() )
    {
        XMLNode displayListNode = m_mainNode.getChildNode("display");
        if( !displayListNode.isEmpty() )
        {
            // Get the attributes from the "resolution" node
            XMLNode resolutionNode = displayListNode.getChildNode("resolution");
            m_size.w = std::stoi(resolutionNode.getAttribute("width"));
            m_size.h = std::stoi(resolutionNode.getAttribute("height"));

            m_fullScreen = ( resolutionNode.getAttribute("fullscreen") == std::string("true") );

            // Get the attributes from the "defaultHeight" node
            XMLNode defResNode = displayListNode.getChildNode("defaultHeight");
            m_default_size.h = std::stof(defResNode.getAttribute("height"));

            CalcRatio();
        }

        XMLNode deviceNode = m_mainNode.getChildNode("device");
        if( !deviceNode.isEmpty() )
        {
            // Get the attribute for OpenGL node
            XMLNode OpenGLNode = deviceNode.getChildNode("OpenGL");
            if( !OpenGLNode.isEmpty() )
            {
                m_major = std::stoi( OpenGLNode.getAttribute("major") );
                m_minor = std::stoi( OpenGLNode.getAttribute("minor") );

                if( std::string( OpenGLNode.getAttribute("profile") ) == "core" )
                    m_profile = SDL_GL_CONTEXT_PROFILE_CORE;

                else if( std::string( OpenGLNode.getAttribute("profile") ) == "compatibility" )
                    m_profile = SDL_GL_CONTEXT_PROFILE_COMPATIBILITY;

                else if( std::string( OpenGLNode.getAttribute("profile") ) == "es" )
                    m_profile = SDL_GL_CONTEXT_PROFILE_ES;
            }

            // Get the projection info
            XMLNode projectionNode = deviceNode.getChildNode("projection");
            m_minZdist = std::atof(projectionNode.getAttribute("minZDist"));
            m_maxZdist = std::atof(projectionNode.getAttribute("maxZDist"));
            m_viewAngle = std::atof(projectionNode.getAttribute("view_angle")) * defs_DEG_TO_RAD;

            // Get the attribute from the "backbuffer" node
            XMLNode backBufferNode = deviceNode.getChildNode("backbuffer");
            if( !backBufferNode.isEmpty() )
            {
                //tripleBuffering = (backBufferNode.getAttribute("tripleBuffering") == string("true"));
                m_vSync = (backBufferNode.getAttribute("VSync") == std::string("true"));
            }

            XMLNode joypadNode = deviceNode.getChildNode("joypad");
            if( !joypadNode.isEmpty() )
            {
                m_gamepadStickDeadZone = std::atoi(joypadNode.getAttribute("stickDeadZone"));
            }

            // Get the attribute from the "depthStencilBuffer" node
            XMLNode depthStencilBufferNode = deviceNode.getChildNode("depthStencilBuffer");
            if( !depthStencilBufferNode.isEmpty() )
            {
                // Do we enable the depth buffer
                if( depthStencilBufferNode.isAttributeSet("enableDepthBuffer") )
                    m_enableDepthBuffer = ( depthStencilBufferNode.getAttribute("enableDepthBuffer") == std::string("true") );

                // Do we create the stencil buffer
                if( depthStencilBufferNode.isAttributeSet("createStencilBuffer") )
                    m_createStencilBuffer = ( depthStencilBufferNode.getAttribute("createStencilBuffer") == std::string("true") );

                // Do we clear the stencil buffer
                if( depthStencilBufferNode.isAttributeSet("clearStencilBuffer") )
                    m_clearStencilBuffer = ( depthStencilBufferNode.getAttribute("clearStencilBuffer") == std::string("true") );

                // Get the number of bits for the stencil buffer
                if( depthStencilBufferNode.isAttributeSet( "stencilBufferBitSize" ) )
                    m_stencilBufferBitSize = std::atoi( depthStencilBufferNode.getAttribute("stencilBufferBitSize") );
            }
        }
    }

}	// LoadFromXML


/************************************************************************
*    desc:  Calculate the ratio
*
*    ret: string - class name
************************************************************************/
void CSettings::CalcRatio()
{
    // NOTE: the default width is based on the actual resolution
    m_default_size.w = (m_size.w / m_size.h) * m_default_size.h;

    // Get half the size for use with screen boundries
    m_default_size_half = m_default_size / 2.f;

    m_size_half = m_size / 2.f;

    // Calculate the screen ratio because of the default height
    m_screenRatio.w = m_size.w / m_default_size.w;
    m_screenRatio.h = m_size.h / m_default_size.h;

    // Ratio for devices (mice) to correctly calculate movement
    // between the default height and actual height
    m_deviceRatio = m_default_size.h / m_size.h;

}   // CalcRatio


/************************************************************************
*    desc:  Get/Set game window size
************************************************************************/
const CSize<float> & CSettings::GetSize() const
{
    return m_size;
}

void CSettings::SetSize( const CSize<float> & size )
{
    m_size = size;
}


/************************************************************************
*    desc:  Get game window size / 2
*
*    ret: CSize - size of game window / 2
************************************************************************/
const CSize<float> & CSettings::GetSizeHalf() const
{
    return m_size_half;
}


/************************************************************************
*    desc:  Get/Set vSync?
************************************************************************/
bool CSettings::GetVSync() const
{
    return m_vSync;
}

void CSettings::SetVSync( bool value )
{
    m_vSync = value;
}


/************************************************************************
*    desc:  Get the OpenGL major version
************************************************************************/
int CSettings::GetMajorVersion() const
{
    return m_major;
}


/************************************************************************
*    desc:  Get the OpenGL minor version
************************************************************************/
int CSettings::GetMinorVersion() const
{
    return m_minor;
}


/************************************************************************
*    desc:  Get the OpenGL profile type
************************************************************************/
int CSettings::GetProfile() const
{
    return m_profile;
}


/************************************************************************
*    desc:  Get the view angle
*
*    ret: float - view angle
************************************************************************/
float CSettings::GetViewAngle() const
{
    return m_viewAngle;
}


/************************************************************************
*    desc:  Get the minimum z distance
*
*    ret: float - minimum z distance
************************************************************************/
float CSettings::GetMinZdist() const
{
    return m_minZdist;
}


/************************************************************************
*    desc:  Get the maximum z distance
*
*    ret: float - maximum z distance
************************************************************************/
float CSettings::GetMaxZdist() const
{
    return m_maxZdist;
}


/************************************************************************
*    desc:  Get 3d area size
*
*    ret: CSize - size of game window
************************************************************************/
const CSize<float> & CSettings::GetDefaultSize() const
{
    return m_default_size;
}


/************************************************************************
*    desc:  Get 3d area size in half
*
*    ret: CSize - size of game window
************************************************************************/
const CSize<float> & CSettings::GetDefaultSizeHalf() const
{
    return m_default_size_half;
}


/************************************************************************
*    desc:  Get the height and width ratios of the screen
*
*    ret: CSize - ratio of the screen
************************************************************************/
const CSize<float> & CSettings::GetScreenRatio() const
{
    return m_screenRatio;
}


/************************************************************************
*    desc:  Get the height and width ratios of the screen
*
*    ret: CSize - ratio of the screen
************************************************************************/
float CSettings::GetDeviceRatio() const
{
    return m_deviceRatio;
}


/************************************************************************
*    desc:  Get the gamepad stick dead zone
************************************************************************/
int CSettings::GetGamePadStickDeadZone() const
{
    return m_gamepadStickDeadZone;
}


/************************************************************************
*    desc:  Set the gamepad stick dead zone
************************************************************************/
void CSettings::SetGamePadStickDeadZone( int value )
{
    m_gamepadStickDeadZone = value;
}


/************************************************************************
*    desc:  Get/Set full screen
************************************************************************/
bool CSettings::GetFullScreen() const
{
    return m_fullScreen;
}

void CSettings::SetFullScreen( bool value )
{
    m_fullScreen = value;
}


/************************************************************************
*    desc:  Do we create the depth stencil buffer
*
*    ret: bool - createDepthStencilBuffer
************************************************************************/
bool CSettings::GetCreateStencilBuffer() const
{
    return m_createStencilBuffer;
}


/************************************************************************
*    desc:  Get the bit size of the stencil buffer
*
*    ret: int - bit size
************************************************************************/
int CSettings::GetStencilBufferBitSize() const
{
    return m_stencilBufferBitSize;
}


/************************************************************************
*    desc:  Do we clear the stencil buffer
*
*    ret: bool - clearStencilBuffer
************************************************************************/
bool CSettings::GetClearStencilBuffer() const
{
    return m_clearStencilBuffer;
}


/************************************************************************
*    desc:  Is the depth buffer enabled by default
*
*    ret: bool - enableDepthBuffer
************************************************************************/
bool CSettings::GetEnableDepthBuffer() const
{
    return m_enableDepthBuffer;
}


/************************************************************************
*    desc:  Do we clear the target buffer
*
*    ret: bool - clearTargetBuffer
************************************************************************/
bool CSettings::GetClearTargetBuffer() const
{
    return m_clearTargetBuffer;
}


/************************************************************************
*    desc:  Save the settings file
************************************************************************/
void CSettings::SaveSettings()
{
    if( !m_mainNode.isEmpty() )
    {
        XMLNode displayListNode = m_mainNode.getChildNode("display");

        if( !displayListNode.isEmpty() )
        {
            // Get the attributes from the "resolution" node
            XMLNode resolutionNode = displayListNode.getChildNode("resolution");

            {
                std::string tmpStr = boost::lexical_cast<std::string>( m_size.w );
                resolutionNode.updateAttribute(tmpStr.c_str(), "width", "width");

                tmpStr = boost::lexical_cast<std::string>( m_size.h );
                resolutionNode.updateAttribute(tmpStr.c_str(), "height", "height");
            }

            {
                std::string tmpStr = "false";

                if( m_fullScreen )
                    tmpStr = "true";

                resolutionNode.updateAttribute(tmpStr.c_str(), "fullscreen", "fullscreen");
            }
        }

        XMLNode deviceNode = m_mainNode.getChildNode("device");

        if( !deviceNode.isEmpty() )
        {
            {
                // Get the attribute from the "backbuffer" node
                XMLNode backBufferNode = deviceNode.getChildNode("backbuffer");

                std::string tmpStr = "false";

                if( m_vSync )
                    tmpStr = "true";

                backBufferNode.updateAttribute(tmpStr.c_str(), "VSync", "VSync");
            }
            /*{
                // Get the attribute from the "backbuffer" node
                XMLNode textFilterNode = deviceNode.getChildNode("textureFiltering");

                std::string tmpStr = GetTextFilterString();

                textFilterNode.updateAttribute(tmpStr.c_str(), "filter", "filter");
            }*/
        }

        {
            // Get the attribute from the "joypad" node
            XMLNode joypadNode = deviceNode.getChildNode("joypad");

            std::string tmpStr = boost::lexical_cast<std::string>( m_gamepadStickDeadZone );

            joypadNode.updateAttribute(tmpStr.c_str(), "stickDeadZone", "stickDeadZone");
        }

        // Save the settings file
        m_mainNode.writeToFile(m_filePath.c_str(), "utf-8");
    }

}	// SaveSettings