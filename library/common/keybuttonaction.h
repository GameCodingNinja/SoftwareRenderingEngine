
/************************************************************************
*    FILE NAME:       keybuttonaction.h
*
*    DESCRIPTION:     Template class to convert the state of a button
*                     or key press into an up, down or hold state
*                     for the given action
************************************************************************/

#ifndef __key_button_action_h__
#define __key_button_action_h__

// Standard lib dependencies
#include <vector>

class CKeyButtonAction
{
public:
    CKeyButtonAction(char id)
    {
        m_id.push_back(id);
    }

    // Get the id the action is associated with
    char GetId( unsigned int index = 0 )
    {
        return m_id[index];
    }

    // Set the second id
    void SetId( char id )
    {
        m_id.push_back(id);
    }

    // Set the second id
    bool WasAction( const char id ) const
    {
        for( size_t i = 0; i < m_id.size(); ++i )
        {
            if( m_id[i] == id )
                return true;
        }

        return false;
    }

    // Get Id count?
    size_t GetIdCount() const
    {
        return m_id.size();
    }

protected:

    // Controller specific type
    std::vector<char> m_id;
};

#endif  // __key_button_action_h__


