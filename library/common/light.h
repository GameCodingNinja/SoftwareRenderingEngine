
/************************************************************************
*    FILE NAME:       light.h
*
*    DESCRIPTION:     Light class for 3D lighting
************************************************************************/

#ifndef __light_h__
#define __light_h__

// Game lib dependencies
#include <common/point.h>
#include <common/color.h>

enum class ELightType
{
    AMBIENT,
    DIRECTIONAL,
    POINT
};

class CLight
{
public:

    // Light type
    ELightType m_type = ELightType::DIRECTIONAL;

    // World-space position (point lights)
    CPoint<float> m_position;

    // Normalized direction (directional lights)
    CPoint<float> m_direction = CPoint<float>(0.0f, 0.0f, -1.0f);

    // Light color
    CColor<float> m_color = CColor<float>(1.0f, 1.0f, 1.0f, 1.0f);

    // Scalar intensity multiplier
    float m_intensity = 1.0f;

    // Falloff radius (point lights)
    float m_radius = 50.0f;

    // Enable specular highlights (directional/point)
    bool m_specular = false;

    // Specular exponent (controls highlight tightness)
    float m_shininess = 32.0f;
};

#endif
