
/************************************************************************
*    FILE NAME:       shadermanager.cpp
*
*    DESCRIPTION:     Shader manager class singleton
************************************************************************/

// Physical component dependency
#include <managers/shadermanager.h>

// Game lib dependencies
#include <utilities/genfunc.h>

/************************************************************************
*    DESC:  Constructor
************************************************************************/
CShaderMgr::CShaderMgr()
{
}

/************************************************************************
*    DESC:  Destructor
************************************************************************/
CShaderMgr::~CShaderMgr()
{
}

/************************************************************************
*    DESC:  Register a named shader function
************************************************************************/
void CShaderMgr::registerShader( const std::string & name, FragmentShaderFunc func )
{
    auto iter = m_shaderMap.find( name );
    if( iter != m_shaderMap.end() )
    {
        NGenFunc::PostDebugMsg( NGenFunc::FormatString("Shader already registered (%s). Overwriting.", name.c_str()) );
        iter->second = func;
    }
    else
    {
        m_shaderMap.emplace( name, func );
    }
}

/************************************************************************
*    DESC:  Look up a shader by name
************************************************************************/
FragmentShaderFunc CShaderMgr::get( const std::string & name ) const
{
    auto iter = m_shaderMap.find( name );
    if( iter != m_shaderMap.end() )
        return iter->second;

    NGenFunc::PostDebugMsg( NGenFunc::FormatString("Shader not found (%s). Using default.", name.c_str()) );
    return nullptr;
}

/************************************************************************
*    DESC:  Check if a shader is registered
************************************************************************/
bool CShaderMgr::has( const std::string & name ) const
{
    return m_shaderMap.find( name ) != m_shaderMap.end();
}

/************************************************************************
*    DESC:  Clear all registered shaders
************************************************************************/
void CShaderMgr::clear()
{
    m_shaderMap.clear();
}
