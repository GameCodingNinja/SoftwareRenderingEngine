
/************************************************************************
*    FILE NAME:       actionmanager.cpp
*
*    DESCRIPTION:     Class for handling action mapping
************************************************************************/

// Physical component dependency
#include <managers/actionmanager.h>

// Boost lib dependencies
#include <boost/format.hpp>

// Game lib dependencies
#include <utilities/exceptionhandling.h>
#include <utilities/genfunc.h>

/************************************************************************
*    desc:  Constructer
************************************************************************/
CActionManager::CActionManager()
    : m_lastDeviceUsed(NDefs::DEVICE_NULL)
{
    m_keyboardParseMap.insert( std::make_pair( "---",  NDefs::UNKNOWN_ACTION ) );

    m_keyboardParseMap.insert( std::make_pair( "RETURN",  SDLK_RETURN ) );
    m_keyboardParseMap.insert( std::make_pair( "ESCAPE",  SDLK_ESCAPE ) );

    m_keyboardParseMap.insert( std::make_pair( "0",  SDLK_0 ) );
    m_keyboardParseMap.insert( std::make_pair( "1",  SDLK_1 ) );
    m_keyboardParseMap.insert( std::make_pair( "2",  SDLK_2 ) );
    m_keyboardParseMap.insert( std::make_pair( "3",  SDLK_3 ) );
    m_keyboardParseMap.insert( std::make_pair( "4",  SDLK_4 ) );
    m_keyboardParseMap.insert( std::make_pair( "5",  SDLK_5 ) );
    m_keyboardParseMap.insert( std::make_pair( "6",  SDLK_6 ) );
    m_keyboardParseMap.insert( std::make_pair( "7",  SDLK_7 ) );
    m_keyboardParseMap.insert( std::make_pair( "8",  SDLK_8 ) );
    m_keyboardParseMap.insert( std::make_pair( "9",  SDLK_9 ) );

    m_keyboardParseMap.insert( std::make_pair( "A",  SDLK_a ) );
    m_keyboardParseMap.insert( std::make_pair( "B",  SDLK_b ) );
    m_keyboardParseMap.insert( std::make_pair( "C",  SDLK_c ) );
    m_keyboardParseMap.insert( std::make_pair( "D",  SDLK_d ) );
    m_keyboardParseMap.insert( std::make_pair( "E",  SDLK_e ) );
    m_keyboardParseMap.insert( std::make_pair( "F",  SDLK_f ) );
    m_keyboardParseMap.insert( std::make_pair( "G",  SDLK_g ) );
    m_keyboardParseMap.insert( std::make_pair( "H",  SDLK_h ) );
    m_keyboardParseMap.insert( std::make_pair( "I",  SDLK_i ) );
    m_keyboardParseMap.insert( std::make_pair( "J",  SDLK_j ) );
    m_keyboardParseMap.insert( std::make_pair( "K",  SDLK_k ) );
    m_keyboardParseMap.insert( std::make_pair( "L",  SDLK_l ) );
    m_keyboardParseMap.insert( std::make_pair( "M",  SDLK_m ) );
    m_keyboardParseMap.insert( std::make_pair( "N",  SDLK_n ) );
    m_keyboardParseMap.insert( std::make_pair( "O",  SDLK_o ) );
    m_keyboardParseMap.insert( std::make_pair( "P",  SDLK_p ) );
    m_keyboardParseMap.insert( std::make_pair( "Q",  SDLK_q ) );
    m_keyboardParseMap.insert( std::make_pair( "R",  SDLK_r ) );
    m_keyboardParseMap.insert( std::make_pair( "S",  SDLK_s ) );
    m_keyboardParseMap.insert( std::make_pair( "T",  SDLK_t ) );
    m_keyboardParseMap.insert( std::make_pair( "U",  SDLK_u ) );
    m_keyboardParseMap.insert( std::make_pair( "V",  SDLK_v ) );
    m_keyboardParseMap.insert( std::make_pair( "W",  SDLK_w ) );
    m_keyboardParseMap.insert( std::make_pair( "X",  SDLK_x ) );
    m_keyboardParseMap.insert( std::make_pair( "Y",  SDLK_y ) );
    m_keyboardParseMap.insert( std::make_pair( "Z",  SDLK_z ) );

    m_keyboardParseMap.insert( std::make_pair( "F1",  SDLK_F1 ) );
    m_keyboardParseMap.insert( std::make_pair( "F2",  SDLK_F2 ) );
    m_keyboardParseMap.insert( std::make_pair( "F3",  SDLK_F3 ) );
    m_keyboardParseMap.insert( std::make_pair( "F4",  SDLK_F4 ) );
    m_keyboardParseMap.insert( std::make_pair( "F5",  SDLK_F5 ) );
    m_keyboardParseMap.insert( std::make_pair( "F6",  SDLK_F6 ) );
    m_keyboardParseMap.insert( std::make_pair( "F7",  SDLK_F7 ) );
    m_keyboardParseMap.insert( std::make_pair( "F8",  SDLK_F8 ) );
    m_keyboardParseMap.insert( std::make_pair( "F9",  SDLK_F9 ) );
    m_keyboardParseMap.insert( std::make_pair( "F10", SDLK_F10 ) );
    m_keyboardParseMap.insert( std::make_pair( "F11", SDLK_F11 ) );
    m_keyboardParseMap.insert( std::make_pair( "F12", SDLK_F12 ) );

    m_keyboardParseMap.insert( std::make_pair( "NUMPAD 1", SDLK_KP_1 ) );
    m_keyboardParseMap.insert( std::make_pair( "NUMPAD 2", SDLK_KP_2 ) );
    m_keyboardParseMap.insert( std::make_pair( "NUMPAD 3", SDLK_KP_3 ) );
    m_keyboardParseMap.insert( std::make_pair( "NUMPAD 4", SDLK_KP_4 ) );
    m_keyboardParseMap.insert( std::make_pair( "NUMPAD 5", SDLK_KP_5 ) );
    m_keyboardParseMap.insert( std::make_pair( "NUMPAD 6", SDLK_KP_6 ) );
    m_keyboardParseMap.insert( std::make_pair( "NUMPAD 7", SDLK_KP_7 ) );
    m_keyboardParseMap.insert( std::make_pair( "NUMPAD 8", SDLK_KP_8 ) );
    m_keyboardParseMap.insert( std::make_pair( "NUMPAD 9", SDLK_KP_9 ) );
    m_keyboardParseMap.insert( std::make_pair( "NUMPAD 0", SDLK_KP_0 ) );

    m_keyboardParseMap.insert( std::make_pair( "NUM LOCK",     SDLK_NUMLOCKCLEAR ) );
    m_keyboardParseMap.insert( std::make_pair( "NUMPAD /",     SDLK_KP_DIVIDE ) );
    m_keyboardParseMap.insert( std::make_pair( "NUMPAD *",     SDLK_KP_MULTIPLY ) );
    m_keyboardParseMap.insert( std::make_pair( "NUMPAD -",     SDLK_KP_MINUS ) );
    m_keyboardParseMap.insert( std::make_pair( "NUMPAD +",     SDLK_KP_PLUS ) );
    m_keyboardParseMap.insert( std::make_pair( "NUMPAD ENTER", SDLK_KP_ENTER ) );

    m_keyboardParseMap.insert( std::make_pair( "LEFT CTRL",   SDLK_LCTRL ) );
    m_keyboardParseMap.insert( std::make_pair( "RIGHT CTRL",  SDLK_RCTRL ) );
    m_keyboardParseMap.insert( std::make_pair( "LEFT SHIFT",  SDLK_LSHIFT ) );
    m_keyboardParseMap.insert( std::make_pair( "RIGHT SHIFT", SDLK_RSHIFT ) );
    m_keyboardParseMap.insert( std::make_pair( "LEFT ALT",    SDLK_LALT ) );
    m_keyboardParseMap.insert( std::make_pair( "RIGHT ALT",   SDLK_RALT ) );

    m_keyboardParseMap.insert( std::make_pair( "END",       SDLK_END ) );
    m_keyboardParseMap.insert( std::make_pair( "INSERT",    SDLK_INSERT ) );
    m_keyboardParseMap.insert( std::make_pair( "DELETE",    SDLK_DELETE ) );
    m_keyboardParseMap.insert( std::make_pair( "HOME",      SDLK_HOME ) );
    m_keyboardParseMap.insert( std::make_pair( "PAGE UP",   SDLK_PAGEUP ) );
    m_keyboardParseMap.insert( std::make_pair( "PAGE DOWN", SDLK_PAGEDOWN ) );

    m_keyboardParseMap.insert( std::make_pair( "BACK",   SDLK_BACKSPACE ) );
    m_keyboardParseMap.insert( std::make_pair( "TAB",    SDLK_TAB ) );
    m_keyboardParseMap.insert( std::make_pair( "SPACE",  SDLK_SPACE ) );
    m_keyboardParseMap.insert( std::make_pair( ",",      SDLK_COMMA ) );
    m_keyboardParseMap.insert( std::make_pair( "-",      SDLK_MINUS ) );
    m_keyboardParseMap.insert( std::make_pair( ".",      SDLK_PERIOD ) );
    m_keyboardParseMap.insert( std::make_pair( "/",      SDLK_SLASH ) );

    m_keyboardParseMap.insert( std::make_pair( ";",  SDLK_SEMICOLON ) );
    m_keyboardParseMap.insert( std::make_pair( "[",  SDLK_LEFTBRACKET ) );
    m_keyboardParseMap.insert( std::make_pair( "\\", SDLK_BACKSLASH ) );
    m_keyboardParseMap.insert( std::make_pair( "]",  SDLK_RIGHTBRACKET ) );
    m_keyboardParseMap.insert( std::make_pair( "`",  SDLK_BACKQUOTE ) );

    m_keyboardParseMap.insert( std::make_pair( "ARROW UP",    SDLK_UP ) );
    m_keyboardParseMap.insert( std::make_pair( "ARROW DOWN",  SDLK_DOWN ) );
    m_keyboardParseMap.insert( std::make_pair( "ARROW LEFT",  SDLK_LEFT ) );
    m_keyboardParseMap.insert( std::make_pair( "ARROW RIGHT", SDLK_RIGHT ) );

    m_mouseParseMap.insert( std::make_pair( "---",            NDefs::UNKNOWN_ACTION ) );
    m_mouseParseMap.insert( std::make_pair( "LEFT MOUSE",     SDL_BUTTON_LEFT ) );
    m_mouseParseMap.insert( std::make_pair( "RIGHT MOUSE",    SDL_BUTTON_MIDDLE ) );
    m_mouseParseMap.insert( std::make_pair( "MIDDLE MOUSE",   SDL_BUTTON_RIGHT ) );
    m_mouseParseMap.insert( std::make_pair( "MOUSE 1",        SDL_BUTTON_X1 ) );
    m_mouseParseMap.insert( std::make_pair( "MOUSE 2",        SDL_BUTTON_X2 ) ); 

    m_gamepadParseMap.insert( std::make_pair( "---",          NDefs::UNKNOWN_ACTION ) );
    m_gamepadParseMap.insert( std::make_pair( "A",            SDL_CONTROLLER_BUTTON_A ) );
    m_gamepadParseMap.insert( std::make_pair( "B",            SDL_CONTROLLER_BUTTON_B ) );
    m_gamepadParseMap.insert( std::make_pair( "X",            SDL_CONTROLLER_BUTTON_X ) );
    m_gamepadParseMap.insert( std::make_pair( "Y",            SDL_CONTROLLER_BUTTON_Y ) );
    m_gamepadParseMap.insert( std::make_pair( "BACK",         SDL_CONTROLLER_BUTTON_BACK ) );
    m_gamepadParseMap.insert( std::make_pair( "GUIDE",        SDL_CONTROLLER_BUTTON_GUIDE ) );
    m_gamepadParseMap.insert( std::make_pair( "START",        SDL_CONTROLLER_BUTTON_START ) );
    m_gamepadParseMap.insert( std::make_pair( "UP",           SDL_CONTROLLER_BUTTON_DPAD_UP ) );
    m_gamepadParseMap.insert( std::make_pair( "RIGHT",        SDL_CONTROLLER_BUTTON_DPAD_RIGHT ) );
    m_gamepadParseMap.insert( std::make_pair( "DOWN",         SDL_CONTROLLER_BUTTON_DPAD_DOWN ) );
    m_gamepadParseMap.insert( std::make_pair( "LEFT",         SDL_CONTROLLER_BUTTON_DPAD_LEFT ) );
    m_gamepadParseMap.insert( std::make_pair( "L STICK",      SDL_CONTROLLER_BUTTON_LEFTSTICK ) );
    m_gamepadParseMap.insert( std::make_pair( "R STICK",      SDL_CONTROLLER_BUTTON_RIGHTSTICK ) );
    m_gamepadParseMap.insert( std::make_pair( "L BUMPER",     SDL_CONTROLLER_BUTTON_LEFTSHOULDER ) );
    m_gamepadParseMap.insert( std::make_pair( "R BUMPER",     SDL_CONTROLLER_BUTTON_RIGHTSHOULDER ) );

    m_gamepadParseMap.insert( std::make_pair( "L STICK UP",    ANALOG1_UP ) );
    m_gamepadParseMap.insert( std::make_pair( "L STICK DOWN",  ANALOG1_DOWN ) );
    m_gamepadParseMap.insert( std::make_pair( "L STICK LEFT",  ANALOG1_LEFT ) );
    m_gamepadParseMap.insert( std::make_pair( "L STICK RIGHT", ANALOG1_RIGHT ) );

}   // constructor


