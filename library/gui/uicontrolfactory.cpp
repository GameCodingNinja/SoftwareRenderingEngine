/************************************************************************
*    FILE NAME:       uicontrolfactory.cpp
*
*    DESCRIPTION:     Class factory for control creation
************************************************************************/

// Physical component dependency
#include <gui/uicontrolfactory.h>

// Standard lib dependencies
#include <string>

// Boost lib dependencies
#include <boost/format.hpp>

// Game lib dependencies
#include <utilities/exceptionhandling.h>
#include <utilities/xmlParser.h>
#include <managers/signalmanager.h>
#include <gui/uilabel.h>
#include <gui/uibutton.h>
#include <gui/uisubcontrol.h>
#include <gui/uibuttonlist.h>
#include <gui/uicheckbox.h>
#include <gui/uislider.h>
#include <gui/uiscrollbox.h>
/*#include <gui/controlevent.h>
#include <gui/uitabcontrol.h>
#include <gui/uiprogressbar.h>
#include <gui/uiamountbutton.h>*/

namespace NUIControlFactory
{
    /************************************************************************
    *    desc:  Load the control info from XML node
    *  
    *    param: node - XML node
    ************************************************************************/
    CUIControl * Create( const XMLNode & node )
    {
        CUIControl * pControl;

        // Get the control type. This is required
        std::string ctrlType = node.getAttribute( "controlType" );

            // New up the control with its respected control type
        if( ctrlType == "label" )
            pControl = new CUILabel;

        else if( ctrlType == "button" )
                pControl = new CUIButton;

        else if( ctrlType == "sub_control" )
                pControl = new CUISubControl;

        else if( ctrlType == "button_list" )
                pControl = new CUIButtonList;

        else if( ctrlType == "check_box" )
                pControl = new CUICheckBox;

        else if( ctrlType == "slider" )
                pControl = new CUISlider;

        else if( ctrlType == "scroll_box" )
                pControl = new CUIScrollBox;

        /*else if( ctrlType == "tab_control" )
                pControl = new CUITabControl;

        else if( ctrlType == "progress_bar" )
                pControl = new CUIProgressBar;

        else if( ctrlType == "amount_button" )
                pControl = new CUIAmountButton;*/

        else
            throw NExcept::CCriticalException("UI Factory Control Load Error!",
                boost::str( boost::format("UI Control not defined (%s).\n\n%s\nLine: %s") % ctrlType % __FUNCTION__ % __LINE__ ));

        // Have the control load it's share
        pControl->LoadFromNode( node );

        // Broadcast signal to let the game handle smart gui inits
        CSignalManager::Instance().Broadcast( pControl );

        // Do any smart gui Create
        pControl->SmartCreate();

        return pControl;

    }	// Create

}