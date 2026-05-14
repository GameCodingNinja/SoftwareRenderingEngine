
/************************************************************************
*    FILE NAME:       uiscrollbox.cpp
*
*    DESCRIPTION:     Class for user interface scroll boxes
************************************************************************/

// Physical component dependency
#include <gui/uiscrollbox.h>

// Standard lib dependencies

// Boost lib dependencies

// Game lib dependencies
#include <2d/sprite2d.h>
#include <common/size.h>
#include <utilities/xmlParser.h>
#include <utilities/xmlparsehelper.h>
#include <utilities/deletefuncs.h>
#include <utilities/genfunc.h>
#include <gui/uicontrolfactory.h>
#include <gui/uislider.h>
#include <objectdata/objectdatamanager2d.h>
#include <objectdata/objectdata2d.h>

/************************************************************************
*    desc:  Constructer
************************************************************************/
CUIScrollBox::CUIScrollBox() :
    m_cullHeight(0),
    m_controlHeight(0),
    m_scrollCurPos(0),
    m_visibleCount(0),
    m_visStartPos(0),
    m_visEndPos(0),
    m_maxMoveAmount(0),
    m_activeScrollCtrl(NUIControl::NO_ACTIVE_CONTROL),
    m_lastActiveScrollCtrl(NUIControl::NO_ACTIVE_CONTROL),
    m_firstScrollCtrlIndex(0)
{
    SetType( NUIControl::ECT_SCROLL_BOX );

}   // constructor


/************************************************************************
*    desc:  destructer                                                             
************************************************************************/
CUIScrollBox::~CUIScrollBox()
{
    NDelFunc::DeleteVectorPointers(m_pScrollControlVec);

}   // destructer


/************************************************************************
*    desc:  Load the control info from XML node
*  
*    param: node - XML node
************************************************************************/
void CUIScrollBox::LoadFromNode( const XMLNode & node )
{
    CUISubControl::LoadFromNode( node );
    
    // Init the slider
    NGenFunc::DynCast<CUISlider *>(GetSubControl(NUIControl::SCROLL_BAR))->SetMaxValue(m_maxMoveAmount);
    NGenFunc::DynCast<CUISlider *>(GetSubControl(NUIControl::SCROLL_BAR))->SetSlider();
    
    // Calc the start and end positions of what should
    // be viewable in the scroll box
    SetStartEndPos();
    
}   // LoadFromNode


/************************************************************************
*    desc:  Load the control specific info from XML node
************************************************************************/
void CUIScrollBox::LoadControlFromNode( const XMLNode & controlNode )
{
    // Call the parent
    CUISubControl::LoadControlFromNode( controlNode );

    // Get the menu controls node
    const XMLNode menuControlsNode = controlNode.getChildNode( "scrollBoxControlList" );
    if( !menuControlsNode.isEmpty() )
    {
        // Get the initial offset of the first control in the scroll box
        m_initialOffset = NParseHelper::LoadPosition( menuControlsNode );

        // Get the scroll boc info node
        const XMLNode controlInfoNode = menuControlsNode.getChildNode( "controlInfo" );
        m_controlHeight = std::atoi( controlInfoNode.getAttribute( "height" ) );
        m_visibleCount = std::atoi( controlInfoNode.getAttribute( "visibleInScrollBox" ) );

        // Get the number of controls in this scroll box
        const int scrollControlCount = menuControlsNode.nChildNode( "control" );

        for( int i = 0; i < scrollControlCount; ++i )
        {
            // Add the scroll control from node
            AddScrollControlFromNode( menuControlsNode.getChildNode( "control", i ) );
        }
    }

    // Get the stencil mask node
    const XMLNode stencilMaskNode = controlNode.getChildNode( "stencilMask" );
    if( !stencilMaskNode.isEmpty() )
    {
        const std::string objectName = stencilMaskNode.getAttribute( "objectName" );

        m_stencilMaskSprite.reset( new CSprite2D( CObjectDataMgr2D::Instance().GetData( GetGroup(), objectName ) ) );

        // Get the cull height
        m_cullHeight = (m_stencilMaskSprite->GetObjectData().GetSize().GetW() + m_controlHeight) / 2;

        // Load the transform data
        m_stencilMaskSprite->LoadTransFromNode( controlNode );
    }

}   // LoadControlFromNode


