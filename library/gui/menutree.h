
/************************************************************************
*    FILE NAME:       menutree.cpp
*
*    DESCRIPTION:     Class that hold a tree of menus
************************************************************************/

#ifndef __menu_tree_h__
#define __menu_tree_h__

// SDL/OpenGL lib dependencies
#include <SDL.h>

// Standard lib dependencies
#include <string>
#include <vector>
#include <map>

// Boost lib dependencies
#include <boost/noncopyable.hpp>

// Game lib dependencies
#include <common/defs.h>
#include <common/matrix.h>
#include <gui/menudefs.h>
#include <gui/scrollparam.h>

// Forward declaration(s)
struct XMLNode;
class CMenu;

class CMenuTree : public boost::noncopyable
{
public:

    // Constructor
    CMenuTree(
        std::map<const std::string, CMenu *> & pMenuMap, 
        const std::string & rootMenu,
        const std::string & defaultMenu );

    // Destructor
    ~CMenuTree();

    // Handle events
    void HandleEvent( const SDL_Event & rEvent );

    // Update the menu tree
    void Update();

    // Transform the menu tree
    void DoTransform();

    // do the render
    void Render( const CMatrix & matrix );

    // Is a menu active
    bool IsActive();

    // Does this tee have a root menu
    bool HasRootMenu();

    // Init the tree for use
    void Init();

    // Get the active menu
    CMenu * GetActiveMenu();

    // Get the scroll param data
    CScrollParam & GetScrollParam( int msg );

private:

    // Transition the menu
    void TransitionMenu();

    // Handle message
    void OnEscape( const SDL_Event & rEvent );
    void OnToggle( const SDL_Event & rEvent );
    void OnBack( const SDL_Event & rEvent );
    void OnToMenu( const SDL_Event & rEvent );
    void OnTransOut( const SDL_Event & rEvent );
    void OnTransIn( const SDL_Event & rEvent );

private:

    // Name of the root menu
    const std::string m_rootMenu;

    // Name of the default menu
    const std::string m_defaultMenu;

    // Name of menu we are transitioning to
    std::string m_toMenu;

    // Vector of the path taken through the menu
    std::vector<std::string> m_pathVector;

    // Map of the menus - does not own pointers
    std::map<const std::string, CMenu *> & m_pMenuMap;

    // menu tree state
    NMenu::EMenuTreeState m_state;

};

#endif  // __menu_tree_h__


