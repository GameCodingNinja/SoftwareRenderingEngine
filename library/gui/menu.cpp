
/************************************************************************
*    FILE NAME:       menu.cpp
*
*    DESCRIPTION:     Class for user interface menu
************************************************************************/

// Physical component dependency
#include <gui/menu.h>

// Boost lib dependencies
#include <boost/format.hpp>

// Game lib dependencies
#include <utilities/xmlParser.h>
#include <utilities/exceptionhandling.h>
#include <utilities/deletefuncs.h>
#include <utilities/xmlparsehelper.h>
#include <utilities/settings.h>
#include <objectdata/objectdatamanager2d.h>
#include <utilities/genfunc.h>
#include <gui/menusprite.h>
#include <gui/menudefs.h>
#include <gui/uicontroldefs.h>
#include <gui/uilabel.h>
#include <gui/uicontrolfactory.h>

/************************************************************************
*    desc:  Constructer
************************************************************************/
CMenu::CMenu( const std::string & name )
    : m_name(name),
      m_state(NMenu::EMS_INACTIVE),
      m_focusGroupIndex(0)
{
    // The menu needs to default to not visible
    m_visible = false;

}   // constructor


/************************************************************************
*    desc:  destructer                                                             
************************************************************************/
CMenu::~CMenu()
{
    NDelFunc::DeleteVectorPointers(m_pStaticSpritesVec);
    NDelFunc::DeleteVectorPointers(m_pStaticControlVec);
    NDelFunc::DeleteVectorPointers(m_pMouseOnlyControlVec);

    for( size_t i = 0; i < m_pControlVecVec.size(); ++i )
    {
        NDelFunc::DeleteVectorPointers( m_pControlVecVec[i] );
        NDelFunc::DeleteVectorPointers( m_pControlNodeVecVec[i] );
    }

}   // destructer


/************************************************************************
*    desc:  Load the menu info from file
************************************************************************/
void CMenu::LoadFromXML( const std::string & filePath )
{
    // Open and parse the XML file:
    const XMLNode mainNode = XMLNode::openFileHelper( filePath.c_str(), "menu" );

    // Get the scrolling info
    const XMLNode scrollNode = mainNode.getChildNode( "scroll" );
    if( !scrollNode.isEmpty() )
    {
        m_scrollParam.SetStartDelay( std::atoi(scrollNode.getAttribute( "startDelay" )) );
        m_scrollParam.SetScrollDelay( std::atoi(scrollNode.getAttribute( "scrollDelay" )) );

        if( scrollNode.isAttributeSet( "up" ) && (std::string(scrollNode.getAttribute( "up" )) == "true") )
            m_scrollParam.AddMsg( NMenu::EGE_MENU_UP_ACTION );

        if( scrollNode.isAttributeSet( "down" ) && (std::string(scrollNode.getAttribute( "down" )) == "true") )
            m_scrollParam.AddMsg( NMenu::EGE_MENU_DOWN_ACTION );

        if( scrollNode.isAttributeSet( "left" ) && (std::string(scrollNode.getAttribute( "left" )) == "true") )
            m_scrollParam.AddMsg( NMenu::EGE_MENU_LEFT_ACTION );

        if( scrollNode.isAttributeSet( "right" ) && (std::string(scrollNode.getAttribute( "right" )) == "true") )
            m_scrollParam.AddMsg( NMenu::EGE_MENU_RIGHT_ACTION );
    }

    // Get the group name
    m_group = mainNode.getAttribute( "group" );

    // Get the static sprite groups
    const XMLNode staticSpritesNode = mainNode.getChildNode( "spriteList" );
    if( !staticSpritesNode.isEmpty() )
    {
        for( int i = 0; i < staticSpritesNode.nChildNode(); ++i )
        {
            XMLNode spriteNode = staticSpritesNode.getChildNode( "sprite", i );

            // Load in the control
            LoadStaticSpriteFromNode( spriteNode );
        }
    }

    // Get the static menu controls node
    XMLNode staticMenuControlsNode = mainNode.getChildNode( "staticMenuControls" );
    if( !staticMenuControlsNode.isEmpty() )
    {
        for( int i = 0; i < staticMenuControlsNode.nChildNode(); ++i )
        {
            // Get the menu controls
            XMLNode controlNode = staticMenuControlsNode.getChildNode( "control", i );

            // Load static controls from an XML node
            LoadStaticControlFromNode( controlNode );
        }
    }

    // Get the mouse only menu controls node
    XMLNode mouseOnlyMenuControlsNode = mainNode.getChildNode( "mouseOnlyControls" );
    if( !mouseOnlyMenuControlsNode.isEmpty() )
    {
        for( int i = 0; i < mouseOnlyMenuControlsNode.nChildNode(); ++i )
        {
            // Get the menu controls
            XMLNode controlNode = mouseOnlyMenuControlsNode.getChildNode( "control", i );

            // Load static controls from an XML node
            LoadMouseOnlyControlFromNode( controlNode );
        }
    }

    // Get the menu controls
    XMLNode menuControlsNode = mainNode.getChildNode( "menuControls" );
    if( !menuControlsNode.isEmpty() )
    {
        // map to help setup the node pointers
        NavHelperMap pNavNodeMap;

        for( int i = 0; i < menuControlsNode.nChildNode(); ++i )
        {
            // Get the menu controls
            XMLNode focusGroupNode = menuControlsNode.getChildNode( "focusGroup", i );
            if( !focusGroupNode.isEmpty() )
            {
                // Setup the vector holding the controls
                m_pControlVecVec.push_back( std::vector<CUIControl *>() );

                // Setup the vector holding the navigation nodes
                m_pControlNodeVecVec.push_back( std::vector<CUIControlNavNode *>() );

                // Push back the active control counter
                m_pActiveNodeVec.push_back( nullptr );

                for( int j = 0; j < focusGroupNode.nChildNode(); ++j )
                {
                    XMLNode controlNode = focusGroupNode.getChildNode( "control", j );

                    // Load in the control
                    LoadControlFromNode( controlNode, pNavNodeMap );
                }

                for( int j = 0; j < focusGroupNode.nChildNode(); ++j )
                {
                    XMLNode controlNode = focusGroupNode.getChildNode( "control", j );

                    // Find the reference nodes
                    FindNodes( controlNode, i, j, pNavNodeMap );
                }
            }
        }
    }

}   // LoadFromXML


