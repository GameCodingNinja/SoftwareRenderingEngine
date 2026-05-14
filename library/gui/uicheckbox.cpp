
/************************************************************************
*    FILE NAME:       uicheckbox.cpp
*
*    DESCRIPTION:     Class for user interface check box buttons
************************************************************************/

// Physical component dependency
#include <gui/uicheckbox.h>

// Game lib dependencies
#include <gui/menusprite.h>

/************************************************************************
*    desc:  Constructer
************************************************************************/
CUICheckBox::CUICheckBox()
    : m_toggleState(false)
{
    SetType( NUIControl::ECT_CHECK_BOX );

}   // constructor


/************************************************************************
*    desc:  destructer                                                             
************************************************************************/
CUICheckBox::~CUICheckBox()
{
}	// destructer


/************************************************************************
*    desc:  Set the Select state of this control
*  
*    param: EControlState value - control state
************************************************************************/
void CUICheckBox::SetToggleState( bool value )
{
    m_toggleState = value;

}	// SetSelectState


/************************************************************************
*    desc:  Get the Select state of this control
*  
*    ret: EControlState - control state
************************************************************************/
bool CUICheckBox::GetToggleState()
{
    return m_toggleState;

}	// GetSelectState

/************************************************************************
*    desc:  Handle OnSelectExecute message
************************************************************************/
void CUICheckBox::OnSelectExecute( const SDL_Event & rEvent )
{
    if( GetState() == NUIControl::ECS_SELECTED )
        m_toggleState = !m_toggleState;

    CUIControl::OnSelectExecute( rEvent );

}   // OnSelectExecute


/************************************************************************
*    desc:  Render the control
************************************************************************/
void CUICheckBox::Render( const CMatrix & matrix )
{
    const auto & pSpriteVec = GetSpriteVec();

    for( size_t i = 0; i < pSpriteVec.size()-1; ++i )
        pSpriteVec[i]->Render( matrix );

    if( m_toggleState == NUIControl::TOGGLE_STATE_ON )
        pSpriteVec.back()->Render( matrix );

}	// Render



