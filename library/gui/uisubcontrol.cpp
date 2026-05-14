
/************************************************************************
*    FILE NAME:       uisubcontrol.cpp
*
*    DESCRIPTION:     Class for user interface controls with sub-controls
************************************************************************/

// Physical component dependency
#include <gui/uisubcontrol.h>

// Boost lib dependencies
#include <boost/format.hpp>

// Game lib dependencies
#include <utilities/deletefuncs.h>
#include <utilities/xmlParser.h>
#include <utilities/genfunc.h>
#include <utilities/exceptionhandling.h>
#include <gui/uicontrolfactory.h>
#include <gui/menudefs.h>

/************************************************************************
*    desc:  Constructer
************************************************************************/
CUISubControl::CUISubControl()
    : m_pActiveNode(nullptr)
{
    SetType( NUIControl::ECT_SUB_CONTROL );

}   // constructor


/************************************************************************
*    desc:  destructer                                                             
************************************************************************/
CUISubControl::~CUISubControl()
{
    NDelFunc::DeleteVectorPointers(m_pSubControlVec);
    NDelFunc::DeleteVectorPointers( m_pControlNodeVec );

}	// destructer


/************************************************************************
*    desc:  Load the control specific info from XML node
************************************************************************/
void CUISubControl::LoadControlFromNode( const XMLNode & controlNode )
{
    // Have the parent load it's stuff
    CUIControl::LoadControlFromNode( controlNode );

    // Check for collision
    /*for( uint i = 0; i < spSpriteGroupVec.size() && !baseControlCollision; ++i )
    {
        if( spSpriteGroupVec[i].IsCollisionActive() )
            baseControlCollision = true;
    }

    // Get the sub-control settings
    XMLNode baseRespondsSelectNode = controlNode.getChildNode( "subControlSettings" );
    if( !baseRespondsSelectNode.isEmpty() )
    {
        // Does the base class respond to select. The default is true.
        if( baseRespondsSelectNode.isAttributeSet( "baseControlRespondsToSelect" ) )
            if( string( baseRespondsSelectNode.getAttribute( "baseControlRespondsToSelect" ) ) == "false" )
                baseReactsToSelect = false;

        // Do the sub-controls respond to all states. The default is false.
        if( baseRespondsSelectNode.isAttributeSet( "respondsToAllStates" ) )
            if( string( baseRespondsSelectNode.getAttribute( "respondsToAllStates" ) ) == "true" )
                respondsToAllStates = true;

        // Do the sub-controls respond gamepad/keyboard navigation.
        if( baseRespondsSelectNode.isAttributeSet( "baseControlAsContainer" ) )
            if( string( baseRespondsSelectNode.getAttribute( "baseControlAsContainer" ) ) == "true" )
            {
                baseControlAsContainer = true;

                // Don't want the base control to react to select when used as a container
                baseReactsToSelect = false;
            }
    }*/

    // Get the menu controls node
    const XMLNode controlListNode = controlNode.getChildNode( "subControlList" );
    if( !controlListNode.isEmpty() )
    {
        // map to help setup the node pointers
        NavHelperMap pNavNodeMap;

        for( int i = 0; i < controlListNode.nChildNode( "control" ); ++i )
        {
            XMLNode controlNode = controlListNode.getChildNode( "control", i );

            // The pointer is placed within a vector for all controls
            m_pSubControlVec.push_back( NUIControlFactory::Create( controlNode ) );

            // Load the transform data
            m_pSubControlVec.back()->LoadTransFromNode( controlNode );

            // Get the control name
            const std::string controlName = m_pSubControlVec.back()->GetName();

            // Does this control have a name then create a node and add it to the map
            if( !controlName.empty() )
            {
                // Add a node to the vector with it's control
                m_pControlNodeVec.push_back( new CUIControlNavNode( m_pSubControlVec.back() ) );

                // Map of menu control nodes
                pNavNodeMap.insert( std::make_pair(controlName, m_pControlNodeVec.back()) );
            }
        }

        for( int i = 0; i < controlListNode.nChildNode( "control" ); ++i )
        {
            XMLNode controlNode = controlListNode.getChildNode( "control", i );

            // Find the reference nodes
            FindNodes( controlNode, i, pNavNodeMap );
        }
    }

}	// LoadControlFromNode


