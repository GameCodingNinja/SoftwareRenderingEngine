
/************************************************************************
*    FILE NAME:       uicheckbox.h
*
*    DESCRIPTION:     Class for user interface check box buttons
************************************************************************/

#ifndef __ui_check_box_h__
#define __ui_check_box_h__

// Physical component dependency
#include <gui/uicontrol.h>

class CUICheckBox : public CUIControl
{
public:

    // Constructor
    CUICheckBox();

    // Destructor
    virtual ~CUICheckBox();

    // Set or get the Select state of this control
    void SetToggleState( bool value );
    bool GetToggleState();

    // Render the button
    virtual void Render( const CMatrix & matrix );

protected:

    virtual void OnSelectExecute( const SDL_Event & rEvent );
    
private:

    // Select state
    bool m_toggleState;

};

#endif  // __classtemplate_h__


