
/************************************************************************
*    FILE NAME:       uibuttonlist.cpp
*
*    DESCRIPTION:     Class for user interface buttons
************************************************************************/

// Physical component dependency
#include <gui/uibuttonlist.h>

// Game lib dependencies
#include <gui/menudefs.h>
#include <gui/uicontroldefs.h>
#include <gui/menusprite.h>
#include <utilities/genfunc.h>
#include <objectdata/objectdata2d.h>
#include <objectdata/objectvisualdata2d.h>
#include <common/defs.h>

/************************************************************************
*    desc:  Constructer
************************************************************************/
CUIButtonList::CUIButtonList()
    : m_activeIndex(0),
      m_imageLstIndex(-1)
{
    SetType( NUIControl::ECT_BUTTON_LIST );

}   // constructor


/************************************************************************
*    desc:  destructer                                                             
************************************************************************/
CUIButtonList::~CUIButtonList()
{
}	// destructer


/************************************************************************
*    desc:  Load the control specific info from XML node
************************************************************************/
void CUIButtonList::LoadControlFromNode( const XMLNode & controlNode )
{
    // Call the parent
    CUISubControl::LoadControlFromNode( controlNode );

    // See if there is an image list
    auto & pSpriteVec = GetSpriteVec();
    for( size_t i = 0; i < pSpriteVec.size(); ++i )
    {
        if( pSpriteVec[i]->GetObjectData().GetVisualData().GetFrameCount() > 1 )
        {
            m_imageLstIndex = i;
            break;
        }
    }

}   // LoadControlFromNode


/************************************************************************
*    desc:  Handle OnLeftAction message
************************************************************************/
void CUIButtonList::OnLeftAction( const SDL_Event & rEvent )
{
    // Send a message to blink the button
    NGenFunc::DispatchEvent( 
        NMenu::EGE_MENU_CONTROL_STATE_CHANGE,
        NUIControl::ECS_SELECTED,
        &m_pSubControlVec[NUIControl::BTN_DEC]->GetName() );

}   // OnLeftAction

/************************************************************************
*    desc:  Handle OnRightAction message
************************************************************************/
void CUIButtonList::OnRightAction( const SDL_Event & rEvent )
{
    // Send a message to blink the button
    NGenFunc::DispatchEvent( 
        NMenu::EGE_MENU_CONTROL_STATE_CHANGE,
        NUIControl::ECS_SELECTED,
        &m_pSubControlVec[NUIControl::BTN_INC]->GetName() );

}   // OnRightAction

/************************************************************************
*    desc:  Handle OnRightScroll message
************************************************************************/
void CUIButtonList::OnLeftScroll( const SDL_Event & rEvent )
{
    // Send a message to blink the button
    NGenFunc::DispatchEvent( 
        NMenu::EGE_MENU_CONTROL_STATE_CHANGE,
        NUIControl::ECS_SELECTED,
        &m_pSubControlVec[NUIControl::BTN_DEC]->GetName() );

}   // OnLeftScrol

/************************************************************************
*    desc:  Handle OnRightScroll message
************************************************************************/
void CUIButtonList::OnRightScroll( const SDL_Event & rEvent )
{
    // Send a message to blink the button
    NGenFunc::DispatchEvent( 
        NMenu::EGE_MENU_CONTROL_STATE_CHANGE,
        NUIControl::ECS_SELECTED,
        &m_pSubControlVec[NUIControl::BTN_INC]->GetName() );

}   // OnRightScroll

/************************************************************************
*    desc:  Handle OnStateChange message
************************************************************************/
void CUIButtonList::OnStateChange( const SDL_Event & rEvent )
{
    CUISubControl::OnStateChange( rEvent );

    NUIControl::EControlState state = NUIControl::EControlState(rEvent.user.code);
    
    if( state == NUIControl::ECS_SELECTED )
    {
        const std::string & ctrlName = *(std::string *)rEvent.user.data1;

        if( m_pSubControlVec[NUIControl::BTN_DEC]->GetName() == ctrlName )
        {
            // Dec the list
            DecList();

            // Update the display
            UpdateDisplay( m_activeIndex );

            // Execute smart gui
            ExecuteAction();
        }
        else if( m_pSubControlVec[NUIControl::BTN_INC]->GetName() == ctrlName )
        {
            // Inc the list
            IncList();

            // Update the display
            UpdateDisplay( m_activeIndex );

            // Execute smart gui
            ExecuteAction();
        }
    }

}   // OnStateChange


/************************************************************************
*    desc:  Inc the list
************************************************************************/
void CUIButtonList::IncList()
{
    m_activeIndex = (m_activeIndex + 1) % GetStringVec().size();

}   // IncList

void CUIButtonList::DecList()
{
    if( m_activeIndex > 0 )
        m_activeIndex = (m_activeIndex - 1) % GetStringVec().size();
    else
        m_activeIndex = static_cast<int>(GetStringVec().size()-1);

}   // DeList


/************************************************************************
*    desc:  Update the display
************************************************************************/
void CUIButtonList::UpdateDisplay( int index )
{
    m_activeIndex = index;

    CreateFontString( m_activeIndex );

    if( m_imageLstIndex > -1 )
        GetSpriteVec()[m_imageLstIndex]->GetVisualComponent().SetTextureID( m_activeIndex );

}  // UpdateDisplay


/************************************************************************
*    desc: Set the first inactive control to be active
*    NOTE: Don't want this functuality for the button list
************************************************************************/
bool CUIButtonList::ActivateFirstInactiveControl()
{
    return CUIControl::ActivateFirstInactiveControl();

}   // ActivateFirstInactiveControl


/************************************************************************
*    desc:  Get the active index
************************************************************************/
int CUIButtonList::GetActiveIndex()
{
    return m_activeIndex;

}   // GetActiveIndex