/************************************************************************
*    desc:  Add the scroll control from node
*           NOTE: This function recalulates the scroll box members because
*                 it is also used for run-time dynamic scroll boxes
*  
*    param: const XMLNode & controlNode - XML node
************************************************************************/
CUIControl * CUIScrollBox::AddScrollControlFromNode( const XMLNode & controlNode )
{
    // The pointer is placed within a vector for all controls
    m_pScrollControlVec.push_back( NUIControlFactory::Create( controlNode ) );
    
    // Get the position for this control
    CPoint<float> pos( m_initialOffset - CPoint<float>(0, m_controlHeight * (m_pScrollControlVec.size()-1), 0) );
    
    // Record the default y offset
    m_defaultOffset.push_back( pos.y );
    
    // Set the position
    m_pScrollControlVec.back()->SetPos( pos );

    // Init the control visual state
    m_pScrollControlVec.back()->DeactivateControl();

    // Calculate the maximum scroll amount in pixels
    if( m_pScrollControlVec.size() > static_cast<uint>(m_visibleCount) )
        m_maxMoveAmount = (static_cast<int>(m_pScrollControlVec.size()) - m_visibleCount) * m_controlHeight;

    return m_pScrollControlVec.back();

}   // AddControlFromNode


/************************************************************************
*    desc:  Handle events
************************************************************************/
void CUIScrollBox::HandleEvent( const SDL_Event & rEvent )
{
    // Call the parent
    CUISubControl::HandleEvent( rEvent );

    for( int i = m_visStartPos; i < m_visEndPos; ++i )
        m_pScrollControlVec[i]->HandleEvent( rEvent );
    
}   // HandleEvent


/************************************************************************
*    desc:  Handle OnUpAction message
************************************************************************/
void CUIScrollBox::OnUpAction( const SDL_Event & rEvent )
{
    HandleKeyboardGamepadScroll( -1 );

}   // OnLeftAction

/************************************************************************
*    desc:  Handle OnRightAction message
************************************************************************/
void CUIScrollBox::OnDownAction( const SDL_Event & rEvent )
{
    HandleKeyboardGamepadScroll( 1 );

}   // OnUpAction

/************************************************************************
*    desc:  Handle the mouse move
************************************************************************/
bool CUIScrollBox::OnMouseMove( const SDL_Event & rEvent )
{
    bool result = CUISubControl::OnMouseMove( rEvent );
    
    if( NGenFunc::DynCast<CUISlider *>(GetSubControl(NUIControl::SCROLL_BAR))->IsMouseDown() )
    {
        // Get the current scroll position
        m_scrollCurPos = NGenFunc::DynCast<CUISlider *>(GetSubControl(NUIControl::SCROLL_BAR))->GetValue();

        // Set the bounds
        SetStartEndPos();

        // Reposition the scroll controlls
        RepositionScrollControls();
    }
    
    return result;
    
}   // OnMouseMove


/************************************************************************
*    desc:  Update the control
************************************************************************/
void CUIScrollBox::Update()
{
    // Call the parent
    CUISubControl::Update();

    // Update all controls
    for( int i = m_visStartPos; i < m_visEndPos; ++i )
        m_pScrollControlVec[i]->Update();

}   // Update


/************************************************************************
*    desc:  Transform the control
************************************************************************/
void CUIScrollBox::DoTransform( const CObject & object )
{
    // Call the parent
    CUISubControl::DoTransform( object );

    // Transform all controls
    for( int i = m_visStartPos; i < m_visEndPos; ++i )
        m_pScrollControlVec[i]->DoTransform( *this );

    // Transform the mask
    m_stencilMaskSprite->Transform( GetMatrix(), WasWorldPosTranformed() );

}   // DoTransform


