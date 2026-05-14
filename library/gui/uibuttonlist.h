
/************************************************************************
*    FILE NAME:       uibuttonlist.h
*
*    DESCRIPTION:     Class for user interface buttons
************************************************************************/

#ifndef __ui_button_list_h__
#define __ui_button_list_h__

// Physical component dependency
#include <gui/uisubcontrol.h>

class CUIButtonList : public CUISubControl
{
public:

    // Constructor
    CUIButtonList();

    // Destructor
    virtual ~CUIButtonList();

    // Set the first inactive control to be active
    virtual bool ActivateFirstInactiveControl();

    // Update the display
    void UpdateDisplay( int index );

    // Get the active index
    int GetActiveIndex();

protected:

    // Load the control specific info from XML node
    virtual void LoadControlFromNode( const XMLNode & node );

    // Handle message
    virtual void OnStateChange( const SDL_Event & rEvent );

private:

    // Handle Action messages
    virtual void OnLeftAction( const SDL_Event & rEvent );
    virtual void OnRightAction( const SDL_Event & rEvent );

    // Handle Scroll messages
    virtual void OnLeftScroll( const SDL_Event & rEvent );
    virtual void OnRightScroll( const SDL_Event & rEvent );

    // Inc/Dec the list
    void IncList();
    void DecList();

private:

    // Active index into the list
    int m_activeIndex;

    // Index of the image list
    int m_imageLstIndex;

};

#endif  // __ui_button_list_h__


