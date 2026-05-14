
/************************************************************************
*    FILE NAME:       settings.h
*
*    DESCRIPTION:     game settings class
************************************************************************/

#ifndef __settings_h__
#define __settings_h__

// Standard lib dependencies
#include <string>

// Game lib dependencies
#include <utilities/xmlParser.h>
#include <common/size.h>

class CSettings
{
public:

    // Get the instance of the singleton class
    static CSettings & Instance()
    {
        static CSettings settings;
        return settings;
    }

    // Save the settings file
    void SaveSettings();

    // Load settings data from xml file
    void LoadFromXML( const std::string & filePath );

    // Get game window size
    const CSize<float> & GetSize() const;
    const CSize<float> & GetSizeHalf() const;
    const CSize<float> & GetDefaultSize() const;
    const CSize<float> & GetDefaultSizeHalf() const;
    void SetSize( const CSize<float> & size );

    // Do we want vSync?
    bool GetVSync() const;
    void SetVSync( bool value );

    // Get the OpenGL major version
    int GetMajorVersion() const;

    // Get the OpenGL minor version
    int GetMinorVersion() const;

    // Get the OpenGL profile type
    int GetProfile() const;

    // Get the view angle
    float GetViewAngle() const;

    // Get the minimum z distance
    float GetMinZdist() const;

    // Get the maximum z distance
    float GetMaxZdist() const;

    // Get ratios
    const CSize<float> & GetScreenRatio() const;

    // Get ratios
    float GetDeviceRatio() const;

    // Get the gamepad stick dead zone
    int GetGamePadStickDeadZone() const;

    // Set the gamepad stick dead zone
    void SetGamePadStickDeadZone( int value );

    // Get/Set full screen
    bool GetFullScreen() const;
    void SetFullScreen( bool value );

    // Calculate the ratio
    void CalcRatio();

    // Do we create the depth stencil buffer
    bool GetCreateStencilBuffer() const;

    // Get the bit size of the stencil buffer
    int GetStencilBufferBitSize() const;

    // Do we clear the stencil buffer
    bool GetClearStencilBuffer() const;

    // Is the depth buffer enabled by default
    bool GetEnableDepthBuffer() const;

    // Do we clear the target buffer
    bool GetClearTargetBuffer() const;

private:

    // Constructor
    CSettings();

    // Destructor
    virtual ~CSettings();

private:

    // xml node
    XMLNode m_mainNode;

    // file path string
    std::string m_filePath;

    // with and height of game window
    CSize<float> m_size;
    CSize<float> m_size_half;
    CSize<float> m_default_size;
    CSize<float> m_default_size_half;

    // Height and width screen ratio for render objects
    CSize<float> m_screenRatio;

    // Ratio for devices (mice) to correctly calculate movement
    // between the default height and actual height
    float m_deviceRatio;

    // Full screen flag loaded from file
    bool m_fullScreen;

    // VSync flag
    bool m_vSync;

    // OpenGL versions
    int m_major;
    int m_minor;
    int m_profile;

    // view angle
    float m_viewAngle;

    // minimum Z distance
    float m_minZdist;

    // maximum Z distance
    float m_maxZdist;

    // Gamepad dead zone
    int m_gamepadStickDeadZone;

    // Do we create the depth stencil buffer
    bool m_createStencilBuffer;

    // Number of bits in the stencil buffer
    int m_stencilBufferBitSize;

    // Do we clear the stencil buffer
    bool m_clearStencilBuffer;

    // Do we enable the depth buffer
    bool m_enableDepthBuffer;

    // Do we clear the target buffer
    bool m_clearTargetBuffer;

};

#endif  // __classtemplate_h__