/************************************************************************
*    desc:  Render the sub control
************************************************************************/
void CUIScrollBox::Render( const CMatrix & matrix )
{
    // Call the parent
    CUISubControl::Render( matrix );

    if( IsVisible() )
    {
        // Disable rendering to the color buffer
        glColorMask( GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE );
        glDepthMask( GL_FALSE );

        // Start using the stencil
        glEnable( GL_STENCIL_TEST );

        glStencilFunc( GL_ALWAYS, 0x1, 0x1 );
        glStencilMask(0x1);
        glStencilOp( GL_REPLACE, GL_REPLACE, GL_REPLACE );
        

        m_stencilMaskSprite->Render( matrix );


        // Re-enable color
        glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );

        // Where a 1 was not rendered
        glStencilFunc( GL_EQUAL, 0x1, 0x1 );

        // Keep the pixel
        glStencilOp( GL_KEEP, GL_KEEP, GL_KEEP );

        // Disable any writing to the stencil buffer
        glStencilMask(0x00);
        glDepthMask(GL_TRUE);

        for( int i = m_visStartPos; i < m_visEndPos; ++i )
            m_pScrollControlVec[i]->Render( matrix );

        // Finished using stencil
        glDisable( GL_STENCIL_TEST );
    }

}   // Render


/************************************************************************
*    desc: Set the first inactive control to be active
*    NOTE: Don't want this functuality for the scroll box buttons and slider
************************************************************************/
bool CUIScrollBox::ActivateFirstInactiveControl()
{
    if( CUIControl::ActivateFirstInactiveControl() )
    {
        for( size_t i = 0; i < m_pScrollControlVec.size(); ++i )
        {
            if( m_pScrollControlVec[i]->ActivateFirstInactiveControl() )
            {
                m_activeScrollCtrl = i;
                break;
            }
        }
    }

    return m_activeScrollCtrl != NUIControl::NO_ACTIVE_CONTROL;

}   // ActivateFirstInactiveControl


/************************************************************************
*    desc:  Handle the select action
************************************************************************/
bool CUIScrollBox::HandleSelectAction( NDefs::EDeviceId deviceUsed, NMenu::EActionPress pressType )
{
    bool result = CUISubControl::HandleSelectAction( deviceUsed, pressType );
    
    if( deviceUsed == NDefs::MOUSE )
    {
        if( pressType == NMenu::EAP_DOWN )
        {
            // Get the current scroll position
            m_scrollCurPos = NGenFunc::DynCast<CUISlider *>(GetSubControl(NUIControl::SCROLL_BAR))->GetValue();

            // Set the bounds
            SetStartEndPos();

            // Reposition the scroll controlls
            RepositionScrollControls();
        }
    }

    return result;

}   // HandleSelectAction


