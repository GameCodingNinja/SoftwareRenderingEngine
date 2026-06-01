
/************************************************************************
*    FILE NAME:       uicontrol.cpp
*
*    DESCRIPTION:     Stub class for user interface controls
************************************************************************/

// Physical component dependency
#include <gui/uicontrol.h>

/************************************************************************
*    DESC:  Constructor / Destructor
************************************************************************/
CUIControl::CUIControl( const std::string & group ) :
    m_group(group)
{
}

CUIControl::~CUIControl()
{
}

/************************************************************************
*    DESC:  Load the initial info from XML node
************************************************************************/
void CUIControl::loadFromNode( const XMLNode & node )
{
}

/************************************************************************
*    DESC:  Init control
************************************************************************/
void CUIControl::init()
{
}

/************************************************************************
*    DESC:  Update the control
************************************************************************/
void CUIControl::update()
{
}

/************************************************************************
*    DESC:  do the render
************************************************************************/
void CUIControl::render( const CCamera & camera )
{
}
