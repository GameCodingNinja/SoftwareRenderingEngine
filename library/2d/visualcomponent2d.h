
/************************************************************************
*    FILE NAME:       visualcomponent2d.h
*
*    DESCRIPTION:     Class for handling the visual part of the sprite
************************************************************************/

#ifndef __visual_component_2d_h__
#define __visual_component_2d_h__

// SDL/OpenGL lib dependencies
#include <SDL_opengl.h>

// Standard lib dependencies
#include <string>

// Boost lib dependencies

// Game lib dependencies
#include <common/matrix.h>
#include <common/color.h>
#include <common/point.h>
#include <common/defs.h>
#include <utilities/xmlParser.h>

// Forward declaration(s)
class CObjectData2D;
class CObjectVisualData2D;
class CFont;

class CVisualComponent2d
{
public:

    // Constructor
    CVisualComponent2d( const CObjectData2D & objectData );

    // Destructor
    ~CVisualComponent2d();

    // do the render
    void Render( const CMatrix & matrix );

    // Is this component active?
    bool IsActive();

    // Set/Get the color
    void SetColor( const CColor & color );
    const CColor & GetColor() const;

    // Set/Get the alpha
    void SetAlpha( float alpha );
    float GetAlpha() const;

    // Set the texture ID from index
    void SetTextureID( uint index );

    // Set the display string
    void SetFontInfo( const std::string & fontName,
                      float kerning = 0.f,
                      float spaceCharKerning = 0.f,
                      float lineWrapWidth = -1.f,
                      float lineWrapHeight = 0.f,
                      NDefs::EHorzAlignment hAlign = NDefs::EHA_HORZ_CENTER,
                      NDefs::EVertAlignment vAlign = NDefs::EVA_VERT_CENTER );

    // Set the string to display
    void CreateFontString( const std::string & fontString );

    // Get the displayed font string 
    const std::string & GetFontString();

private:

    // Add the line width to the vector based on horz alignment
    void AddLineWithToVec(
        const CFont & font,
        std::vector<float> & lineWidthOffsetVec,
        const NDefs::EHorzAlignment hAlign,
        float width,
        float firstCharOffset,
        float lastCharOffset );

    // Add up all the character widths
    std::vector<float> CalcLineWidthOffset(
        const CFont & font,
        const std::string & str,
        float kerning,
        float spaceCharKerning,
        float lineWrapWidth,
        NDefs::EHorzAlignment hAlign );

    // Set the string to display
    void CreateFontString(
        const std::string & fontString,
        const std::string & fontName,
        float kerning,
        float spaceCharKerning,
        float lineWrapWidth,
        float lineWrapHeight,
        NDefs::EHorzAlignment hAlign,
        NDefs::EVertAlignment vAlign );

private:

    // Reference to object visual data
    const CObjectVisualData2D & m_visualData;

    // shader location data
    GLint m_vertexLocation;
    GLint m_uvLocation;
    GLint m_text0Location;
    GLint m_colorLocation;
    GLint m_matrixLocation;

    // offset variables
    const int m_vertexBufSize;
    const int m_uvOffset;

    // Loaded texture data
    GLuint m_textureID;

    // Shader program ID
    GLuint m_programID;

    // Color
    CColor m_color;

    // VBO
    GLuint m_vbo;

    // IBO
    GLuint m_ibo;

    // Vertex count
    int m_vertexCount;

    // INdex count
    int m_IndexCount;

    // OpenGL draw mode
    GLenum m_drawMode;

    // IBO indice type
    GLenum m_indiceType;

    ///////////////////////
    //  Font datatypes
    ///////////////////////

    // Displayed font string
    std::string m_fontString;

    // Font members
    std::string m_fontName;
    float m_kerning;
    float m_spaceCharKerning;
    float m_lineWrapWidth;
    float m_lineWrapHeight;
    NDefs::EHorzAlignment m_hAlign;
    NDefs::EVertAlignment m_vAlign;

    // Total number of characters to render
    int m_charCount;
};

#endif  // __visual_component_2d_h__

