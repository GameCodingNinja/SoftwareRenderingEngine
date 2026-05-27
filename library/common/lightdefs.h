
/************************************************************************
*    FILE NAME:       lightdefs.h
*
*    DESCRIPTION:     Lighting type definitions
************************************************************************/

#ifndef __light_defs_h__
#define __light_defs_h__

// Define one of these to select shading model
#define LIGHTING_GOURAUD
//#define LIGHTING_PHONG

#if defined(LIGHTING_GOURAUD) && defined(LIGHTING_PHONG)
#error "Only one shading model can be active: LIGHTING_GOURAUD or LIGHTING_PHONG"
#endif

#endif
