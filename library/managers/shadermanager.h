
/************************************************************************
*    FILE NAME:       shadermanager.h
*
*    DESCRIPTION:     shader class singleton
************************************************************************/

#ifndef __shader_manager_h__
#define __shader_manager_h__

// SDL/OpenGL lib dependencies
#include <SDL_opengl.h>

// Standard lib dependencies
#include <string>
#include <map>

// Game lib dependencies
#include <utilities/xmlParser.h>
#include <common/shaderdata.h>

class CShaderMgr
{
public:

    // Get the instance of the singleton class
    static CShaderMgr & Instance()
    {
        static CShaderMgr shader;
        return shader;
    }

    // Load the shader from xml file path
    void LoadFromXML( const std::string & filePath );

    // Get the shader data
    const CShaderData & GetShaderData( const std::string & shaderID );

    // Function call used to manage what shader is currently bound.
    void BindShaderProgram( GLuint programID );

    // Unbind the shader and reset the flag
    void UnbindShaderProgram();

private:

    // Constructor
    CShaderMgr();

    // Destructor
    ~CShaderMgr();

    // Create the shader
    void CreateShader( const XMLNode & node );

    // Create the shader
    void CreateShader( GLenum shaderType, const std::string & filePath );

    // Create the shader program
    void CreateProgram();

    // Bind the attribute location
    void BindAttributeLocation( const XMLNode & vertexNode );

    // Link the shader program
    void LinkProgram();

    // Find the location of the shader variables
    void LocateShaderVariables( const XMLNode & vertexNode,
                                const XMLNode & fragmentNode );

    // Get the uniform location
    void GetUniformLocation( const XMLNode & node );

private:

    // Map containing shader programs
    std::map< const std::string, CShaderData > m_shaderMap;
    std::map< const std::string, CShaderData >::iterator m_Iter;

    // Current shader program ID
    GLuint m_currentProgramID;

};

#endif  // __shader_manager_h__