/************************************************************************
*    desc:  Load a static sprite from an XML node
************************************************************************/
void CMenu::LoadStaticSpriteFromNode( const XMLNode & node )
{
    // Get the type of object
    std::string objectName = node.getAttribute( "objectName" );

    m_pStaticSpritesVec.push_back( new CMenuSprite( CObjectDataMgr2D::Instance().GetData( m_group, objectName )) );

    // Load the transform data
    m_pStaticSpritesVec.back()->LoadTransFromNode( node );

    // Init the script functions
    m_pStaticSpritesVec.back()->InitScriptFunctions( node );

}   // LoadStaticSpriteFromNode


/************************************************************************
*    desc:  Load static controls from an XML node
************************************************************************/
void CMenu::LoadStaticControlFromNode( XMLNode & node )
{
    // New up the control with its respected control type
    m_pStaticControlVec.push_back( NUIControlFactory::Create( node ) );

    // Load the transform data
    m_pStaticControlVec.back()->LoadTransFromNode( node );

    // Does this control have a name then add it to the map
    if( !m_pStaticControlVec.back()->GetName().empty() )
    {
        // Map of menu controls
        m_pControlMap.insert( std::make_pair(m_pStaticControlVec.back()->GetName(), m_pStaticControlVec.back()) );
    }

}   // LoadControlFromNode


/************************************************************************
*    desc:  Load mouse only controls from an XML node
************************************************************************/
void CMenu::LoadMouseOnlyControlFromNode( const XMLNode & node )
{
    // New up the control with its respected control type
    m_pMouseOnlyControlVec.push_back( NUIControlFactory::Create( node ) );

    // Load the transform data
    m_pMouseOnlyControlVec.back()->LoadTransFromNode( node );

}   // LoadMouseOnlyControlFromNode


