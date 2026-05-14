
/************************************************************************
*    FILE NAME:       menusprite.h
*
*    DESCRIPTION:     Class template
************************************************************************/

#ifndef __classtemplate_h__
#define __classtemplate_h__

// Physical component dependency
#include <2d/sprite2d.h>

// Standard lib dependencies
#include <string>
#include <map>

// Game lib dependencies
#include <gui/uicontroldefs.h>

// Forward declaration(s)
struct XMLNode;

class CMenuSprite : public CSprite2D
{
public:

    // Constructor
    CMenuSprite( const CObjectData2D & objectData );

    // Destructor
    virtual ~CMenuSprite();

    // Init the script functions
    void InitScriptFunctions( const XMLNode & node );

    // Prepare the script function to run
    void Prepare( NUIControl::ESpriteScriptFunctions scriptFunc );

private:

    std::map< int, std::string > m_scriptFunction;

};

#endif  // __classtemplate_h__


