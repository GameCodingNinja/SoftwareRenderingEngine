
/************************************************************************
*    FILE NAME:       uislider.cpp
*
*    DESCRIPTION:     Class for user interface slider
************************************************************************/

// Physical component dependency
#include <gui/uislider.h>

// Boost lib dependencies
#include <boost/format.hpp>

// Game lib dependencies
#include <gui/menudefs.h>
#include <gui/uicontroldefs.h>
#include <managers/actionmanager.h>
#include <utilities/genfunc.h>
#include <utilities/xmlParser.h>
#include <utilities/settings.h>

/************************************************************************
*    desc:  Constructer
************************************************************************/
CUISlider::CUISlider()
    : m_travelDistPixels(0),
      m_orientation(EO_HORIZONTAL),
      m_minValue(0),
      m_maxValue(0),
      m_curValue(0),
      m_incValue(0),
      m_displayValueAsInt(false),
      m_sliderBtnHold(false),
      m_pressType(NMenu::EAP_IDLE)
{
    SetType( NUIControl::ECT_SLIDER );

}   // constructor


/************************************************************************
*    desc:  destructer                                                             
************************************************************************/
CUISlider::~CUISlider()
{
}	// destructer


/************************************************************************
*    desc:  Load the control info from XML node
************************************************************************/
void CUISlider::LoadFromNode( const XMLNode & node )
{
    CUISubControl::LoadFromNode( node );

    // Get the slider specific settings
    XMLNode settingsNode = node.getChildNode( "settings" );
    if( !settingsNode.isEmpty() )
    {
        if( settingsNode.isAttributeSet( "orientation" ) )
            if( std::string(settingsNode.getAttribute( "orientation" )) == "VERT" )
                m_orientation = EO_VERTICAL;

        if( settingsNode.isAttributeSet( "minValue" ) )
            m_minValue = atof(settingsNode.getAttribute( "minValue" ));

        if( settingsNode.isAttributeSet( "maxValue" ) )
            m_maxValue = atof(settingsNode.getAttribute( "maxValue" ));

        if( settingsNode.isAttributeSet( "incValue" ) )
            m_incValue = atof(settingsNode.getAttribute( "incValue" ));

        if( settingsNode.isAttributeSet( "defValue" ) )
            SetSlider( atof(settingsNode.getAttribute( "defValue" )) );
        else
            SetSlider();

        if( settingsNode.isAttributeSet( "displayValueAsInt" ) )
            if( std::string(settingsNode.getAttribute( "displayValueAsInt" )) == "true" )
                m_displayValueAsInt = true;
    }

}   // LoadFromNode


/************************************************************************
*    desc:  Load the control specific info from XML node
*  
*    param: node - XML node
************************************************************************/
void CUISlider::LoadControlFromNode( const XMLNode & controlNode )
{
    // Have the parent load it's stuff
    CUISubControl::LoadControlFromNode( controlNode );

    // Get the position of the slider button as the default position
    m_defaultPos = GetSubControl()->GetPos();

    // Get the slider specific settings
    XMLNode settingsNode = controlNode.getChildNode( "settings" );
    if( !settingsNode.isEmpty() )
    {
        if( settingsNode.isAttributeSet( "maxTravelDistPixels" ) )
            m_travelDistPixels = atof(settingsNode.getAttribute( "maxTravelDistPixels" ));
    }

}	// LoadControlFromNode


/************************************************************************
*    desc:  Handle OnLeftAction message
************************************************************************/
void CUISlider::OnLeftAction( const SDL_Event & rEvent )
{
    // Handle the slider change 
    HandleSliderChange( -m_incValue, true );

}   // OnLeftAction

/************************************************************************
*    desc:  Handle OnRightAction message
************************************************************************/
void CUISlider::OnRightAction( const SDL_Event & rEvent )
{
    // Handle the slider change
    HandleSliderChange( m_incValue, true );

}   // OnRightAction

/************************************************************************
*    desc:  Handle OnRightScroll message
************************************************************************/
void CUISlider::OnLeftScroll( const SDL_Event & rEvent )
{
    HandleSliderChange( -m_incValue );

}   // OnLeftScrol

/************************************************************************
*    desc:  Handle OnRightScroll message
************************************************************************/
void CUISlider::OnRightScroll( const SDL_Event & rEvent )
{
    HandleSliderChange( m_incValue );

}   // OnRightScroll

/************************************************************************
*    desc:  Handle OnMouseMove message
************************************************************************/
bool CUISlider::OnMouseMove( const SDL_Event & rEvent )
{
    bool result = CUISubControl::OnMouseMove( rEvent );
    
    if( IsActive() && (m_pressType == NMenu::EAP_DOWN) )
    {
        if( m_orientation == EO_HORIZONTAL )
            IncSliderMovePos( (float)rEvent.motion.xrel * CSettings::Instance().GetDeviceRatio() );
        else
            IncSliderMovePos( (float)rEvent.motion.yrel * CSettings::Instance().GetDeviceRatio() );

        ExecuteAction();
    }
    
    return result;

}   // OnMouseMove


