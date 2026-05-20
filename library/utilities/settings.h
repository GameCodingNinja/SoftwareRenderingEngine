
/************************************************************************
*    FILE NAME:       settings.h
*
*    DESCRIPTION:     game settings class
************************************************************************/

#pragma once

// Standard lib dependencies
#include <string>

// Game lib dependencies
#include <utilities/xmlParser.h>
#include <common/size.h>
#include <common/defs.h>

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
    void saveSettings();

    // Load settings data from xml file
    void loadFromXML( const std::string & filePath );

    // Get game window size
    const CSize<float> & getSize() const;
    const CSize<float> & getSizeHalf() const;
    const CSize<float> & getDefaultSize() const;
    const CSize<float> & getDefaultSizeHalf() const;
    void setSize( const CSize<float> & size );

    // Get the view angle
    float getViewAngle() const;

    // Get the minimum z distance
    float getMinZdist() const;

    // Get the maximum z distance
    float getMaxZdist() const;

    // Get the projection type
    NDefs::EProjectionType getProjectionType() const;

    // Height and width screen ratio for orthographic objects
    const CSize<float> & getOrthoAspectRatio() const;

    // Height and width screen ratio for perspective projection
    const CSize<float> & getScreenAspectRatio() const;

    // Get the screen ratio (ortho aspect ratio)
    const CSize<float> & getScreenRatio() const;

    // Get ratios
    float getDeviceRatio() const;

    // Get/Set VSync
    bool getVSync() const;
    void setVSync( bool value );

    // Get/Set full screen
    bool getFullScreen() const;
    void setFullScreen( bool value );

    // Calculate the ratio
    void calcRatio();

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

    // Height and width screen ratio for perspective projection
    CSize<float> m_screenAspectRatio;

    // Pre-calculated aspect ratios for orthographic projection
    CSize<float> m_orthoAspectRatio;

    // Ratio for devices (mice) to correctly calculate movement
    // between the default height and actual height
    float m_deviceRatio;

    // Full screen flag loaded from file
    bool m_fullScreen;

    // VSync flag
    bool m_vSync;

    // view angle
    float m_viewAngle;

    // minimum Z distance
    float m_minZdist;

    // maximum Z distance
    float m_maxZdist;

    // The projection type
    NDefs::EProjectionType m_projectionType;

};
