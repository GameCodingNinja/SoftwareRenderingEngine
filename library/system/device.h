
/************************************************************************
*    FILE NAME:       device.h
*
*    DESCRIPTION:     Class used for window management
************************************************************************/

#ifndef __device_h__
#define __device_h__

// Standard lib dependencies
#include <string>
#include <memory>

// Game lib dependencies
#include <common/matrix.h>
#include <common/defs.h>

// Forward declaration(s)
class IWindow;
class IFrameBuffer;

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

    // Get the native window
    IWindow* GetNativeWindow();

    // Get the frame buffer
    IFrameBuffer* GetFrameBuffer();

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
    std::unique_ptr<IWindow> m_upWindow;

    // Projection matrices
    CMatrix m_perspectiveMatrix;
    CMatrix m_orthographicMatrix;

};

#endif  // __device_h__
