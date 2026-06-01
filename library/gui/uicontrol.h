
/************************************************************************
*    FILE NAME:       uicontrol.h
*
*    DESCRIPTION:     Stub class for user interface controls.
*                     Minimal interface for CNode payload support.
*                     Will be fleshed out when the GUI system is
*                     implemented.
************************************************************************/

#pragma once

// Physical component dependency
#include <common/object.h>

// Standard lib dependencies
#include <string>

// Forward declaration(s)
class CCamera;
struct XMLNode;

class CUIControl : public CObject
{
public:

    // Constructor
    CUIControl( const std::string & group );

    // Destructor
    virtual ~CUIControl();

    // Load the initial info from XML node
    virtual void loadFromNode( const XMLNode & node );

    // Init control
    virtual void init();

    // Update the control
    virtual void update();

    // do the render
    virtual void render( const CCamera & camera );

protected:

    // Group this control belongs to
    std::string m_group;
};
