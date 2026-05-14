
/************************************************************************
*    FILE NAME:       menumanager.cpp
*
*    DESCRIPTION:     menu manager class singleton
************************************************************************/

// Physical component dependency
#include <gui/menumanager.h>

// Standard lib dependencies

// Boost lib dependencies
#include <boost/format.hpp>

// Game lib dependencies
#include <common/defs.h>
#include <utilities/xmlParser.h>
#include <utilities/exceptionhandling.h>
#include <utilities/deletefuncs.h>
#include <utilities/genfunc.h>
#include <gui/menutree.h>
#include <gui/menu.h>
#include <gui/scrollparam.h>
#include <managers/actionmanager.h>

/************************************************************************
*    desc:  Constructer
************************************************************************/
CMenuManager::CMenuManager() :
    m_active(false),
    m_scrollTimerId(0)
{
}   // constructor


/************************************************************************
*    desc:  destructer                                                             
************************************************************************/
CMenuManager::~CMenuManager()
{
    SDL_RemoveTimer( m_scrollTimerId );

    NDelFunc::DeleteMapPointers( m_pMenuMap );
    NDelFunc::DeleteMapPointers( m_pMenuTreeMap );

}	// destructer


/************************************************************************
*    desc:  Activate a tree to be used
************************************************************************/
void CMenuManager::ActivateTree( const std::string & treeStr )
{
    if( m_pMenuTreeMap.find(treeStr) != m_pMenuTreeMap.end() )
    {
        if( std::find( m_activeTreeVec.begin(), m_activeTreeVec.end(), treeStr ) == m_activeTreeVec.end())
        {
            m_active = false;

            // Find the tree in the map
            auto iter = m_pMenuTreeMap.find( treeStr );
            if( iter != m_pMenuTreeMap.end() )
            {
                if( iter->second->HasRootMenu() )
                    m_active = true;

                // Init the tree for use
                iter->second->Init();
                m_activeTreeVec.push_back( treeStr );
            }
        }
    }
    else
    {
        throw NExcept::CCriticalException("Menu Tree Acticate Error!",
            boost::str( boost::format("Menu tree doesn't exist (%s).\n\n%s\nLine: %s") % treeStr % __FUNCTION__ % __LINE__ ));
    }

}   // ActivateTree


/************************************************************************
*    desc:  Clear the active trees
************************************************************************/
void CMenuManager::ClearActiveTrees()
{
    SDL_RemoveTimer( m_scrollTimerId );

    m_activeTreeVec.clear();

}   // ClearActiveTrees


/************************************************************************
*    desc:  Load the menu info from file
************************************************************************/
void CMenuManager::LoadFromXML( const std::string & filePath )
{
    // open and parse the XML file:
    const XMLNode mainNode = XMLNode::openFileHelper( filePath.c_str(), "menuTreeList" );

    // Load the menus from xml file
    LoadMenusFromXML( mainNode.getAttribute( "file" ) );

    const XMLNode actionNode = mainNode.getChildNode( "action" );

    m_backAction = actionNode.getAttribute( "backAction" );
    m_toggleAction = actionNode.getAttribute( "toggleAction" );
    m_escapeAction = actionNode.getAttribute( "escapeAction" );
    m_selectAction = actionNode.getAttribute( "selectAction" );
    m_upAction = actionNode.getAttribute( "upAction" );
    m_downAction = actionNode.getAttribute( "downAction" );
    m_leftAction = actionNode.getAttribute( "leftAction" );
    m_rightAction = actionNode.getAttribute( "rightAction" );

    for( int i = 0; i < mainNode.nChildNode("tree"); ++i )
    {
        // Get the tree node
        const XMLNode treeNode = mainNode.getChildNode("tree", i);

        // Get the name
        const std::string name = treeNode.getAttribute( "name" );

        // Get the root menu
        const std::string rootMenu = treeNode.getAttribute( "root" );

        // Get the default menu
        const std::string defaultMenu = treeNode.getAttribute( "default" );

        // Add the tree data to the map
        auto iter = m_pMenuTreeMap.insert( std::make_pair(name, new CMenuTree(m_pMenuMap, rootMenu, defaultMenu)) );

        // Check for duplicate names
        if( !iter.second )
        {
            throw NExcept::CCriticalException("Menu Tree Load Error!",
                boost::str( boost::format("Duplicate tree name (%s).\n\n%s\nLine: %s") % name % __FUNCTION__ % __LINE__ ));
        }

        // Check that the root menu exists
        if( !rootMenu.empty() )
        {
            if( m_pMenuMap.find(rootMenu) == m_pMenuMap.end() )
            {
                throw NExcept::CCriticalException("Menu Load Error!",
                    boost::str( boost::format("Root menu doesn't exist (%s - %s).\n\n%s\nLine: %s") % rootMenu % filePath % __FUNCTION__ % __LINE__ ));
            }
        }
    }

}   // LoadFromXML


