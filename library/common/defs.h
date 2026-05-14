
/************************************************************************
*    FILE NAME:   defs.h
*
*    DESCRIPTION: misc defines. Some expected to be used with windows.h
************************************************************************/

#ifndef __defs_h__
#define __defs_h__

// define an unsigned int
typedef unsigned int uint;
typedef unsigned char uchar;

#define defs_DEG_TO_RAD 0.0174532925199432957
#define defs_RAD_TO_DEG 57.29577951308232
#define defs_EPSILON 8.854187817e-12
#define defs_PI_2 1.57079632679489661923
#define defs_RGB_TO_DEC 0.00390625f
#define TRI 3

namespace NDefs
{
    const char UNKNOWN_ACTION(-1);

    // Analog stick max values -32768 to 32767 but to simplify it, we'll just use 32767
    const int MAX_ANALOG_AXIS_VALUE(32767);
    const float ANALOG_PERCENTAGE_CONVERTION(327.67f);

    enum EProjectionType
    {
        EPT_NULL,
        EPT_PERSPECTIVE,
        EPT_ORTHOGRAPHIC
    };

    enum EHorzAlignment
    {
        EHA_HORZ_LEFT,
        EHA_HORZ_CENTER,
        EHA_HORZ_RIGHT
    };

    enum EVertAlignment
    {
        EVA_VERT_TOP,
        EVA_VERT_CENTER,
        EVA_VERT_BOTTOM
    };

    enum EGenerationType
    {
        EGT_NULL,
        EGT_QUAD,
        EGT_SCALED_FRAME,
        EGT_MESH_FILE,
        EGT_FONT
    };

    enum
    {
        // No parameters
        NONE = 0,

        // Transform parameters
        ROTATE	  = 0x0001,
        SCALE	  = 0x0002,

        // Translate parameters
        TRANSFORM_LOCAL = 0x0004,
        TRANSFORM_WORLD_POS = 0x0008
    };

    enum EPoint2D
    {
        EP2D_X,
        EP2D_Y,
        EP2D_MAX,
    };

    enum EPoint3D
    {
        EP3D_X,
        EP3D_Y,
        EP3D_Z,
        EP3D_MAX,
    };

    enum EVert
    {
        EVERT_1,
        EVERT_2,
        EVERT_3,
        EVERT_MAX,
    };

    enum EDeviceId
    {
        DEVICE_NULL=-1,
        KEYBOARD,
        MOUSE,
        GAMEPAD,
        MAX_UNIQUE_DEVICES
    };
}

#endif // __defs_h__
