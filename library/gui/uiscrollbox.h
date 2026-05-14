
/************************************************************************
*    FILE NAME:       uiscrollbox.h
*
*    DESCRIPTION:     Class for user interface scroll boxes
************************************************************************/

#ifndef __ui_scroll_box_h__
#define __ui_scroll_box_h__

// Physical component dependency
#include <gui/uisubcontrol.h>

// Standard lib dependencies
#include <vector>

// Boost lib dependencies
#include <boost/scoped_ptr.hpp>

// Game lib dependencies
#include <common/point.h>
#include <common/defs.h>
#include <2d/sprite2d.h>

// Forward declaration(s)
class CSprite2D;

class CUIScrollBox : public CUISubControl
{
public:

    // Constructor
    CUIScrollBox();

    // Destructor
    virtual ~CUIScrollBox();
    
    // Load the initial info from XML node
    virtual void LoadFromNode( const XMLNode & node );

    // Update the control
    virtual void Update();
    
    // Handle events
    virtual void HandleEvent( const SDL_Event & rEvent );

    // Transform the control
    virtual void DoTransform( const CObject & object );

    // Render the control
    virtual void Render( const CMatrix & matrix );

    // Set the first inactive control to be active
    virtual bool ActivateFirstInactiveControl();
    
    // Handle the mouse move
    virtual bool OnMouseMove( const SDL_Event & rEvent );

protected:

    // Load the control specific info from XML node
    virtual void LoadControlFromNode( const XMLNode & node );

    // Add the scroll control from node
    CUIControl * AddScrollControlFromNode( const XMLNode & controlNode );
    
    // Handle Action messages
    virtual void OnUpAction( const SDL_Event & rEvent );
    virtual void OnDownAction( const SDL_Event & rEvent );
    
    // Handle the select action
    virtual bool HandleSelectAction( NDefs::EDeviceId deviceUsed, NMenu::EActionPress pressType );
    
    // Handle the keyboard/Gamepad scrolling
    void HandleKeyboardGamepadScroll( int scrollVector );
    
    // Handle the sub control mouse move
    virtual bool OnSubControlMouseMove( const SDL_Event & rEvent );
    
    // Get the pointer to the subcontrol if found
    virtual CUIControl * FindSubControl( const std::string & name );
    
    // Deactivate the sub control
    virtual void DeactivateSubControl();
    
    // Set the start and end positions
    void SetStartEndPos();
    
    //
    void RepositionScrollControls();

private:

    // Vector list of controlsin scroll box
    std::vector<CUIControl *> m_pScrollControlVec;

    // Initial scroll box control offset
    CPoint<float> m_initialOffset;

    // Height to cull
    float m_cullHeight;

    // height of control
    uint m_controlHeight;
    
    // Scroll move counter
    float m_scrollCurPos;

    // Number of controls visible in scroll box
    int m_visibleCount;
    
    // Visible start pos
    int m_visStartPos;
    int m_visEndPos;

    // Max scroll amount
    float m_maxMoveAmount;

    // stencil mask sprite
    boost::scoped_ptr<CSprite2D> m_stencilMaskSprite;
    
    // Active scroll control index in this control
    int m_activeScrollCtrl;
    
    // Last active scroll control index
    int m_lastActiveScrollCtrl;
    
    // index of first control in scroll box
    int m_firstScrollCtrlIndex;
    
    // Default offsets
    std::vector<float> m_defaultOffset;

};

#endif  // __ui_scroll_box_h__


