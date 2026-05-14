
/************************************************************************
*    FILE NAME:       scrollparam.h
*
*    DESCRIPTION:     Class for handling scroll parameter data
************************************************************************/

#ifndef __scroll_param_h__
#define __scroll_param_h__

// Standard lib dependencies
#include <vector>
#include <algorithm>

class CScrollParam
{
public:

    // Constructor
    CScrollParam() : 
        m_startDelay(-1),
        m_scrollDelay(-1), 
        m_msg(-1)
    {}

    // Clear out the data
    void Clear()
    {
        m_startDelay = -1;
        m_scrollDelay = -1;
        m_msg = -1;
        m_scrollTypesVec.clear();
    }

    // Does this menu or control support scrolling this message?
    bool CanScroll( int msg )
    {
        m_msg = -1;

        if( m_scrollTypesVec.empty() )
            return false;

        if( std::find(m_scrollTypesVec.begin(), m_scrollTypesVec.end(), msg) != m_scrollTypesVec.end() )
        {
            if( msg == NMenu::EGE_MENU_UP_ACTION )
                m_msg = NMenu::EGE_MENU_SCROLL_UP;

            else if( msg == NMenu::EGE_MENU_DOWN_ACTION )
                m_msg = NMenu::EGE_MENU_SCROLL_DOWN;

            else if( msg == NMenu::EGE_MENU_LEFT_ACTION )
                m_msg = NMenu::EGE_MENU_SCROLL_LEFT;

            else if( msg == NMenu::EGE_MENU_RIGHT_ACTION )
                m_msg = NMenu::EGE_MENU_SCROLL_RIGHT;

            return true;
        }

        return false;
    }

    // Get/Set Start delay
    int GetStartDelay()
    { return m_startDelay; }

    void SetStartDelay( int value )
    { m_startDelay = value; }

    // Get/Set scroll delay
    int GetScrollDelay()
    { return m_scrollDelay; }

    void SetScrollDelay( int value )
    { m_scrollDelay = value; }

    // Add supported messages
    void AddMsg( int msg )
    { m_scrollTypesVec.push_back(msg); }

    // Get the message
    int GetMsg()
    { return m_msg; }

private:

    // Vector that holds the scroll messages allows by this menu or control
    std::vector<int> m_scrollTypesVec;

    // The delay of the first scroll message
    int m_startDelay;

    // The delay of the rest of the scroll messages
    int m_scrollDelay;

    // The scroll message to send from the timer
    int m_msg;

};

#endif  // __scroll_param_h__