/************************************************************************
*    desc:  Find the reference nodes
************************************************************************/
void CUISubControl::FindNodes(
    const XMLNode & node,
    int nodeIndex,
    NavHelperMap & pNavNodeMap )
{
    const XMLNode navNode = node.getChildNode( "navigate" );
    if( !navNode.isEmpty() )
    {
        SetNodes( navNode, nodeIndex, "up", CUIControlNavNode::ENAV_NODE_UP, pNavNodeMap );
        SetNodes( navNode, nodeIndex, "down", CUIControlNavNode::ENAV_NODE_DOWN, pNavNodeMap );
        SetNodes( navNode, nodeIndex, "left", CUIControlNavNode::ENAV_NODE_LEFT, pNavNodeMap );
        SetNodes( navNode, nodeIndex, "right", CUIControlNavNode::ENAV_NODE_RIGHT, pNavNodeMap );
    }

}   // FindNodes


/************************************************************************
*    desc:  Find the reference nodes
************************************************************************/
void CUISubControl::SetNodes(
    const XMLNode & node,
    int nodeIndex,
    std::string attr,
    CUIControlNavNode::ENavNode navNode,
    NavHelperMap & pNavNodeMap )
{
    if( node.isAttributeSet( attr.c_str() ) )
    {
        const std::string name = node.getAttribute( attr.c_str() );
        auto iter = pNavNodeMap.find( name );
        if( iter != pNavNodeMap.end() )
        {
            m_pControlNodeVec[nodeIndex]->SetNode( navNode, iter->second );
        }
        else
        {
            throw NExcept::CCriticalException("Control Node Find Error!",
                boost::str( boost::format("Control node doesn't exist (%s).\n\n%s\nLine: %s") % name % __FUNCTION__ % __LINE__ ));
        }
    }

}   // SetNodes


/************************************************************************
*    desc:  Update the control
************************************************************************/
void CUISubControl::Update()
{
    // Call the parent
    CUIControl::Update();

    // Update all controls
    for( size_t i = 0; i < m_pSubControlVec.size(); ++i )
        m_pSubControlVec[i]->Update();

}   // Update


/************************************************************************
*    desc:  Transform the control
************************************************************************/
void CUISubControl::DoTransform( const CObject & object )
{
    // Call the parent
    CUIControl::DoTransform( object );

    // Update all controls
    for( size_t i = 0; i < m_pSubControlVec.size(); ++i )
        m_pSubControlVec[i]->DoTransform( *this );

}   // DoTransform


/************************************************************************
*    desc:  Render the sub control
************************************************************************/
void CUISubControl::Render( const CMatrix & matrix )
{
    // Call the parent
    CUIControl::Render( matrix );

    if( IsVisible() )
    {
        for( size_t i = 0; i < m_pSubControlVec.size(); ++i )
            m_pSubControlVec[i]->Render( matrix );
    }

}	// Render


/************************************************************************
*    desc:  Handle events
************************************************************************/
void CUISubControl::HandleEvent( const SDL_Event & rEvent )
{
    // Call the parent
    CUIControl::HandleEvent( rEvent );

    for( size_t i = 0; i < m_pSubControlVec.size(); ++i )
        m_pSubControlVec[i]->HandleEvent( rEvent );

    if( IsActive() )
    {
        if( (rEvent.type >= NMenu::EGE_MENU_UP_ACTION) &&
            (rEvent.type <= NMenu::EGE_MENU_RIGHT_ACTION) )
        {
            if( rEvent.user.code == NMenu::EAP_DOWN )
            {
                if( rEvent.type == NMenu::EGE_MENU_UP_ACTION )
                {
                    OnUpAction( rEvent );
                }
                else if( rEvent.type == NMenu::EGE_MENU_DOWN_ACTION )
                {
                    OnDownAction( rEvent );
                }
                if( rEvent.type == NMenu::EGE_MENU_LEFT_ACTION )
                {
                    OnLeftAction( rEvent );
                }
                else if( rEvent.type == NMenu::EGE_MENU_RIGHT_ACTION )
                {
                    OnRightAction( rEvent );
                }
            }
        }
        else if( rEvent.type == NMenu::EGE_MENU_SCROLL_UP )
        {
            OnUpScroll( rEvent );
        }
        else if( rEvent.type == NMenu::EGE_MENU_SCROLL_DOWN )
        {
            OnDownScroll( rEvent );
        }
        else if( rEvent.type == NMenu::EGE_MENU_SCROLL_LEFT )
        {
            OnLeftScroll( rEvent );
        }
        else if( rEvent.type == NMenu::EGE_MENU_SCROLL_RIGHT )
        {
            OnRightScroll( rEvent );
        }
    }

}   // HandleEvent


