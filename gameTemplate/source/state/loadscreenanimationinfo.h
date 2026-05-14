
/************************************************************************
*    FILE NAME:       loadscreenanimationinfo.h
*
*    DESCRIPTION:     Load screen animation info
************************************************************************/

#ifndef __load_screen_animation_info_h__
#define __load_screen_animation_info_h__

// SDL lib dependencies
#include <SDL.h>

class CLoadScrnAnim
{
public:

    CLoadScrnAnim() : 
      pSurface(NULL),
      fadeTime(0),
      displayDelay(0)
      {}

    SDL_Surface * pSurface;

    int fadeTime;
    int displayDelay;
    
};

#endif  // __load_screen_animation_info_h__


