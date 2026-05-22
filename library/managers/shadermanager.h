
/************************************************************************
*    FILE NAME:       shadermanager.h
*
*    DESCRIPTION:     Shader manager class singleton.
*                     The game side registers named shader functions.
*                     Object data looks up shaders by name at load time.
************************************************************************/

#pragma once

// Game lib dependencies
#include <softwareRender/renderdefs.h>

// Standard lib dependencies
#include <string>
#include <map>

class CShaderMgr
{
public:

    // Get the instance of the singleton class
    static CShaderMgr & Instance()
    {
        static CShaderMgr shaderMgr;
        return shaderMgr;
    }

    // Register a named shader function (called from game side at startup)
    void registerShader( const std::string & name, FragmentShaderFunc func );

    // Look up a shader by name. Returns nullptr if not found.
    FragmentShaderFunc get( const std::string & name ) const;

    // Check if a shader is registered
    bool has( const std::string & name ) const;

    // Clear all registered shaders
    void clear();

private:

    CShaderMgr();
    ~CShaderMgr();

private:

    // Map of named shader functions
    std::map<std::string, FragmentShaderFunc> m_shaderMap;
};
