
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

    // Get/Set full screen
    bool GetFullScreen() const;
    void SetFullScreen( bool value );

    // Calculate the ratio
    void CalcRatio();

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

    // view angle
    float m_viewAngle;

    // minimum Z distance
    float m_minZdist;

    // maximum Z distance
    float m_maxZdist;

};

#endif  // __classtemplate_h__
