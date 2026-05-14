
/************************************************************************
*    FILE NAME:       menu.h
*
*    DESCRIPTION:     Class for user interface menu
************************************************************************/

#ifndef __menu_h__
#define __menu_h__

// SDL/OpenGL lib dependencies
#include <SDL.h>

// Physical component dependency
#include <common/object.h>

// Standard lib dependencies
#include <string>
#include <vector>
#include <map>

// Boost lib dependencies
#include <boost/noncopyable.hpp>

// Game lib dependencies
#include <gui/menudefs.h>
#include <gui/uicontrolnavnode.h>
#include <gui/uicontrol.h>
#include <gui/scrollparam.h>
#include <common/dynamicoffset.h>

// Forward declaration(s)
struct XMLNode;
class CMenuSprite;
class CUIControl;

class CMenu : public CObject, boost::noncopyable
{
public:

    // Define the navigation helper map
    typedef std::map<const std::string, CUIControlNavNode *> NavHelperMap;

    CMenu( const std::string & name );
    virtual ~CMenu();

    // Load the menu info from XML file
    void LoadFromXML( const std::string & filePath );

    // Activate this menu because it's probably a root menu
    void ActivateMenu();

    // Update the menu
    void Update();

    // Transform the menu
    void DoTransform();

    // Get the name of the menu
    const std::string & GetName() const;

    // do the render
    void Render( const CMatrix & matrix );

    // Handle events
    void HandleEvent( const SDL_Event & rEvent );

    // Reset all controls
    void Reset();

    // Get the pointer to the control in question
    CUIControl * GetPtrToControl( const std::string & nameStr );

    // Load the dynamic offset data from node
    void LoadDynamicOffsetFromNode( const XMLNode & node );

    // Set the dynamic position
    void SetDynamicPos();

    // Does this menu use dynamic offsets
    bool IsDynamicOffset();

    // Get the scroll params
    CScrollParam & GetScrollParam( int msg );

private:

    // Load a static sprite from an XML node
    void LoadStaticSpriteFromNode( const XMLNode & node );

    // Load static controls from an XML node
    void LoadStaticControlFromNode( XMLNode & node );

    // Load mouse only controls from an XML node
    void LoadMouseOnlyControlFromNode( const XMLNode & node );

    // Load a control from an XML node
    void LoadControlFromNode( XMLNode & node, NavHelperMap & pNavNodeMap );

    // Find the reference nodes
    void FindNodes(
        const XMLNode & node,
        int focusIndex,
        int nodeIndex,
        NavHelperMap & pNavNodeMap );

    // Find the reference nodes
    void SetNodes(
        const XMLNode & node,
        int focusIndex,
        int nodeIndex,
        std::string attr,
        CUIControlNavNode::ENavNode navNode,
        NavHelperMap & pNavNodeMap );

    // Navigate the menu
    void NavigateMenu( CUIControlNavNode::ENavNode navNode );

    // Handle message
    void OnUpAction( const SDL_Event & rEvent );
    void OnDownAction( const SDL_Event & rEvent );
    void OnLeftAction( const SDL_Event & rEvent );
    void OnRightAction( const SDL_Event & rEvent );
    void OnSelectAction( const SDL_Event & rEvent );
    void OnSetActiveControl( const SDL_Event & rEvent );
    void OnReactivate( const SDL_Event & rEvent );
    void OnTransIn( const SDL_Event & rEvent );
    void OnTransOut( const SDL_Event & rEvent );
    void OnMouseMove( const SDL_Event & rEvent );

    // Set the first inactive control to be active
    void ActivateFirstInactiveControl();

private:

    // This menu's name
    std::string m_name;

    // Group name
    std::string m_group;

    // Map of menu static sprites
    std::vector<CMenuSprite *> m_pStaticSpritesVec;

    // Vector list of static controls
    std::vector<CUIControl *> m_pStaticControlVec;

    // Vector list of mouse only controls
    std::vector<CUIControl *> m_pMouseOnlyControlVec;

    // Vector list of controls
    std::vector< std::vector<CUIControl *> > m_pControlVecVec;

    // Vector list of navigation nodes
    std::vector< std::vector<CUIControlNavNode *> > m_pControlNodeVecVec;

    // Map container of controls for easy name access
    // NOTE: This container does not own it's pointers.
    std::map<const std::string, CUIControl *> m_pControlMap;

    // Current active node
    // NOTE: This container does not own it's pointers.
    std::vector<CUIControlNavNode *> m_pActiveNodeVec;

    // menu state
    NMenu::EMenuState m_state;

    // focus group index
    int m_focusGroupIndex;

    // Dynamic offset
    CDynamicOffset m_dynamicOffset;

    // Scrolling parameters
    CScrollParam m_scrollParam;

};

#endif  // __menu_h__


