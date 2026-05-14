
/************************************************************************
*    FILE NAME:       smartvsynccheckbox.h
*
*    DESCRIPTION:     Class CSmartVSyncCheckBox
************************************************************************/

#ifndef __smart_vsync_check_box_h__
#define __smart_vsync_check_box_h__

// Physical component dependency
#include "smartsettingsmenubtn.h"

class CSmartVSyncCheckBox : public CSmartSettingsMenuBtn
{
public:

    // Constructor
    explicit CSmartVSyncCheckBox( CUIControl * pUIControl );

    // Called when the gui is to be displayed
    void Display();

    // Called when the control is executed
    void Execute();

};

#endif  // __smart_vsync_check_box_h__


