
/************************************************************************
*    FILE NAME:       smartfullscreencheckbox.h
*
*    DESCRIPTION:     Class CSmartScrnCheckBox
************************************************************************/

#ifndef __smart_full_screen_check_box_h__
#define __smart_full_screen_check_box_h__

// Physical component dependency
#include "smartsettingsmenubtn.h"

// Forward declaration(s)
class CUIControl;

class CSmartScrnCheckBox : public CSmartSettingsMenuBtn
{
public:

    // Constructor
    explicit CSmartScrnCheckBox( CUIControl * pUIControl );

    // Called when the gui is to be displayed
    void Display();

    // Called when the control is executed
    void Execute();

};

#endif  // __smart_full_screen_check_box_h__


