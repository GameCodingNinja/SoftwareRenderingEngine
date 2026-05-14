
/************************************************************************
*    FILE NAME:       shaderdata.h
*
*    DESCRIPTION:     shader data class
************************************************************************/

#ifndef __shader_data_h__
#define __classtemplate_h__

// SDL/OpenGL lib dependencies
#include <SDL_opengl.h>

// Standard lib dependencies
#include <string>
#include <map>
#include <vector>

class CShaderData
{
public:

    CShaderData();
    ~CShaderData();

    // Set/Get the program location
    void SetProgramID( GLuint location );
    GLuint GetProgramID() const;

    // Set/Get the vertex ID
    void SetVertexID( GLuint location );
    GLuint GetVertexID() const;

    // Set/Get the fragment ID
    void SetFragmentID( GLuint location );
    GLuint GetFragmentID() const;

    // Get the attribute location
    void SetAttributeLocation( const std::string & name, GLint location );
    GLint GetAttributeLocation( const std::string & name ) const;

    // Get the uniform location
    void SetUniformLocation( const std::string & name, GLint location );
    GLint GetUniformLocation( const std::string & name ) const;

    // Free the data
    void Free();

private:

    // Clear the data
    void Clear();

private:

    // OpenGL ID's
    GLuint m_programID;
    GLuint m_vertexID;
    GLuint m_fragmentID;

    // attribute location shader map
    std::map<const std::string, GLint > m_attributeMap;

    // uniform location shader map
    std::map<const std::string, GLint > m_uniformMap;

};

#endif  // __shader_data_h__


