
/************************************************************************
*    FILE NAME:       menumanager.h
*
*    DESCRIPTION:     menu manager class singleton
************************************************************************/

#ifndef __menu_manager_h__
#define __menu_manager_h__

// SDL/OpenGL lib dependencies
#include <SDL.h>

// Standard lib dependencies
#include <string>
#include <vector>
#include <map>

// Game lib dependencies
#include <common/matrix.h>
#include <gui/menudefs.h>

// Forward declaration(s)
class CMenu;
class CMenuTree;

class CMenuManager
{
public:

    // Get the instance of the singleton class
    static CMenuManager & Instance()
    {
        static CMenuManager menuMgr;
        return menuMgr;
    }

    // Activate a tree to be used
    void ActivateTree( const std::string & treeStr );

    // Clear the active trees
    void ClearActiveTrees();

    // Load the menu info from file
    void LoadFromXML( const std::string & filePath );

    // Handle input events and dispatch menu events
    void HandleEvent( const SDL_Event & rEvent );

    // Update the menu
    void Update();

    // Transform the menu
    void Transform();

    // do the render
    void Render( const CMatrix & matrix );

    // Get the pointer to the menu in question
    CMenu * GetPtrToMenu( const std::string & nameStr );

    // Is this menu system active
    bool IsActive();

    // Reset the world transform
    void ResetWorldTransform();

    // Reset the dynamic positions of menus
    void ResetDynamicOffset();

private:

    // Constructor
    CMenuManager();

    // Destructor
    ~CMenuManager();

    // Load the from XML file
    void LoadMenusFromXML( const std::string & filePath );

    // Set the active state
    void SetActiveState();

private:

    // Timer call back function
    static Uint32 ScrollTimerCallbackFunc( Uint32 interval, void *param );

private:

    // Map of menu trees
    std::map<const std::string, CMenuTree *> m_pMenuTreeMap;

    // Map of the menus
    std::map<const std::string, CMenu *> m_pMenuMap;

    // Vector of active trees
    std::vector<std::string> m_activeTreeVec;

    // menu manager state
    bool m_active;

    // Actions
    std::string m_backAction;
    std::string m_toggleAction;
    std::string m_escapeAction;
    std::string m_selectAction;
    std::string m_upAction;
    std::string m_downAction;
    std::string m_leftAction;
    std::string m_rightAction;

    // timer Id
    SDL_TimerID m_scrollTimerId;

};

#endif  // __menu_manager_h__

