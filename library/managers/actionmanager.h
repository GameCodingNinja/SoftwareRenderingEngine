
/************************************************************************
*    FILE NAME:       actionmanager.h
*
*    DESCRIPTION:     Class for handling action mapping
************************************************************************/

#ifndef __action_manager_h__
#define __action_manager_h__

// SDL/OpenGL lib dependencies
#include <SDL.h>

// Standard lib dependencies
#include <string>
#include <vector>
#include <array>
#include <map>

// Game lib dependencies
#include <utilities/xmlParser.h>
#include <common/keybuttonaction.h>
#include <common/defs.h>
#include <common/point.h>
#include <gui/menudefs.h>

class CActionManager
{
public:

    // Get the instance of the singleton class
    static CActionManager & Instance()
    {
        static CActionManager actionMgr;
        return actionMgr;
    }

    // Load the action mappings from xml
    void LoadActionFromXML( const std::string & filePath );

    // Was this an action
    bool WasAction( const SDL_Event & rEvent, const std::string & actionStr, NMenu::EActionPress );
    NMenu::EActionPress WasAction( const SDL_Event & rEvent, const std::string & actionStr );

    // What was the last devic
    bool WasLastDeviceGamepad();
    bool WasLastDeviceKeyboard();
    bool WasLastDeviceMouse();
    NDefs::EDeviceId GetLastDeviceUsed();

    // Get the last mouse position
    const CPoint<float> & GetLastMousePos() const;

private:

    // Constructor
    CActionManager();

    // Destructor
    ~CActionManager();

    // Load action data from xml node
    void LoadActionFromNode(
        const XMLNode & node, 
        std::map< const std::string, char > & xmlParseMap,
        std::map< const std::string, CKeyButtonAction > & actionMap );

    // Load the keyboard/mouse/gamepad mapping from node
    void LoadKeyboardMappingFromNode( const XMLNode & node );
    void LoadMouseMappingFromNode( const XMLNode & node );
    void LoadGamepadMappingFromNode( const XMLNode & node );

    // Was this an action
    bool WasAction( 
        const char Id,
        const std::string & actionStr,
        const std::map< const std::string, CKeyButtonAction > & actionMap );

private:

    enum
    {
        ANALOG1_UP = SDL_CONTROLLER_BUTTON_MAX,
        ANALOG1_DOWN,
        ANALOG1_LEFT,
        ANALOG1_RIGHT,
        ANALOG_STICK_MSG_MAX = 20000,
    };

    // Maps for parsing codes
    std::map< const std::string, char > m_keyboardParseMap;
    std::map< const std::string, char > m_mouseParseMap;
    std::map< const std::string, char > m_gamepadParseMap;

    // Action maps
    std::map< const std::string, CKeyButtonAction > m_keyboardActionMap;
    std::map< const std::string, CKeyButtonAction > m_mouseActionMap;
    std::map< const std::string, CKeyButtonAction > m_gamepadActionMap;

    // xml node
    XMLNode mainNode;

    // Last device used
    NDefs::EDeviceId m_lastDeviceUsed;

    // Last mouse pos
    CPoint<float> m_lastMousePos;

    // Flag to indicate analog state as button
    std::array<NMenu::EActionPress,16> m_analogXButtonStateAry;
    std::array<NMenu::EActionPress,16> m_analogYButtonStateAry;

};

#endif  // __action_manager_h__