/************************************************************************
*    desc:  destructer                                                             
************************************************************************/
CActionManager::~CActionManager()
{
}	// destructer


/************************************************************************
*    desc:  Load the action mappings from xml
*  
*    param: std::string filePath - path to file
************************************************************************/
void CActionManager::LoadActionFromXML( const std::string & filePath )
{
    mainNode = XMLNode::openFileHelper( filePath.c_str(), "controllerMapping" );

    // Load the keyboard/mouse/gamepad mapping
    LoadKeyboardMappingFromNode( mainNode.getChildNode( "keyboardMapping" ) );
    LoadMouseMappingFromNode( mainNode.getChildNode( "mouseMapping" ) );
    LoadGamepadMappingFromNode( mainNode.getChildNode( "gamepadMapping" ) );

}	// LoadActionFromXML


/************************************************************************
*    desc:  Load the keyboard mapping from node
************************************************************************/
void CActionManager::LoadKeyboardMappingFromNode( const XMLNode & node )
{
    // Load the player hidden controls
    LoadActionFromNode( node.getChildNode("playerHidden"), m_keyboardParseMap, m_keyboardActionMap );

    // Load the player visible controls
    LoadActionFromNode( node.getChildNode("playerVisible"), m_keyboardParseMap, m_keyboardActionMap );

}   // LoadKeyboardMappingFromNode


