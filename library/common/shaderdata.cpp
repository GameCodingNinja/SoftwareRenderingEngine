
/************************************************************************
*    FILE NAME:       shaderdata.cpp
*
*    DESCRIPTION:     shader data class
************************************************************************/

// Glew dependencies (have to be defined first)
#include <GL/glew.h>

// Physical component dependency
#include <common/shaderdata.h>

// Boost lib dependencies
#include <boost/format.hpp>

// Game lib dependencies
#include <utilities/exceptionhandling.h>


/************************************************************************
*    desc:  Constructer
************************************************************************/
CShaderData::CShaderData()
    : m_programID(0), m_vertexID(0), m_fragmentID(0)
{
}   // constructor


/************************************************************************
*    desc:  destructer                                                             
************************************************************************/
CShaderData::~CShaderData()
{
    // DO NOT USE FOR FREEING DATA!

    // This class is for holding and copying data. Freeing OpenGL assets
    // is done from the out side with the Free() call.
}


/************************************************************************
*    desc:  Set/Get the program ID
************************************************************************/
void CShaderData::SetProgramID( GLuint location )
{
    m_programID = location;

}   // SetProgramID

GLuint CShaderData::GetProgramID() const
{
    return m_programID;

}   // GetProgramID


/************************************************************************
*    desc:  Set/Get the vertex ID
************************************************************************/
void CShaderData::SetVertexID( GLuint location )
{
    m_vertexID = location;

}   // SetVertexID

GLuint CShaderData::GetVertexID() const
{
    return m_vertexID;

}   // GetVertexID


/************************************************************************
*    desc:  Set/Get the fragment ID
************************************************************************/
void CShaderData::SetFragmentID( GLuint location )
{
    m_fragmentID = location;

}   // SetFragmentID

GLuint CShaderData::GetFragmentID() const
{
    return m_fragmentID;

}   // GetFragmentID


/************************************************************************
*    desc:  Set/Get the attribute location
************************************************************************/
void CShaderData::SetAttributeLocation( const std::string & name, GLint location )
{
    auto iter = m_attributeMap.find( name );
    if( iter != m_attributeMap.end() )
    {
        throw NExcept::CCriticalException("Set Attribute Error!",
                boost::str( boost::format("Shader attribute allready exists (%s).\n\n%s\nLine: %s") % name % __FUNCTION__ % __LINE__ ));
    }

    m_attributeMap.insert( std::make_pair(name, location) );

}   // SetAttributeLocation

GLint CShaderData::GetAttributeLocation( const std::string & name ) const
{
    auto iter = m_attributeMap.find( name );
    if( iter == m_attributeMap.end() )
    {
        throw NExcept::CCriticalException("Get Attribute Error!",
                boost::str( boost::format("Shader attribute does not exist (%s).\n\n%s\nLine: %s") % name % __FUNCTION__ % __LINE__ ));
    }

    return iter->second;

}   // GetAttributeLocation


/************************************************************************
*    desc:  Set/Get the uniform location
************************************************************************/
void CShaderData::SetUniformLocation( const std::string & name, GLint location )
{
    auto iter = m_uniformMap.find( name );
    if( iter != m_uniformMap.end() )
    {
        throw NExcept::CCriticalException("Set Attribute Error!",
                boost::str( boost::format("Shader attribute allready exists (%s).\n\n%s\nLine: %s") % name % __FUNCTION__ % __LINE__ ));
    }

    m_uniformMap.insert( std::make_pair(name, location) );

}   // SetUniformLocation

GLint CShaderData::GetUniformLocation( const std::string & name ) const
{
    auto iter = m_uniformMap.find( name );
    if( iter == m_uniformMap.end() )
    {
        throw NExcept::CCriticalException("Get Uniform Error!",
                boost::str( boost::format("Shader uniform does not exists (%s).\n\n%s\nLine: %s") % name % __FUNCTION__ % __LINE__ ));
    }

    return iter->second;

}   // GetUniformLocation


/************************************************************************
*    desc:  Free the data
************************************************************************/
void CShaderData::Free()
{
    // Detach the shaders
    if( (m_vertexID > 0) && (m_fragmentID > 0) && (m_programID > 0) )
    {
        glDetachShader(m_programID, m_vertexID);
        glDetachShader(m_programID, m_fragmentID);
    }

    // Delete the shaders
    if( m_vertexID > 0 )
        glDeleteShader(m_vertexID);

    if( m_fragmentID > 0 )
        glDeleteShader(m_fragmentID);

    // Delete the shader program
    if( m_programID > 0 )
        glDeleteProgram(m_programID);

    Clear();

}   // Free


/************************************************************************
*    desc:  Clear the data
************************************************************************/
void CShaderData::Clear()
{
    m_programID = m_vertexID = m_fragmentID = 0;

    m_attributeMap.clear();
    m_uniformMap.clear();

}   // Clear

