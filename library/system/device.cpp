
/************************************************************************
*    FILE NAME:       device.cpp
*
*    DESCRIPTION:     Class used for window management
************************************************************************/

// Physical component dependency
#include <system/device.h>

// Game lib dependencies
#include <utilities/exceptionhandling.h>
#include <utilities/settings.h>
#include <utilities/genfunc.h>
#include <common/size.h>
#include <softwareRender/softwareRender.h>
#include <system/windowfactory.h>
#include <system/iwindow.h>
#include <system/iframebuffer.h>

/************************************************************************
*    desc:  Constructer
************************************************************************/
CDevice::CDevice()
{
}   // constructor


/************************************************************************
*    desc:  destructer                                                             
************************************************************************/
CDevice::~CDevice()
{
}	// destructer


/***************************************************************************
*   desc:  Create the window
 ****************************************************************************/
void CDevice::Create()
{
    // Get the window size
    const CSize<int> size( CSettings::Instance().getSize() );

    // Create the native window
    m_upWindow = CreateNativeWindow();
    m_upWindow->Create( size.getW(), size.getH() );

    // Set the software render surface to the window's framebuffer
    CSoftwareRender::Instance().SetSurface( m_upWindow->GetFrameBuffer() );

    // Set the full screen
    if( CSettings::Instance().getFullScreen() )
        SetFullScreen( CSettings::Instance().getFullScreen() );

    // Create the projection matrixes
    CreateProjMatrix();
    
}   // Create


/************************************************************************
*    desc:  Create the projection matrixes
************************************************************************/
void CDevice::CreateProjMatrix()
{
    // Calc the aspect ratio
    float aspectRatio = CSettings::Instance().getSize().getW() / 
                        CSettings::Instance().getSize().getH();

    m_perspectiveMatrix.perspectiveFovRH(
        CSettings::Instance().getViewAngle(),
        aspectRatio,
        CSettings::Instance().getMinZdist(),
        CSettings::Instance().getMaxZdist() );

    m_orthographicMatrix.orthographicRH(
        CSettings::Instance().getDefaultSize().getW(),
        CSettings::Instance().getDefaultSize().getH(),
        CSettings::Instance().getMinZdist(),
        CSettings::Instance().getMaxZdist() );

}   // CreateProjMatrix


/************************************************************************
*    desc:  Get the projection matrix
************************************************************************/
const CMatrix & CDevice::GetProjectionMatrix( NDefs::EProjectionType type ) const
{
    if( type == NDefs::EPT_PERSPECTIVE )
        return m_perspectiveMatrix;
    else
        return m_orthographicMatrix;

}   // GetProjectionMatrix


/***************************************************************************
*   desc:  Show/Hide the Window
 ****************************************************************************/
void CDevice::ShowWindow( bool visible )
{
    m_upWindow->Show( visible );

}   // hide


/***************************************************************************
*   desc:  Set full screen or windowed mode
 ****************************************************************************/
void CDevice::SetFullScreen( bool fullscreen )
{
    m_upWindow->SetFullScreen( fullscreen );

    NGenFunc::PostDebugMsg( "SetFullScreen called" );

}   // SetFullScreen


/***************************************************************************
*   desc:  Get the native window
 ****************************************************************************/
IWindow * CDevice::GetNativeWindow()
{
    return m_upWindow.get();

}   // GetNativeWindow


/***************************************************************************
*   desc:  Get the frame buffer
 ****************************************************************************/
IFrameBuffer * CDevice::GetFrameBuffer()
{
    return m_upWindow->GetFrameBuffer();

}   // GetFrameBuffer
