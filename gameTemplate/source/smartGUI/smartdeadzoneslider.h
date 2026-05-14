
/************************************************************************
*    FILE NAME:       smartdeadzoneslider.h
*
*    DESCRIPTION:     Class CSmartVSyncCheckBox
************************************************************************/

#ifndef __smart_dead_zone_slider_h__
#define __smart_dead_zone_slider_h__

// Physical component dependency
#include "smartsettingsmenubtn.h"

class CSmartDeadZoneSlider : public CSmartSettingsMenuBtn
{
public:

    // Constructor
    explicit CSmartDeadZoneSlider( CUIControl * pUIControl );

    // Called when the gui is to be displayed
    void Display();

    // Called when the control is executed
    void Execute();

};

#endif  // __smart_dead_zone_slider_h__


