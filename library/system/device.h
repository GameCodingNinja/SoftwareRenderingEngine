
/************************************************************************
*    FILE NAME:       device.h
*
*    DESCRIPTION:     Class used for window management
************************************************************************/

#ifndef __device_h__
#define __device_h__

// SDL lib dependencies
#include <SDL.h>

// Standard lib dependencies
#include <string>

// Game lib dependencies
#include <common/matrix.h>
#include <common/defs.h>

class CDevice
{
public:

    // Get the instance
    static CDevice & Instance()
    {
        static CDevice device;
        return device;
    }

    // Create the window
    void Create();

    // Show/Hide the Window
    void ShowWindow( bool visible );

    // Get the projection matrix
    const CMatrix & GetProjectionMatrix( NDefs::EProjectionType type ) const;

    // Get the SDL window
    SDL_Window * GetWindow();

    // Set full screen or windowed mode
    void SetFullScreen( bool fullscreen );

    // Create the projection matrixes
    void CreateProjMatrix();

private:

    // Constructor
    CDevice();

    // Destructor
    virtual ~CDevice();

private:

    // The window we'll be rendering to
    SDL_Window * m_pWindow;

    // Projection matrices
    CMatrix m_perspectiveMatrix;
    CMatrix m_orthographicMatrix;

};

#endif  // __device_h__