/************************************************************************
*    desc:  Load the menus from XML file
************************************************************************/
void CMenuManager::LoadMenusFromXML( const std::string & filePath )
{
    // open and parse the XML file:
    const XMLNode mainNode = XMLNode::openFileHelper( filePath.c_str(), "menuTree" );

    for( int i = 0; i < mainNode.nChildNode("menu"); ++i )
    {
        // Menu name and file path 
        const XMLNode menuNode = mainNode.getChildNode( "menu", i );

        // Get the name of the menu
        const std::string name = menuNode.getAttribute( "name" );

        // Insert the menu into the menu map
        auto iter = m_pMenuMap.insert( std::make_pair(name, new CMenu(name)) );

        // Check for duplicate names
        if( !iter.second )
        {
            throw NExcept::CCriticalException("Menu Load Error!",
                boost::str( boost::format("Duplicate menu name (%s).\n\n%s\nLine: %s") % name % __FUNCTION__ % __LINE__ ));
        }

        // Load the transform data from node
        iter.first->second->LoadTransFromNode( menuNode );

        // Load the dynamic offset data from node
        iter.first->second->LoadDynamicOffsetFromNode( menuNode );

        // Have the menu load it's share
        iter.first->second->LoadFromXML( menuNode.getAttribute( "file" ) );
    }

}   // LoadFromNode


