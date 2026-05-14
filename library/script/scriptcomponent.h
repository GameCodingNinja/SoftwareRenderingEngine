
/************************************************************************
*    FILE NAME:       scriptcomponent.h
*
*    DESCRIPTION:     Class for handling the scripting
************************************************************************/

#ifndef __script_component_h__
#define __script_component_h__

// Standard lib dependencies
#include <string>
#include <vector>

// Game lib dependencies
#include <common/defs.h>

// Forward declaration(s)
class asIScriptContext;

class CScriptComponent
{
public:

    // Constructor
    CScriptComponent();

    // Destructor
    ~CScriptComponent();

    // Update the script
    void Update();

    // Reset the contexts and recycle
    void ResetAndRecycle();

    // Is this component active?
    bool IsActive();

    // Prepare the script function to run
    void PrepareScript( const std::string & name, const std::string & group );

protected:

    // Get the current context
    asIScriptContext * GetContext();

private:

    // dynamic context vector
    // NOTE: This class does not own the pointer
    std::vector<asIScriptContext *> m_pContextVec;

};

#endif  // __script_component_2d_h__