/************************************************************************
*    desc:  Load the mouse mapping from node
************************************************************************/
void CActionManager::LoadMouseMappingFromNode( const XMLNode & node )
{
    // Load the player hidden controls
    LoadActionFromNode( node.getChildNode("playerHidden"), m_mouseParseMap, m_mouseActionMap );

    // Load the player visible controls
    LoadActionFromNode( node.getChildNode("playerVisible"), m_mouseParseMap, m_mouseActionMap );

}   // LoadMouseMappingFromNode


/************************************************************************
*    desc:  Load the gamepad mapping from node
************************************************************************/
void CActionManager::LoadGamepadMappingFromNode( const XMLNode & node )
{
    // Load the player hidden controls
    LoadActionFromNode( node.getChildNode("playerHidden"), m_gamepadParseMap, m_gamepadActionMap );

    // Load the player visible controls
    LoadActionFromNode( node.getChildNode("playerVisible"), m_gamepadParseMap, m_gamepadActionMap );

}   // LoadGamepadMappingFromNode


/************************************************************************
*    desc:  Load action data from xml node
************************************************************************/
void CActionManager::LoadActionFromNode( 
    const XMLNode & node, 
    std::map< const std::string, char > & xmlParseMap,
    std::map< const std::string, CKeyButtonAction > & actionMap )
{
    if( !node.isEmpty() )
    {
        std::map< const std::string, char >::iterator xmlParseIter;

        for( int i = 0; i < node.nChildNode(); ++i )
        {
            XMLNode actionNode = node.getChildNode( "actionMap", i );

            // See if we can find the string that represents the com id
            xmlParseIter = xmlParseMap.find( actionNode.getAttribute( "componetId" ) );

            // Add it in if we found it
            if( xmlParseIter != xmlParseMap.end() && (xmlParseIter->second != NDefs::UNKNOWN_ACTION) )
            {
                std::string actionStr = actionNode.getAttribute( "action" );

                // See if the controller action string has already been added
                auto iter = actionMap.find( actionStr );

                if( iter != actionMap.end() )
                {
                    // If it's found, add another id to this map
                    iter->second.SetId( xmlParseIter->second );
                }
                else
                {
                    // Add new action to the map
                    actionMap.insert( make_pair(actionStr, CKeyButtonAction(xmlParseIter->second)) );
                }
            }
        }
    }
}	// LoadActionFromXMLNode


