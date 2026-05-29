
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
    POINT,
    SPOT
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

    // Scalar intensity multiplier (0.0 = off, 1.0 = full, >1.0 = overbright)
    float m_intensity = 1.0f;

    // Falloff radius for point/spot lights (0.0 = no reach, larger = wider coverage)
    float m_radius = 50.0f;

    // Spot light cone angles stored as cosines of the half-angle
    // Inner = full intensity, outer = zero, smooth falloff between
    // Range: 0.0 (180° hemisphere) to 1.0 (infinitely narrow), inner > outer
    float m_innerCone = 0.9f;   // cos(~25 degrees)
    float m_outerCone = 0.7f;   // cos(~45 degrees)

    // Enable specular highlights (directional/point/spot)
    bool m_specular = false;

    // Specular exponent (1.0 = broad highlight, 128.0 = tight pinpoint)
    float m_shininess = 32.0f;
};

#endif
