
/************************************************************************
*    FILE NAME:       smartapplysettingsbtn.h
*
*    DESCRIPTION:     Class CSmartApplySettingsBtn
************************************************************************/

#ifndef __smart_apply_settings_btn_h__
#define __smart_apply_settings_btn_h__

// Physical component dependency
#include "smartsettingsmenubtn.h"

class CSmartApplySettingsBtn : public CSmartSettingsMenuBtn
{
public:

    // Constructor
    explicit CSmartApplySettingsBtn( CUIControl * pUIControl );

    // Called when the gui is to be displayed
    void Display();

    // Called when the control is to excute
    void Execute();

};

#endif  // __smart_apply_settings_btn_h__