/************************************************************************
*    desc:  Was this an action
************************************************************************/
bool CActionManager::WasAction( const SDL_Event & rEvent, const std::string & actionStr, NMenu::EActionPress actionPress )
{
    if( WasAction( rEvent, actionStr ) == actionPress )
        return true;

    return false;

}   // WasAction


/************************************************************************
*    desc:  Was this an action
************************************************************************/
NMenu::EActionPress CActionManager::WasAction( const SDL_Event & rEvent, const std::string & actionStr )
{
    NMenu::EActionPress result(NMenu::EAP_IDLE);

    if( (rEvent.type == SDL_CONTROLLERBUTTONDOWN) || (rEvent.type == SDL_CONTROLLERBUTTONUP) )
    {
        m_lastDeviceUsed = NDefs::GAMEPAD;

        if( WasAction( rEvent.cbutton.button, actionStr, m_gamepadActionMap ) )
        {
            result = NMenu::EAP_UP;

            if( rEvent.type == SDL_CONTROLLERBUTTONDOWN )
            {
                result = NMenu::EAP_DOWN;
            }
        }
    }
    else if( ((rEvent.type == SDL_KEYDOWN) || (rEvent.type == SDL_KEYUP)) && (rEvent.key.repeat == 0) )
    {
        m_lastDeviceUsed = NDefs::KEYBOARD;

        if( WasAction( rEvent.key.keysym.sym, actionStr, m_keyboardActionMap ) )
        {
            result = NMenu::EAP_UP;

            if( rEvent.type == SDL_KEYDOWN )
            {
                result = NMenu::EAP_DOWN;
            }
        }
    }
    else if( (rEvent.type == SDL_MOUSEBUTTONDOWN) || (rEvent.type == SDL_MOUSEBUTTONUP) )
    {
        m_lastDeviceUsed = NDefs::MOUSE;

        if( WasAction( rEvent.button.button, actionStr, m_mouseActionMap ) )
        {
            result = NMenu::EAP_UP;

            m_lastMousePos.x = rEvent.button.x;
            m_lastMousePos.y = rEvent.button.y;

            if( rEvent.type == SDL_MOUSEBUTTONDOWN )
            {
                result = NMenu::EAP_DOWN;
            }
        }
    }
    else if( rEvent.type == SDL_CONTROLLERAXISMOTION )
    {
        if( rEvent.caxis.axis == SDL_CONTROLLER_AXIS_LEFTX )
        {
            if( m_analogXButtonStateAry[rEvent.caxis.which] == NMenu::EAP_IDLE )
            {
                if( (rEvent.caxis.value < -ANALOG_STICK_MSG_MAX) && WasAction( ANALOG1_LEFT, actionStr, m_gamepadActionMap ) )
                    result = NMenu::EAP_DOWN;

                else if( (rEvent.caxis.value > ANALOG_STICK_MSG_MAX) && WasAction( ANALOG1_RIGHT, actionStr, m_gamepadActionMap ) )
                    result = NMenu::EAP_DOWN;
            }
            else if( m_analogXButtonStateAry[rEvent.caxis.which] == NMenu::EAP_DOWN )
            {
                if( (rEvent.caxis.value > -ANALOG_STICK_MSG_MAX) && (rEvent.caxis.value < 0) && WasAction( ANALOG1_LEFT, actionStr, m_gamepadActionMap ) )
                    result = NMenu::EAP_UP;

                else if( (rEvent.caxis.value < ANALOG_STICK_MSG_MAX) && (rEvent.caxis.value > 0) && WasAction( ANALOG1_RIGHT, actionStr, m_gamepadActionMap ) )
                    result = NMenu::EAP_UP;
            }

            if( result != NMenu::EAP_IDLE )
            {
                if( result == NMenu::EAP_DOWN )
                    m_analogXButtonStateAry[rEvent.caxis.which] = NMenu::EAP_DOWN;
                else
                    m_analogXButtonStateAry[rEvent.caxis.which] = NMenu::EAP_IDLE;

                m_lastDeviceUsed = NDefs::GAMEPAD;
            }
        }
        else if( rEvent.caxis.axis == SDL_CONTROLLER_AXIS_LEFTY )
        {
            if( m_analogYButtonStateAry[rEvent.caxis.which] == NMenu::EAP_IDLE )
            {
                if( (rEvent.caxis.value < -ANALOG_STICK_MSG_MAX) && WasAction( ANALOG1_UP, actionStr, m_gamepadActionMap ) )
                    result = NMenu::EAP_DOWN;

                else if( (rEvent.caxis.value > ANALOG_STICK_MSG_MAX) && WasAction( ANALOG1_DOWN, actionStr, m_gamepadActionMap ) )
                    result = NMenu::EAP_DOWN;
            }
            else if( m_analogYButtonStateAry[rEvent.caxis.which] == NMenu::EAP_DOWN )
            {
                if( (rEvent.caxis.value > -ANALOG_STICK_MSG_MAX) && (rEvent.caxis.value < 0) && WasAction( ANALOG1_UP, actionStr, m_gamepadActionMap ) )
                    result = NMenu::EAP_UP;

                else if( (rEvent.caxis.value < ANALOG_STICK_MSG_MAX) && (rEvent.caxis.value > 0) && WasAction( ANALOG1_DOWN, actionStr, m_gamepadActionMap ) )
                    result = NMenu::EAP_UP;
            }

            if( result != NMenu::EAP_IDLE )
            {
                if( result == NMenu::EAP_DOWN )
                    m_analogYButtonStateAry[rEvent.caxis.which] = NMenu::EAP_DOWN;
                else
                    m_analogYButtonStateAry[rEvent.caxis.which] = NMenu::EAP_IDLE;

                m_lastDeviceUsed = NDefs::GAMEPAD;
            }
        }
    }

    return result;

}	// WasAction


/************************************************************************
*    desc:  Was this an action
************************************************************************/
bool CActionManager::WasAction( 
    const char Id,
    const std::string & actionStr,
    const std::map< const std::string, CKeyButtonAction > & actionMap )
{
    bool result(false);

    // See if the action has already been added
    auto iter = actionMap.find( actionStr );

    // If it's found, see if this is the correct action
    if( iter != actionMap.end() )
    {
        result = iter->second.WasAction( Id );
    }

    return result;

}   // WasAction


/************************************************************************
*    desc:  What was the last device
************************************************************************/
bool CActionManager::WasLastDeviceGamepad()
{
    return (m_lastDeviceUsed == NDefs::GAMEPAD);
}

bool CActionManager::WasLastDeviceKeyboard()
{
    return (m_lastDeviceUsed == NDefs::KEYBOARD);
}

bool CActionManager::WasLastDeviceMouse()
{
    return (m_lastDeviceUsed == NDefs::MOUSE);
}

NDefs::EDeviceId CActionManager::GetLastDeviceUsed()
{
    return m_lastDeviceUsed;
}


/************************************************************************
*    desc:  Get the last mouse position
************************************************************************/
const CPoint<float> & CActionManager::GetLastMousePos() const
{
    return m_lastMousePos;
}
