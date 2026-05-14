
/************************************************************************
*    FILE NAME:       scriptglobals.h
*
*    DESCRIPTION:     AngelScript global function registration
************************************************************************/

#ifndef __script_globals_h__
#define __script_globals_h__

// Forward declaration(s)
class asIScriptEngine;

namespace NScriptGlobals
{
    // Register Script CColor Object
    void Register( asIScriptEngine * pEngine );
}

#endif  // __script_globals_h__


