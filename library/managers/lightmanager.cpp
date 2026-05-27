
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
*    desc:  Load the list table
************************************************************************/
void CLightMgr::loadListTable( const std::string & filePath )
{
    const XMLNode node = XMLNode::openFileHelper( filePath.c_str(), "listTable" );

    for( int i = 0; i < node.nChildNode("groupList"); ++i )
    {
        const XMLNode groupNode = node.getChildNode( "groupList", i );
        const std::string group = groupNode.getAttribute( "groupName" );

        for( int j = 0; j < groupNode.nChildNode("file"); ++j )
        {
            const XMLNode fileNode = groupNode.getChildNode( "file", j );
            const std::string path = fileNode.getAttribute( "path" );

            loadFromFile( group, path );
        }
    }
}

/************************************************************************
*    desc:  Load a group's lights from a data file
************************************************************************/
void CLightMgr::loadFromFile( const std::string & group, const std::string & filePath )
{
    const XMLNode node = XMLNode::openFileHelper( filePath.c_str(), "lightDataList" );
    const XMLNode listNode = node.getChildNode( "lightList" );

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
*    desc:  Add a light to a named group
************************************************************************/
void CLightMgr::add( const std::string & group, const CLight & light )
{
    m_lightVecMap[group].push_back( light );
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
*    desc:  Delete a light group
************************************************************************/
void CLightMgr::deleteGroup( const std::string & group )
{
    auto iter = m_lightVecMap.find( group );
    if( iter != m_lightVecMap.end() )
        m_lightVecMap.erase( iter );
}

/************************************************************************
*    desc:  Clear all groups
************************************************************************/
void CLightMgr::clear()
{
    m_lightVecMap.clear();
}
