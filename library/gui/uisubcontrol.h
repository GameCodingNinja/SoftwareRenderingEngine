
/************************************************************************
*    FILE NAME:       uisubcontrol.h
*
*    DESCRIPTION:     Class for user interface controls with sub-controls
************************************************************************/

#ifndef __ui_sub_control_h__
#define __ui_sub_control_h__

// Physical component dependency
#include <gui/uicontrol.h>

// Standard lib dependencies
#include <vector>
#include <map>

// Game lib dependencies
#include <gui/uicontrolnavnode.h>

class CUISubControl : public CUIControl
{
public:

    // Define the navigation helper map
    typedef std::map<const std::string, CUIControlNavNode *> NavHelperMap;

    // Constructor
    CUISubControl();

    // Destructor
    virtual ~CUISubControl();

    // Update the control
    virtual void Update();

    // Transform the control
    virtual void DoTransform( const CObject & object );

    // Render the control
    virtual void Render( const CMatrix & matrix );

    // Handle events
    virtual void HandleEvent( const SDL_Event & rEvent );

    // Reset the control to inactive if its not disabled
    virtual void Reset( bool complete = false );

    // Handle the select action
    virtual bool HandleSelectAction( NDefs::EDeviceId deviceUsed, NMenu::EActionPress pressType );

    // Set the first inactive control to be active
    virtual bool ActivateFirstInactiveControl();
    
    // Deactivate the control
    virtual void DeactivateControl();

    // Get the pointer to the control if found
    virtual CUIControl * FindControl( const std::string & name );
    
    // Get the pointer to the subcontrol if found
    virtual CUIControl * FindSubControl( const std::string & name );

    // Get the sub control via index
    virtual CUIControl * GetSubControl( uint index = 0 );
    
    // Handle the mouse move
    virtual bool OnMouseMove( const SDL_Event & rEvent );

protected:

    // Load the control specific info from XML node
    virtual void LoadControlFromNode( const XMLNode & controlNode );

    // Handle message
    virtual void OnStateChange( const SDL_Event & rEvent );
    
    // Handle the sub control mouse move
    virtual bool OnSubControlMouseMove( const SDL_Event & rEvent );

    // Handle Action messages
    virtual void OnUpAction( const SDL_Event & rEvent );
    virtual void OnDownAction( const SDL_Event & rEvent );
    virtual void OnLeftAction( const SDL_Event & rEvent );
    virtual void OnRightAction( const SDL_Event & rEvent );

    // Handle Scroll messages
    virtual void OnUpScroll( const SDL_Event & rEvent );
    virtual void OnDownScroll( const SDL_Event & rEvent );
    virtual void OnLeftScroll( const SDL_Event & rEvent );
    virtual void OnRightScroll( const SDL_Event & rEvent );
    
    // Deactivate the sub control
    virtual void DeactivateSubControl();

private:

    // Find the reference nodes
    void FindNodes(
        const XMLNode & node,
        int nodeIndex,
        NavHelperMap & pNavNodeMap );

    // Find the reference nodes
    void SetNodes(
        const XMLNode & node,
        int nodeIndex,
        std::string attr,
        CUIControlNavNode::ENavNode navNode,
        NavHelperMap & pNavNodeMap );

private:

    // Navigate the menu
    void NavigateMenu( CUIControlNavNode::ENavNode navNode );

protected:

    // Vector list of sub-controls
    std::vector<CUIControl *> m_pSubControlVec;

    // Vector list of navigation nodes
    std::vector< CUIControlNavNode * > m_pControlNodeVec;

    // Current active node
    // NOTE: This variable does not own it's pointers.
    CUIControlNavNode * m_pActiveNode;

};

#endif  // __ui_sub_control_h__