/************************************************************************
*    desc:  Handle input events and dispatch menu events
************************************************************************/
void CMenuManager::HandleEvent( const SDL_Event & rEvent )
{
    if( (rEvent.type > SDL_SYSWMEVENT) && (rEvent.type < SDL_FINGERDOWN) )
    {
        // Message testing code
        /*if( rEvent.type == SDL_CONTROLLERBUTTONDOWN )
            NGenFunc::PostDebugMsg( boost::str( boost::format("Controller Button: %d") % ((int)rEvent.cbutton.button) ) );
        else if( rEvent.type == SDL_JOYBUTTONDOWN )
            NGenFunc::PostDebugMsg( boost::str( boost::format("Joystick Button: %d") % ((int)rEvent.cbutton.button) ) );
        else if( rEvent.type == SDL_MOUSEBUTTONDOWN )
            NGenFunc::PostDebugMsg( boost::str( boost::format("Mouse Button: %d") % ((int)rEvent.button.button) ) );
        else if( rEvent.type == SDL_KEYDOWN )
            NGenFunc::PostDebugMsg( boost::str( boost::format("Key Button: %d") % ((int)rEvent.key.keysym.sym) ) );
        else if( rEvent.type == SDL_MOUSEMOTION )
            NGenFunc::PostDebugMsg( boost::str( boost::format("Mouse Motion: x = %d, y = %d") % ((int)rEvent.motion.x) % ((int)rEvent.motion.y) ) );
        else if( rEvent.type == SDL_CONTROLLERAXISMOTION )
        {
            if( rEvent.caxis.axis == SDL_CONTROLLER_AXIS_LEFTX )
                NGenFunc::PostDebugMsg( boost::str( boost::format("CAxis Left: x = %d") % ((int)rEvent.caxis.value) ) );
            else if( rEvent.caxis.axis == SDL_CONTROLLER_AXIS_LEFTY )
                NGenFunc::PostDebugMsg( boost::str( boost::format("CAxis Left: y = %d") % ((int)rEvent.caxis.value) ) );
            if( rEvent.caxis.axis == SDL_CONTROLLER_AXIS_RIGHTX )
                NGenFunc::PostDebugMsg( boost::str( boost::format("CAxis Right: x = %d") % ((int)rEvent.caxis.value) ) );
            else if( rEvent.caxis.axis == SDL_CONTROLLER_AXIS_RIGHTY )
                NGenFunc::PostDebugMsg( boost::str( boost::format("CAxis Right: y = %d") % ((int)rEvent.caxis.value) ) );
            else if( rEvent.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERLEFT )
                NGenFunc::PostDebugMsg( boost::str( boost::format("CTrigger Left: %d") % ((int)rEvent.caxis.value) ) );
            else if( rEvent.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERRIGHT )
                NGenFunc::PostDebugMsg( boost::str( boost::format("CTrigger Right: %d") % ((int)rEvent.caxis.value) ) );
        }
        else if( rEvent.type == SDL_JOYAXISMOTION )
        {
            if( rEvent.jaxis.axis == SDL_CONTROLLER_AXIS_LEFTX )
                NGenFunc::PostDebugMsg( boost::str( boost::format("Joystick Left: x = %d") % ((int)rEvent.jaxis.value) ) );
            else if( rEvent.jaxis.axis == SDL_CONTROLLER_AXIS_LEFTY )
                NGenFunc::PostDebugMsg( boost::str( boost::format("Joystick Left: y = %d") % ((int)rEvent.jaxis.value) ) );
        }*/
        
        if( CActionManager::Instance().WasAction( rEvent, m_escapeAction, NMenu::EAP_DOWN ) )
            NGenFunc::DispatchEvent( NMenu::EGE_MENU_ESCAPE_ACTION );

        else if( CActionManager::Instance().WasAction( rEvent, m_toggleAction, NMenu::EAP_DOWN ) )
            NGenFunc::DispatchEvent( NMenu::EGE_MENU_TOGGLE_ACTION );

        else if( IsActive() )
        {
            NMenu::EActionPress pressType;
            if( rEvent.type == SDL_MOUSEMOTION )
            {
                if( m_activeTreeVec.size() == 1 )
                    m_pMenuTreeMap.find( m_activeTreeVec.back() )->second->HandleEvent( rEvent );
            }
            else if( (pressType = CActionManager::Instance().WasAction( rEvent, m_selectAction )) > NMenu::EAP_IDLE ) // Need to pack multiple data into one 32-bit int for this message
                NGenFunc::DispatchEvent( NMenu::EGE_MENU_SELECT_ACTION, (pressType << 8) | CActionManager::Instance().GetLastDeviceUsed() );

            else if( CActionManager::Instance().WasAction( rEvent, m_backAction, NMenu::EAP_DOWN ) )
                NGenFunc::DispatchEvent( NMenu::EGE_MENU_BACK_ACTION );

            else if( (pressType = CActionManager::Instance().WasAction( rEvent, m_upAction )) > NMenu::EAP_IDLE )
                NGenFunc::DispatchEvent( NMenu::EGE_MENU_UP_ACTION, pressType );

            else if( (pressType = CActionManager::Instance().WasAction( rEvent, m_downAction )) > NMenu::EAP_IDLE )
                NGenFunc::DispatchEvent( NMenu::EGE_MENU_DOWN_ACTION, pressType );

            else if( (pressType = CActionManager::Instance().WasAction( rEvent, m_leftAction )) > NMenu::EAP_IDLE )
                NGenFunc::DispatchEvent( NMenu::EGE_MENU_LEFT_ACTION, pressType );

            else if( (pressType = CActionManager::Instance().WasAction( rEvent, m_rightAction )) > NMenu::EAP_IDLE )
                NGenFunc::DispatchEvent( NMenu::EGE_MENU_RIGHT_ACTION, pressType );
        }
    }
    else if( rEvent.type > SDL_USEREVENT )
    {
        // Are we doing menu actions? May need to do some scrolling
        if( (rEvent.type >= NMenu::EGE_MENU_UP_ACTION) && (rEvent.type <= NMenu::EGE_MENU_RIGHT_ACTION) )
        {
            // Free a timer if one happens to be running
            SDL_RemoveTimer(m_scrollTimerId);
            m_scrollTimerId = 0;

            if( rEvent.user.code == NMenu::EAP_DOWN )
            {
                CScrollParam & rScrollParam = m_pMenuTreeMap.find( m_activeTreeVec.back() )->second->GetScrollParam( rEvent.type );

                // If scrolling is allowed, start the timer
                if( rScrollParam.CanScroll( rEvent.type ) )
                    m_scrollTimerId = SDL_AddTimer(rScrollParam.GetStartDelay(), ScrollTimerCallbackFunc, &rScrollParam);
            }
        }

        if( m_activeTreeVec.size() == 1 )
            m_pMenuTreeMap.find( m_activeTreeVec.back() )->second->HandleEvent( rEvent );

        // Set the active state
        SetActiveState();
    }

}   // HandleEvent