/************************************************************************
*    desc:  Handle the select action
************************************************************************/
bool CUISlider::HandleSelectAction( NDefs::EDeviceId deviceUsed, NMenu::EActionPress pressType )
{
    if( IsActive() && (deviceUsed == NDefs::MOUSE) )
    {
        m_pressType = pressType;

        if( m_pressType == NMenu::EAP_DOWN )
        {
            PrepareControlScriptFunction( NUIControl::ECSF_ON_SELECTED );

            CPoint<float> dif = (CActionManager::Instance().GetLastMousePos() - GetSubControl()->GetCollisionPos()) * CSettings::Instance().GetDeviceRatio();

            if( m_orientation == EO_HORIZONTAL )
                IncSliderMovePos( dif.x );
            else
                IncSliderMovePos( dif.y );

            ExecuteAction();
        }
    }

    return CUISubControl::HandleSelectAction( deviceUsed, pressType );

}   // HandleSelectAction


/************************************************************************
*    desc:  Deactivate the control
************************************************************************/
void CUISlider::DeactivateControl()
{
    CUISubControl::DeactivateControl();

    m_pressType = NMenu::EAP_IDLE;

}   // DeactivateControl


/************************************************************************
*    desc:  Handle the slider change
************************************************************************/
void CUISlider::HandleSliderChange( float value, bool prepareOnSelect )
{
    if( IsActive() )
    {
        if( prepareOnSelect )
            PrepareControlScriptFunction( NUIControl::ECSF_ON_SELECTED );

        // Send a message to blink the button
        NGenFunc::DispatchEvent( 
            NMenu::EGE_MENU_CONTROL_STATE_CHANGE,
            NUIControl::ECS_SELECTED,
            &GetSubControl()->GetName() );

        IncSlider(value);

        ExecuteAction();
    }

}   // HandleSliderChange


/************************************************************************
*    desc:  Set the slider min value
*  
*    param: value
************************************************************************/
void CUISlider::SetMinValue( float value )
{
    m_minValue = value;

}	// SetMinValue


/************************************************************************
*    desc:  Set the slider max value
*  
*    param: value
************************************************************************/
void CUISlider::SetMaxValue( float value )
{
    m_maxValue = value;

}	// SetMaxValue


/************************************************************************
*    desc:  Set the slider inc value
*  
*    param: float sliderPos
************************************************************************/
void CUISlider::SetSlider( float value )
{
    m_curValue = value;

    // Update the slider
    UpdateSlider();

}	// SetSlider


/************************************************************************
*    desc:  Set the slider inc value
*  
*    param: float slider value
************************************************************************/
void CUISlider::IncSlider( float value )
{
    m_curValue += value;

    // Update the slider
    UpdateSlider();

}	// IncSlider


/************************************************************************
*    desc:  Inc the slider based on mouse movement
*  
*    param: float slider value
************************************************************************/
void CUISlider::IncSliderMovePos( float value )
{
    m_curValue += value * (m_maxValue / m_travelDistPixels);

    // Update the slider
    UpdateSlider();

}	// IncSliderMovePos


/************************************************************************
*    desc:  Update the slider
************************************************************************/
void CUISlider::UpdateSlider()
{
    // Cap current value to it's range
    CapSliderValue();

    // Set the position of the slider
    SetSliderPos();

    // Set the slider label if there is one
    const std::vector<std::string> & strVec = GetStringVec();
    if( !strVec.empty() )
    {
        // See if we specified a format string
        std::string formatStr = strVec.back();

        // Format for display
        std::string valueStr;

        if( m_displayValueAsInt )
            valueStr = boost::str( boost::format(formatStr) % (int)m_curValue );
        else
            valueStr = boost::str( boost::format(formatStr) % m_curValue );

        CreateFontString( valueStr );
    }

}	// UpdateSlider


/************************************************************************
*    desc:  Cap the slider value
************************************************************************/
void CUISlider::CapSliderValue()
{
    // Cap current value to range
    if( m_curValue < m_minValue )
        m_curValue = m_minValue;

    else if( m_curValue > m_maxValue )
        m_curValue = m_maxValue;

}	// CapSliderValue


/************************************************************************
*    desc:  Set the position of the slider
************************************************************************/
void CUISlider::SetSliderPos()
{
    if( std::fabs(m_maxValue) > 0.001f )
    {
        float startPos = -(m_travelDistPixels / 2);
        float pixelsPerValue = m_travelDistPixels / m_maxValue;
        float pos = startPos + (pixelsPerValue * m_curValue);

        if( m_orientation == EO_HORIZONTAL )
            GetSubControl()->SetPos( m_defaultPos + CPoint<float>(pos,0,0) );
        else
            GetSubControl()->SetPos( m_defaultPos + CPoint<float>(0,-pos,0) );
    }

}	// SetSliderPos


/************************************************************************
*    desc:  Cap the slider value
************************************************************************/
float CUISlider::GetValue()
{
    return m_curValue;

}   // GetValue


/************************************************************************
*    desc:  Is the mouse down
************************************************************************/
bool CUISlider::IsMouseDown()
{
    return (m_pressType == NMenu::EAP_DOWN);

}   // IsMouseDown
