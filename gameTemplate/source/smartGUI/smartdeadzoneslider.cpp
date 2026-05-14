
/************************************************************************
*    FILE NAME:       smartdeadzoneslider.cpp
*
*    DESCRIPTION:     Class CSmartVSyncCheckBox
************************************************************************/

// Physical component dependency
#include "smartdeadzoneslider.h"

// Game lib dependencies
#include <gui/uislider.h>
#include <utilities/genfunc.h>
#include <utilities/settings.h>

/************************************************************************
*    desc:  Constructer
************************************************************************/
CSmartDeadZoneSlider::CSmartDeadZoneSlider( CUIControl * pUIControl )
                    : CSmartSettingsMenuBtn( pUIControl )
{
}   // constructor


/***************************************************************************
*    decs:  Called when the gui is to be displayed
****************************************************************************/
void CSmartDeadZoneSlider::Display()
{
    (*dynamic_cast<CUISlider *>(m_pUIControl)).SetSlider( CSettings::Instance().GetGamePadStickDeadZone() );

}	// Display


/***************************************************************************
*    decs:  Called when the control is executed - Enable/disable the apply btn
****************************************************************************/
void CSmartDeadZoneSlider::Execute()
{
    EnableDisableApplyBtn();

}	// Execute