/************************************************************************
*    desc:  Update the menu
************************************************************************/
void CMenuManager::Update()
{
    if( m_active )
    {
        for( size_t i = 0; i < m_activeTreeVec.size(); ++i )
            m_pMenuTreeMap.find( m_activeTreeVec[i] )->second->Update();
    }

}   // Update


/************************************************************************
*    desc:  Transform the menu
************************************************************************/
void CMenuManager::Transform()
{
    if( m_active )
    {
        for( size_t i = 0; i < m_activeTreeVec.size(); ++i )
            m_pMenuTreeMap.find( m_activeTreeVec[i] )->second->DoTransform();
    }

}   // Transform


/************************************************************************
*    desc:  do the render                                                            
************************************************************************/
void CMenuManager::Render( const CMatrix & matrix )
{
    if( m_active )
    {
        for( size_t i = 0; i < m_activeTreeVec.size(); ++i )
            m_pMenuTreeMap.find( m_activeTreeVec[i] )->second->Render( matrix );
    }

}	// Render


/************************************************************************
*    desc:  Is this menu system active?
************************************************************************/
bool CMenuManager::IsActive()
{
    return m_active;

}   // IsActive


/************************************************************************
*    desc:  Set the active state
************************************************************************/
void CMenuManager::SetActiveState()
{
    m_active = false;

    for( size_t i = 0; i < m_activeTreeVec.size(); ++i )
    {
        if( m_pMenuTreeMap.find( m_activeTreeVec[i] )->second->IsActive() )
        {
            m_active = true;
            break;
        }
    }

}   // SetActiveState


/************************************************************************
*    desc:  Get the pointer to the menu in question
************************************************************************/
CMenu * CMenuManager::GetPtrToMenu( const std::string & nameStr )
{
    // Get a pointer to the menu
    auto iter = m_pMenuMap.find( nameStr );

    // Pass back the pointer if found
    if( iter != m_pMenuMap.end() )
        return iter->second;

    throw NExcept::CCriticalException("UIMenu Error!",
        boost::str( boost::format("Menu being asked for is missing (%s).\n\n%s\nLine: %s") % nameStr.c_str() % __FUNCTION__ % __LINE__ ));

    return NULL;

}	// GetPtrToMenu


/************************************************************************
*    desc:  Reset the world transform
************************************************************************/
void CMenuManager::ResetWorldTransform()
{
    for( auto iter = m_pMenuMap.begin();
         iter != m_pMenuMap.end();
         ++iter )
    {
        iter->second->ForceWorldTransform();
    }

}   // ResetWorldTransform


/************************************************************************
*    desc:  Reset the dynamic positions of menus
************************************************************************/
void CMenuManager::ResetDynamicOffset()
{
    for( auto iter = m_pMenuMap.begin();
         iter != m_pMenuMap.end();
         ++iter )
    {
        iter->second->SetDynamicPos();
    }

}	// ResetDynamicOffset


/************************************************************************
*    desc:  Timer call back function for 
************************************************************************/
Uint32 CMenuManager::ScrollTimerCallbackFunc( Uint32 interval, void *param )
{
    CScrollParam * pScrollParam = static_cast<CScrollParam *>(param);

    NGenFunc::DispatchEvent( pScrollParam->GetMsg() );

    return pScrollParam->GetScrollDelay();

}   // ScrollTimerCallbackFunc