/************************************************************************
*    desc:  Handle the keyboard/Gamepad scrolling
*
*	 param: int scrollVector - direction to scroll
************************************************************************/
void CUIScrollBox::HandleKeyboardGamepadScroll( int scrollVector )
{
    bool controlInc(false);
    const bool SCROLL_DOWN((scrollVector > 0));
    const bool SCROLL_UP((scrollVector < 0));
    
    // Set a temp variable to the active scroll control
    int tmpScrollCtrl = m_activeScrollCtrl;
    
    // If the active control is not within the active area, make it so that it will be the first one selected
    if( (tmpScrollCtrl < m_firstScrollCtrlIndex) || (tmpScrollCtrl > (m_firstScrollCtrlIndex + m_visibleCount)) )
        tmpScrollCtrl = m_firstScrollCtrlIndex + (-scrollVector);
    
    if( (SCROLL_UP && (tmpScrollCtrl > 0)) || 
        (SCROLL_DOWN && (tmpScrollCtrl < static_cast<int>(m_pScrollControlVec.size()-1))) )
    {
        // This loop doesn't go through all the controls. It only uses the
        // full range to find the next selectable control
        for( size_t i = 0; i < m_pScrollControlVec.size(); ++i )
        {
            tmpScrollCtrl += scrollVector;
            
            // Do not loop the items in the scroll box
            if( tmpScrollCtrl < 0 )
                tmpScrollCtrl = 0;
            else if( tmpScrollCtrl >= static_cast<int>(m_pScrollControlVec.size()) )
                tmpScrollCtrl = static_cast<int>(m_pScrollControlVec.size()-1);
            else
                controlInc = true;
            
            if( !m_pScrollControlVec[tmpScrollCtrl]->IsDisabled() )
            {
                NGenFunc::DispatchEvent( 
                    NMenu::EGE_MENU_CONTROL_STATE_CHANGE,
                    NUIControl::ECS_ACTIVE,
                    &m_pScrollControlVec[tmpScrollCtrl]->GetName() );
                
                m_activeScrollCtrl = tmpScrollCtrl;
                break;
            }
        }
    }
    
}   // HandleKeyboardGamepadScroll


/************************************************************************
*    desc:  Get the pointer to the subcontrol if found
************************************************************************/
CUIControl * CUIScrollBox::FindSubControl( const std::string & name )
{
    CUIControl * pCtrl = CUISubControl::FindSubControl( name );
    
    for( int i = m_visStartPos; i < m_visEndPos && (pCtrl == nullptr); ++i )
        pCtrl = m_pScrollControlVec[i]->FindControl( name );

    return pCtrl;
    
}   // FindSubControl


/************************************************************************
*    desc:  Handle the sub control mouse move
************************************************************************/
bool CUIScrollBox::OnSubControlMouseMove( const SDL_Event & rEvent )
{
    bool result = CUISubControl::OnSubControlMouseMove( rEvent );

    // We only care about the scroll controls if the point is within the scroll box
    if( !result && CUISubControl::IsPointInControl( rEvent.motion.x, rEvent.motion.y ) )
    {
        for( int i = m_visStartPos; i < m_visEndPos && !result; ++i )
        {
            result = m_pScrollControlVec[i]->OnMouseMove( rEvent );
            
            if( result )
                m_activeScrollCtrl = i;
        }
    }

    return result;

}   // OnSubControlMouseMove


/************************************************************************
*    desc:  Deactivate the sub control
************************************************************************/
void CUIScrollBox::DeactivateSubControl()
{
    CUISubControl::DeactivateSubControl();
    
    for( int i = m_visStartPos; i < m_visEndPos; ++i )
        m_pScrollControlVec[i]->DeactivateControl();

}   // DeactivateSubControl


/************************************************************************
*    desc:  Set the start and end positions
************************************************************************/
void CUIScrollBox::SetStartEndPos()
{
    float pos( m_scrollCurPos / (float)m_controlHeight );
    
    m_visStartPos = pos;
    m_visEndPos = m_visStartPos + m_visibleCount + 1;
  
    // Add one of the bottom is peaking out enough to see
    //if( (pos - (float)m_visStartPos) > .1f )
        //m_visEndPos++;

    // Sanity checks
    if( m_visStartPos < 0 )
        m_visStartPos = 0;

    if( m_visEndPos > static_cast<int>(m_pScrollControlVec.size()) )
        m_visEndPos = static_cast<int>(m_pScrollControlVec.size());

}   // SetStartEndPos


/************************************************************************
*    desc:  Reposition the scroll controls
************************************************************************/
void CUIScrollBox::RepositionScrollControls()
{
    for( int i = m_visStartPos; i < m_visEndPos; ++i )
    {
        CPoint<float> pos( m_pScrollControlVec[i]->GetPos() );
        pos.y = m_defaultOffset[i] + m_scrollCurPos;
        m_pScrollControlVec[i]->SetPos( pos );
    }

}   // RepositionScrollControls