/************************************************************************
*    desc:  Handle OnUpAction message
************************************************************************/
void CUISubControl::OnUpAction( const SDL_Event & rEvent )
{
    NavigateMenu( CUIControlNavNode::ENAV_NODE_UP );

}   // OnUpAction

/************************************************************************
*    desc:  Handle OnMenuDown message
************************************************************************/
void CUISubControl::OnDownAction( const SDL_Event & rEvent )
{
    NavigateMenu( CUIControlNavNode::ENAV_NODE_DOWN );

}   // OnUpAction

/************************************************************************
*    desc:  Handle OnMenuLeft message
************************************************************************/
void CUISubControl::OnLeftAction( const SDL_Event & rEvent )
{
    NavigateMenu( CUIControlNavNode::ENAV_NODE_LEFT );

}   // OnLeftAction

/************************************************************************
*    desc:  Handle OnRightAction message
************************************************************************/
void CUISubControl::OnRightAction( const SDL_Event & rEvent )
{
    NavigateMenu( CUIControlNavNode::ENAV_NODE_RIGHT );

}   // OnRightAction

/************************************************************************
*    desc:  Handle OnUpScroll message
************************************************************************/
void CUISubControl::OnUpScroll( const SDL_Event & rEvent )
{
    NavigateMenu( CUIControlNavNode::ENAV_NODE_UP );

}   // OnUpScroll

/************************************************************************
*    desc:  Handle OnUpScroll message
************************************************************************/
void CUISubControl::OnDownScroll( const SDL_Event & rEvent )
{
    NavigateMenu( CUIControlNavNode::ENAV_NODE_DOWN );

}   // OnDownScroll

/************************************************************************
*    desc:  Handle OnRightScroll message
************************************************************************/
void CUISubControl::OnLeftScroll( const SDL_Event & rEvent )
{
    NavigateMenu( CUIControlNavNode::ENAV_NODE_LEFT );

}   // OnLeftScrol

/************************************************************************
*    desc:  Handle OnRightScroll message
************************************************************************/
void CUISubControl::OnRightScroll( const SDL_Event & rEvent )
{
    NavigateMenu( CUIControlNavNode::ENAV_NODE_RIGHT );

}   // OnRightScroll


/************************************************************************
*    desc:  Navigate the menu
************************************************************************/
void CUISubControl::NavigateMenu( CUIControlNavNode::ENavNode navNode )
{
    if( m_pActiveNode != nullptr )
    {
        do
        {
            if( m_pActiveNode->GetNode( navNode ) == nullptr )
                break;

            else
            {
                m_pActiveNode = m_pActiveNode->GetNode( navNode );

                if( !m_pActiveNode->Get()->IsDisabled() )
                {
                    NGenFunc::DispatchEvent( 
                        NMenu::EGE_MENU_CONTROL_STATE_CHANGE,
                        NUIControl::ECS_ACTIVE,
                        &m_pActiveNode->Get()->GetName() );

                    break;
                }
            }
        }
        while( true );
    }

}   // NavigateMenu


/************************************************************************
*    desc:  Handle OnStateChange message
************************************************************************/
void CUISubControl::OnStateChange( const SDL_Event & rEvent )
{
    const std::string & ctrlName = *(std::string *)rEvent.user.data1;
    NUIControl::EControlState state = NUIControl::EControlState(rEvent.user.code);

    CUIControl * pCtrl = FindSubControl( ctrlName );

    // Restart the active state of the sub control if something
    // changed in the child controls or their children controls
    if( (state == NUIControl::ECS_ACTIVE) && (pCtrl != nullptr) )
    {
        if( pCtrl->GetState() != state )
        {
            SetState(state, true);

            RecycleContext();

            SetDisplayState();
        }
    }
    // The sub control doesn't respond to selected message
    else if( state < NUIControl::ECS_SELECTED )
        CUIControl::OnStateChange( rEvent );

}   // OnStateChange