/************************************************************************
*    desc:  Load a control from an XML node
************************************************************************/
void CMenu::LoadControlFromNode( XMLNode & node, NavHelperMap & pNavNodeMap )
{
    // Add the control to the vector
    m_pControlVecVec.back().push_back( NUIControlFactory::Create( node ) );

    // Load the transform data
    m_pControlVecVec.back().back()->LoadTransFromNode( node );

    // Get the control name
    const std::string controlName = m_pControlVecVec.back().back()->GetName();

    // Does this control have a name then add it to the map
    if( !controlName.empty() )
    {
        // Map of menu controls
        auto iter = m_pControlMap.insert( std::make_pair(controlName, m_pControlVecVec.back().back()) );

        // Check for duplicate names
        if( !iter.second )
        {
            throw NExcept::CCriticalException("Control Load Error!",
                boost::str( boost::format("Duplicate control name (%s).\n\n%s\nLine: %s") % controlName % __FUNCTION__ % __LINE__ ));
        }

        // Add a node to the vector with it's control
        m_pControlNodeVecVec.back().push_back( new CUIControlNavNode( m_pControlVecVec.back().back() ) );

        // Map of menu control nodes
        pNavNodeMap.insert( std::make_pair(controlName, m_pControlNodeVecVec.back().back()) );
    }

}   // LoadControlFromNode


/************************************************************************
*    desc:  Load the dynamic offset data from node
************************************************************************/
void CMenu::LoadDynamicOffsetFromNode( const XMLNode & node )
{
    // Load the dynamic offset
    m_dynamicOffset = NParseHelper::LoadDynamicOffset( node );

    // Set the dynamic position
    SetDynamicPos();

}   // LoadDynamicOffsetFromNode


/************************************************************************
*    desc:  Set the dynamic position
************************************************************************/
void CMenu::SetDynamicPos()
{
    // Position the menu based on the dynamic offset
    if( m_dynamicOffset.IsDynamicOffset() )
    {
        SetPos( m_dynamicOffset.GetDynamicPos( GetPos(), CSettings::Instance().GetDefaultSizeHalf() ) );
    }

}   // SetDynamicPos


/************************************************************************
*    desc:  Find the reference nodes
************************************************************************/
void CMenu::FindNodes(
    const XMLNode & node,
    int focusIndex,
    int nodeIndex,
    NavHelperMap & pNavNodeMap )
{
    const XMLNode navNode = node.getChildNode( "navigate" );
    if( !navNode.isEmpty() )
    {
        SetNodes( navNode, focusIndex, nodeIndex, "up", CUIControlNavNode::ENAV_NODE_UP, pNavNodeMap );
        SetNodes( navNode, focusIndex, nodeIndex, "down", CUIControlNavNode::ENAV_NODE_DOWN, pNavNodeMap );
        SetNodes( navNode, focusIndex, nodeIndex, "left", CUIControlNavNode::ENAV_NODE_LEFT, pNavNodeMap );
        SetNodes( navNode, focusIndex, nodeIndex, "right", CUIControlNavNode::ENAV_NODE_RIGHT, pNavNodeMap );
    }

}   // FindNodes


/************************************************************************
*    desc:  Find the reference nodes
************************************************************************/
void CMenu::SetNodes(
    const XMLNode & node,
    int focusIndex,
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
            m_pControlNodeVecVec[focusIndex][nodeIndex]->SetNode( navNode, iter->second );
        }
        else
        {
            throw NExcept::CCriticalException("Control Node Find Error!",
                boost::str( boost::format("Control node doesn't exist (%s).\n\n%s\nLine: %s") % name % __FUNCTION__ % __LINE__ ));
        }
    }

}   // SetNodes


/************************************************************************
*    desc:  Activate this menu because it's probably a root menu
************************************************************************/
void CMenu::ActivateMenu()
{
    m_state = NMenu::EMS_IDLE;
    SetVisible(true);
    ActivateFirstInactiveControl();

}   // ActivateMenu


/************************************************************************
*    desc:  Update the menu
************************************************************************/
void CMenu::Update()
{
    if( m_visible )
    {
        for( size_t i = 0; i < m_pStaticSpritesVec.size(); ++i )
            m_pStaticSpritesVec[i]->Update();

        for( size_t i = 0; i < m_pStaticControlVec.size(); ++i )
            m_pStaticControlVec[i]->Update();

        for( size_t i = 0; i < m_pMouseOnlyControlVec.size(); ++i )
            m_pMouseOnlyControlVec[i]->Update();

        for( size_t i = 0; i < m_pControlVecVec.size(); ++i )
            for( size_t j = 0; j < m_pControlVecVec[i].size(); ++j )
                m_pControlVecVec[i][j]->Update();
    }

}   // Update


