
/************************************************************************
*    FILE NAME:       lightmanager.cpp
*
*    DESCRIPTION:     Light manager class singleton
************************************************************************/

// Physical component dependency
#include <managers/lightmanager.h>

// Game lib dependencies
#include <utilities/xmlParser.h>
#include <utilities/xmlparsehelper.h>
#include <utilities/exceptionhandling.h>
#include <utilities/genfunc.h>

// Standard lib dependencies
#include <cstring>

/************************************************************************
*    desc:  Constructor
************************************************************************/
CLightMgr::CLightMgr()
{
}

/************************************************************************
*    desc:  Destructor
************************************************************************/
CLightMgr::~CLightMgr()
{
}

/************************************************************************
*    desc:  Load all lights for a specific group
************************************************************************/
void CLightMgr::LoadGroup( const std::string & group )
{
    // Make sure the group we are looking has been defined in the list table file
    auto listTableIter = m_listTableMap.find( group );
    if( listTableIter == m_listTableMap.end() )
        throw NExcept::CCriticalException("Light Load Group Error!",
            NGenFunc::FormatString("Light group name can't be found (%s).\n\n%s\nLine: %d", group, __FUNCTION__, __LINE__));

    // Load the group data if it doesn't already exist
    if( m_lightVecMap.find( group ) == m_lightVecMap.end() )
    {
        // Create a new group vector
        m_lightVecMap.insert( std::make_pair(group, std::vector<CLight>()) );

        for( size_t i = 0; i < listTableIter->second.size(); ++i )
            LoadFromXML( group, listTableIter->second[i] );
    }
    else
    {
        throw NExcept::CCriticalException("Light Load Group Error!",
            NGenFunc::FormatString("Light group has already been loaded (%s).\n\n%s\nLine: %d", group, __FUNCTION__, __LINE__));
    }
}

/************************************************************************
*    desc:  Load lights from an XML data file
************************************************************************/
void CLightMgr::LoadFromXML( const std::string & group, const std::string & filePath )
{
    const XMLNode listNode = XMLNode::openFileHelper( filePath.c_str(), "lightList" );

    if( listNode.isEmpty() )
        return;

    auto & lightVec = m_lightVecMap[group];

    for( int i = 0; i < listNode.nChildNode("light"); ++i )
    {
        const XMLNode lightNode = listNode.getChildNode( "light", i );
        const std::string type = lightNode.getAttribute( "type" );

        CLight light;

        // Parse type
        if( type == "ambient" )
            light.m_type = ELightType::AMBIENT;
        else if( type == "directional" )
            light.m_type = ELightType::DIRECTIONAL;
        else if( type == "point" )
            light.m_type = ELightType::POINT;
        else if( type == "spot" )
            light.m_type = ELightType::SPOT;

        // Parse color
        light.m_color = NParseHelper::loadColor( lightNode, light.m_color );

        // Parse intensity
        const XMLNode intensityNode = lightNode.getChildNode("intensity");
        if( !intensityNode.isEmpty() )
            light.m_intensity = std::atof( intensityNode.getAttribute("value") );

        // Parse direction (auto-normalizes)
        bool hasDir;
        light.m_direction = NParseHelper::loadDirection( lightNode, hasDir );

        // Parse position
        bool hasPos;
        light.m_position = NParseHelper::loadPosition( lightNode, hasPos );

        // Parse radius
        const XMLNode radiusNode = lightNode.getChildNode("radius");
        if( !radiusNode.isEmpty() )
            light.m_radius = std::atof( radiusNode.getAttribute("value") );

        // Parse spot cone angles
        const XMLNode coneNode = lightNode.getChildNode("cone");
        if( !coneNode.isEmpty() )
        {
            if( coneNode.isAttributeSet("inner") )
                light.m_innerCone = std::atof( coneNode.getAttribute("inner") );
            if( coneNode.isAttributeSet("outer") )
                light.m_outerCone = std::atof( coneNode.getAttribute("outer") );
        }

        // Parse specular
        const XMLNode specNode = lightNode.getChildNode("specular");
        if( !specNode.isEmpty() )
        {
            light.m_specular = (std::string(specNode.getAttribute("enable")) == "true");
            if( specNode.isAttributeSet("shininess") )
                light.m_shininess = std::atof( specNode.getAttribute("shininess") );
        }

        lightVec.push_back( light );
    }
}

/************************************************************************
*    desc:  Free a group's lights
************************************************************************/
void CLightMgr::FreeGroup( const std::string & group )
{
    auto iter = m_lightVecMap.find( group );
    if( iter != m_lightVecMap.end() )
        m_lightVecMap.erase( iter );
}

/************************************************************************
*    desc:  Get the light vector for a group
************************************************************************/
const std::vector<CLight> & CLightMgr::get( const std::string & group ) const
{
    auto iter = m_lightVecMap.find( group );
    if( iter == m_lightVecMap.end() )
        throw NExcept::CCriticalException("CLightMgr Error!",
            NGenFunc::FormatString("Light group not found (%s).\n\n%s\nLine: %d",
                group.c_str(), __FUNCTION__, __LINE__));

    return iter->second;
}

/************************************************************************
*    desc:  Clear all groups
************************************************************************/
void CLightMgr::clear()
{
    m_lightVecMap.clear();
}