/************************************************************************
*    desc:  Reset and recycle the contexts
************************************************************************/
void CUISubControl::Reset( bool complete )
{
    CUIControl::Reset( complete );

    for( size_t i = 0; i < m_pSubControlVec.size(); ++i )
        m_pSubControlVec[i]->Reset( complete );

}   // Reset


/************************************************************************
*    desc:  Handle the mouse move
************************************************************************/
bool CUISubControl::OnMouseMove( const SDL_Event & rEvent )
{
    bool result = CUIControl::OnMouseMove( rEvent );

    bool found = OnSubControlMouseMove( rEvent );

    // If the sub control is not found, deactivate them
    if( result && !found )
        DeactivateSubControl();

    return result || found;

}   // HandleMouseMove


/************************************************************************
*    desc:  Handle the sub control mouse move
************************************************************************/
bool CUISubControl::OnSubControlMouseMove( const SDL_Event & rEvent )
{
    bool result(false);

    for( size_t i = 0; i < m_pSubControlVec.size() && !result; ++i )
        result = m_pSubControlVec[i]->OnMouseMove( rEvent );

    return result;

}   // HandleSubControlMouseMove


/************************************************************************
*    desc:  Handle the select action
************************************************************************/
bool CUISubControl::HandleSelectAction( NDefs::EDeviceId deviceUsed, NMenu::EActionPress pressType )
{
    for( size_t i = 0; i < m_pSubControlVec.size(); ++i )
        if( m_pSubControlVec[i]->HandleSelectAction(deviceUsed, pressType) )
            return true;

    return false;

}   // HandleSelectAction


/************************************************************************
*    desc:  Get the pointer to the control if found
*
*    NOTE: This function is mainly for sub controls
************************************************************************/
CUIControl * CUISubControl::FindControl( const std::string & name )
{
    CUIControl * pCtrl = CUIControl::FindControl( name );

    if( pCtrl == nullptr )
        pCtrl = FindSubControl( name );

    return pCtrl;

}   // FindControl


/************************************************************************
*    desc:  Get the pointer to the subcontrol if found
*
*    NOTE: This function is mainly for sub controls
************************************************************************/
CUIControl * CUISubControl::FindSubControl( const std::string & name )
{
    CUIControl * pCtrl( nullptr );

    for( size_t i = 0; i < m_pSubControlVec.size() && (pCtrl == nullptr); ++i )
        pCtrl = m_pSubControlVec[i]->FindControl( name );

    return pCtrl;

}   // FindSubControl


/************************************************************************
*    desc:  Get the sub control via index
*  
*    ret:	CUIControl &
************************************************************************/
CUIControl * CUISubControl::GetSubControl( uint index )
{
    if( index >= m_pSubControlVec.size() )
        throw NExcept::CCriticalException("Index out of range",
            boost::str( boost::format("Index out of range of vector size (%d,%d).\n\n%s\nLine: %s") % index % m_pSubControlVec.size() % __FUNCTION__ % __LINE__ ));

    return m_pSubControlVec[index];

}	// GetSubControl


/************************************************************************
*    desc: Set the first inactive control to be active
*    NOTE: This is mainly here to be virtual for sub controls
************************************************************************/
bool CUISubControl::ActivateFirstInactiveControl()
{
    if( CUIControl::ActivateFirstInactiveControl() )
    {
        for( size_t i = 0; i < m_pSubControlVec.size(); ++i )
        {
            if( m_pSubControlVec[i]->ActivateFirstInactiveControl() )
            {
                m_pActiveNode = m_pControlNodeVec[i];
                break;
            }
        }

        return true;
    }

    return false;

}   // ActivateFirstInactiveControl


/************************************************************************
*    desc:  Deactivate the control
************************************************************************/
void CUISubControl::DeactivateControl()
{
    CUIControl::DeactivateControl();
    
    DeactivateSubControl();

}   // DeactivateControl


/************************************************************************
*    desc:  Deactivate the sub control
************************************************************************/
void CUISubControl::DeactivateSubControl()
{
    for( size_t i = 0; i < m_pSubControlVec.size(); ++i )
        m_pSubControlVec[i]->DeactivateControl();

}   // DeactivateSubControl