/************************************************************************
*    desc:  Transform the menu
************************************************************************/
void CMenu::DoTransform()
{
    if( m_visible )
    {
        Transform();

        for( size_t i = 0; i < m_pStaticSpritesVec.size(); ++i )
            m_pStaticSpritesVec[i]->Transform( GetMatrix(), WasWorldPosTranformed() );

        for( size_t i = 0; i < m_pStaticControlVec.size(); ++i )
            m_pStaticControlVec[i]->DoTransform( *this );

        for( size_t i = 0; i < m_pMouseOnlyControlVec.size(); ++i )
            m_pMouseOnlyControlVec[i]->DoTransform( *this );

        for( size_t i = 0; i < m_pControlVecVec.size(); ++i )
            for( size_t j = 0; j < m_pControlVecVec[i].size(); ++j )
                m_pControlVecVec[i][j]->DoTransform( *this );
    }

}   // DoTransform


/************************************************************************
*    desc:  do the render                                                            
************************************************************************/
void CMenu::Render( const CMatrix & matrix )
{
    if( m_visible )
    {
        for( size_t i = 0; i < m_pStaticSpritesVec.size(); ++i )
            m_pStaticSpritesVec[i]->Render( matrix );

        for( size_t i = 0; i < m_pStaticControlVec.size(); ++i )
            m_pStaticControlVec[i]->Render( matrix );

        for( size_t i = 0; i < m_pMouseOnlyControlVec.size(); ++i )
            m_pMouseOnlyControlVec[i]->Render( matrix );

        for( size_t i = 0; i < m_pControlVecVec.size(); ++i )
            for( size_t j = 0; j < m_pControlVecVec[i].size(); ++j )
                m_pControlVecVec[i][j]->Render( matrix );
    }

}   // Render


/************************************************************************
*    desc:  Get the name of the menu
************************************************************************/
const std::string & CMenu::GetName() const
{
    return m_name;

}   // GetName


