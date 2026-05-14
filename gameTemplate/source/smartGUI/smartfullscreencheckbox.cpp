
/************************************************************************
*    FILE NAME:       smartfullscreencheckbox.cpp
*
*    DESCRIPTION:     Class CSmartScrnCheckBox
************************************************************************/

// Physical component dependency
#include "smartfullscreencheckbox.h"

// Game lib dependencies
#include <gui/uicheckbox.h>
#include <utilities/settings.h>
#include <utilities/genfunc.h>

// Required namespace(s)
using namespace std;

/************************************************************************
*    desc:  Constructer
************************************************************************/
CSmartScrnCheckBox::CSmartScrnCheckBox( CUIControl * pUIControl )
                   : CSmartSettingsMenuBtn( pUIControl )
{
}   // constructor


/***************************************************************************
*    decs:  Called when the gui is to be displayed
****************************************************************************/
void CSmartScrnCheckBox::Display()
{
    (*dynamic_cast<CUICheckBox *>(m_pUIControl)).SetToggleState( CSettings::Instance().GetFullScreen() );

}	// Display


/***************************************************************************
*    decs:  Called when the control is executed - Enable/disable the apply btn
****************************************************************************/
void CSmartScrnCheckBox::Execute()
{
    EnableDisableApplyBtn();

}	// Execute







