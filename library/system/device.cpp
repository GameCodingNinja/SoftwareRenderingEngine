
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

/************************************************************************
*    desc:  Constructer
************************************************************************/
CDevice::CDevice()
    : m_pWindow(nullptr)
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
    // Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS | SDL_INIT_TIMER ) < 0 )
        throw NExcept::CCriticalException("SDL could not initialize!", SDL_GetError() );

    // Get the window size
    const CSize<int> size( CSettings::Instance().GetSize() );

    // Create window
    m_pWindow = SDL_CreateWindow( "", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, size.GetW(), size.GetH(), SDL_WINDOW_HIDDEN );
    if( m_pWindow == nullptr )
        throw NExcept::CCriticalException("Game window could not be created!", SDL_GetError() );

    // Create the surface from the window
    CSoftwareRender::Instance().CreateSurface( m_pWindow );

    // Set the full screen
    if( CSettings::Instance().GetFullScreen() )
        SetFullScreen( CSettings::Instance().GetFullScreen() );

    // Create the projection matrixes
    CreateProjMatrix();
    
}   // Create


/************************************************************************
*    desc:  Create the projection matrixes
************************************************************************/
void CDevice::CreateProjMatrix()
{
    // Calc the aspect ratio
    float aspectRatio = CSettings::Instance().GetSize().GetW() / 
                        CSettings::Instance().GetSize().GetH();

    m_perspectiveMatrix.PerspectiveFovRH(
        CSettings::Instance().GetViewAngle(),
        aspectRatio,
        CSettings::Instance().GetMinZdist(),
        CSettings::Instance().GetMaxZdist() );

    m_orthographicMatrix.OrthographicRH(
        CSettings::Instance().GetDefaultSize().GetW(),
        CSettings::Instance().GetDefaultSize().GetH(),
        CSettings::Instance().GetMinZdist(),
        CSettings::Instance().GetMaxZdist() );

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
    if( visible )
        SDL_ShowWindow( m_pWindow );
    else
        SDL_HideWindow( m_pWindow );

}   // hide


/***************************************************************************
*   desc:  Set full screen or windowed mode
 ****************************************************************************/
void CDevice::SetFullScreen( bool fullscreen )
{
    int flag(0);

    if( fullscreen )
        flag = SDL_WINDOW_FULLSCREEN;

    if( SDL_SetWindowFullscreen( m_pWindow, flag ) < 0 )
        NGenFunc::PostDebugMsg( NGenFunc::FormatString("Warning: Unable to set full screen! SDL Error: %s", SDL_GetError()) );

}   // SetFullScreen


/***************************************************************************
*   desc:  Get the SDL window
 ****************************************************************************/
SDL_Window * CDevice::GetWindow()
{
    return m_pWindow;

}   // GetWindow