/************************************************************************
*    desc:  Handle events
************************************************************************/
void CMenu::HandleEvent( const SDL_Event & rEvent )
{
    // Have the controls handle events
    for( size_t i = 0; i < m_pControlVecVec.size(); ++i )
        for( size_t j = 0; j < m_pControlVecVec[i].size(); ++j )
            m_pControlVecVec[i][j]->HandleEvent( rEvent );

    for( size_t i = 0; i < m_pMouseOnlyControlVec.size(); ++i )
        m_pMouseOnlyControlVec[i]->HandleEvent( rEvent );

    if( rEvent.type == NMenu::EGE_MENU_TRANS_IN )
    {
        OnTransIn( rEvent );
    }
    else if( rEvent.type == NMenu::EGE_MENU_TRANS_OUT )
    {
        OnTransOut( rEvent );
    }
    else if( rEvent.type == NMenu::EGE_MENU_REACTIVATE )
    {
        OnReactivate( rEvent );
    }
    else if( m_state == NMenu::EMS_IDLE )
    {
        if( rEvent.type == NMenu::EGE_MENU_SELECT_ACTION )
        {
            OnSelectAction( rEvent );
        }
        else if( rEvent.type == NMenu::EGE_MENU_SET_ACTIVE_CONTROL )
        {
            OnSetActiveControl( rEvent );
        }
        else if( rEvent.type == NMenu::EGE_MENU_SCROLL_UP )
        {
            OnUpAction( rEvent );
        }
        else if( rEvent.type == NMenu::EGE_MENU_SCROLL_DOWN )
        {
            OnDownAction( rEvent );
        }
        else if( rEvent.type == NMenu::EGE_MENU_SCROLL_LEFT )
        {
            OnLeftAction( rEvent );
        }
        else if( rEvent.type == NMenu::EGE_MENU_SCROLL_RIGHT )
        {
            OnRightAction( rEvent );
        }
        else if( rEvent.type == SDL_MOUSEMOTION )
        {
            OnMouseMove( rEvent );
        }
        else if( (rEvent.type >= NMenu::EGE_MENU_UP_ACTION) &&
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
    }

}   // HandleEvent

/************************************************************************
*    desc:  Handle OnUpAction message
************************************************************************/
void CMenu::OnUpAction( const SDL_Event & rEvent )
{
    NavigateMenu( CUIControlNavNode::ENAV_NODE_UP );

}   // OnUpAction

/************************************************************************
*    desc:  Handle OnMenuDown message
************************************************************************/
void CMenu::OnDownAction( const SDL_Event & rEvent )
{
    NavigateMenu( CUIControlNavNode::ENAV_NODE_DOWN );

}   // OnDownAction

/************************************************************************
*    desc:  Handle OnMenuLeft message
************************************************************************/
void CMenu::OnLeftAction( const SDL_Event & rEvent )
{
    NavigateMenu( CUIControlNavNode::ENAV_NODE_LEFT );

}   // OnLeftAction

/************************************************************************
*    desc:  Handle OnRightAction message
************************************************************************/
void CMenu::OnRightAction( const SDL_Event & rEvent )
{
    NavigateMenu( CUIControlNavNode::ENAV_NODE_RIGHT );

}   // OnRightAction


/************************************************************************
*    desc:  Navigate the menu
************************************************************************/
void CMenu::NavigateMenu( CUIControlNavNode::ENavNode navNode )
{
    if( m_pActiveNodeVec[m_focusGroupIndex] != nullptr )
    {
        CUIControlNavNode * pNavNode = m_pActiveNodeVec[m_focusGroupIndex];
        CUIControlNavNode * pLastActiveNavNode = pNavNode;

        do
        {
            if( pNavNode->GetNode( navNode ) == nullptr )
                break;

            else
            {
                pNavNode = pNavNode->GetNode( navNode );

                if( !pNavNode->Get()->IsDisabled() )
                {
                    pLastActiveNavNode = pNavNode;

                    NGenFunc::DispatchEvent( 
                        NMenu::EGE_MENU_CONTROL_STATE_CHANGE,
                        NUIControl::ECS_ACTIVE,
                        &pNavNode->Get()->GetName() );
                    break;
                }
            }
        }
        while( true );

        m_pActiveNodeVec[m_focusGroupIndex] = pLastActiveNavNode;
    }

}   // NavigateMenu


/************************************************************************
*    desc:  Handle OnMouseMove message
************************************************************************/
void CMenu::OnMouseMove( const SDL_Event & rEvent )
{
    for( size_t i = 0; i < m_pControlVecVec.size(); ++i )
    {
        for( size_t j = 0; j < m_pControlVecVec[i].size(); ++j )
        {
            if( m_pControlVecVec[i][j]->OnMouseMove( rEvent ) )
                m_pActiveNodeVec[i] = m_pControlNodeVecVec[i][j];
            else
                m_pControlVecVec[i][j]->DeactivateControl();
        }
    }

    for( size_t i = 0; i < m_pMouseOnlyControlVec.size(); ++i )
        if( !m_pMouseOnlyControlVec[i]->OnMouseMove( rEvent ) )
            m_pMouseOnlyControlVec[i]->DeactivateControl();

}   // OnMouseMove


/************************************************************************
*    desc:  Handle OnSelectAction message
************************************************************************/
void CMenu::OnSelectAction( const SDL_Event & rEvent )
{
    // Unpack the data needed to be stored in the packed datatype
    NMenu::EActionPress pressType = NMenu::EActionPress( (rEvent.user.code >> 8) & 0xff );
    NDefs::EDeviceId deviceUsed = NDefs::EDeviceId( rEvent.user.code & 0xff );

    if( (m_pActiveNodeVec[m_focusGroupIndex] != nullptr) &&
        (m_pActiveNodeVec[m_focusGroupIndex]->Get()->HandleSelectAction(deviceUsed, pressType)) )
    {
        // Set the state to active which will block all messages until the state is reset to idle
        if( m_pActiveNodeVec[m_focusGroupIndex]->Get()->GetActionType() > NUIControl::ECAT_NULL )
            m_state = NMenu::EMS_ACTIVE;
    }
    else
    {
        // For mouse only controls
        for( size_t i = 0; i < m_pMouseOnlyControlVec.size(); ++i )
        {
            if( m_pMouseOnlyControlVec[i]->HandleSelectAction(deviceUsed, pressType) )
            {
                // Set the state to active which will block all messages until the state is reset to idle
                if( m_pMouseOnlyControlVec[i]->GetActionType() > NUIControl::ECAT_NULL )
                    m_state = NMenu::EMS_ACTIVE;

                break;
            }
        }
    }

}   // OnSelectAction

/************************************************************************
*    desc:  Handle OnSetActiveControl message
************************************************************************/
void CMenu::OnSetActiveControl( const SDL_Event & rEvent )
{
    // Set the first inactive control to active
    if( rEvent.user.code == NMenu::EAC_FIRST_ACTIVE_CONTROL )
        ActivateFirstInactiveControl();

}   // OnSetActiveControl

/************************************************************************
*    desc:  Handle OnReactivate message
************************************************************************/
void CMenu::OnReactivate( const SDL_Event & rEvent )
{
    m_state = NMenu::EMS_IDLE;

}   // OnReactivate

/************************************************************************
*    desc:  Handle OnTransIn message
************************************************************************/
void CMenu::OnTransIn( const SDL_Event & rEvent )
{
    if( rEvent.user.code == NMenu::ETC_BEGIN )
    {
        m_visible = true;
        m_state = NMenu::EMS_ACTIVE;

        for( size_t i = 0; i < m_pStaticSpritesVec.size(); ++i )
            m_pStaticSpritesVec[i]->Prepare(NUIControl::ESSF_TRANS_IN);
    }
    else if( rEvent.user.code == NMenu::ETC_END )
    {
        m_state = NMenu::EMS_IDLE;
    }

    // Have the static controls respond
    for( size_t i = 0; i < m_pStaticControlVec.size(); ++i )
        m_pStaticControlVec[i]->HandleEvent( rEvent );

}   // OnTransIn

/************************************************************************
*    desc:  Handle OnTransOut message
************************************************************************/
void CMenu::OnTransOut( const SDL_Event & rEvent )
{
    if( rEvent.user.code == NMenu::ETC_BEGIN )
    {
        m_state = NMenu::EMS_ACTIVE;

        for( size_t i = 0; i < m_pStaticSpritesVec.size(); ++i )
            m_pStaticSpritesVec[i]->Prepare(NUIControl::ESSF_TRANS_OUT);
    }
    else if( rEvent.user.code == NMenu::ETC_END )
    {
        m_state = NMenu::EMS_INACTIVE;
        m_visible = false;
    }

    // Have the static controls respond
    for( size_t i = 0; i < m_pStaticControlVec.size(); ++i )
        m_pStaticControlVec[i]->HandleEvent( rEvent );

}   // OnTransOut


/************************************************************************
*    desc:  Set the first inactive control to be active
************************************************************************/
void CMenu::ActivateFirstInactiveControl()
{
    bool found(false);

    for( size_t i = 0; i < m_pControlNodeVecVec[m_focusGroupIndex].size(); ++i )
    {
        if( !found && m_pControlVecVec[m_focusGroupIndex][i]->ActivateFirstInactiveControl() )
        {
            m_pActiveNodeVec[m_focusGroupIndex] = m_pControlNodeVecVec[m_focusGroupIndex][i];

            found = true;
        }
        else
        {
            m_pControlVecVec[m_focusGroupIndex][i]->DeactivateControl();
        }
    }

}   // ActivateFirstInactiveControl


/************************************************************************
*    desc:  Reset all controls
************************************************************************/
void CMenu::Reset()
{
    for( size_t i = 0; i < m_pControlVecVec.size(); ++i )
        for( size_t j = 0; j < m_pControlVecVec[i].size(); ++j )
            m_pControlVecVec[i][j]->Reset( true );

    for( size_t i = 0; i < m_pMouseOnlyControlVec.size(); ++i )
        m_pMouseOnlyControlVec[i]->Reset( true );

}   // Reset


/************************************************************************
*    desc:  Get the pointer to the control in question
************************************************************************/
CUIControl * CMenu::GetPtrToControl( const std::string & nameStr )
{
    // See if this material has already been loaded
    auto iter = m_pControlMap.find( nameStr );

    // Check that we have not over run the queue
    if(iter == m_pControlMap.end() )
        throw NExcept::CCriticalException("Menu Control Error!",
                    boost::str( boost::format("Control being asked for is missing!(%s)\n\n%s\nLine: %s") % nameStr.c_str() % __FUNCTION__ % __LINE__ ));

    // Pass back the pointer if found
    return iter->second;

}	// GetPtrToControl


/************************************************************************
*    desc:  Does this menu use dynamic offsets
************************************************************************/
bool CMenu::IsDynamicOffset()
{
    return m_dynamicOffset.IsDynamicOffset();

}   // IsDynamicOffset


/************************************************************************
*    desc:  Get the scroll params
************************************************************************/
CScrollParam & CMenu::GetScrollParam( int msg )
{
    if( (m_pActiveNodeVec[m_focusGroupIndex] != nullptr) &&
        m_pActiveNodeVec[m_focusGroupIndex]->Get()->IsActive() &&
        m_pActiveNodeVec[m_focusGroupIndex]->Get()->GetScrollParam().CanScroll(msg) )
    {
        return m_pActiveNodeVec[m_focusGroupIndex]->Get()->GetScrollParam();
    }

    return m_scrollParam;

}   // GetScrollParam
