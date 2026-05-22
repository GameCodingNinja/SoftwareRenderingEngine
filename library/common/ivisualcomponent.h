
/************************************************************************
*    FILE NAME:       ivisualcomponent.h
*
*    DESCRIPTION:     iVisualComponent Interface
************************************************************************/

#ifndef __ivisual_component_h__
#define __ivisual_component_h__

// Game lib dependencies
#include <common/color.h>
#include <common/defs.h>

// Forward declaration(s)
class CMatrix;

class iVisualComponent
{
public:

    // Constructor
    iVisualComponent(){}

    // Destructor
    virtual ~iVisualComponent(){}

    // do the render
    virtual void render( const CMatrix & matrix ) = 0;

    // Is this component active?
    virtual bool isActive() = 0;

    // Set/Get the color
    virtual void setColor( const CColor<float> & color ) = 0;
    virtual const CColor<float> & getColor() const = 0;

    // Set the texture from index
    virtual void setTexture( uint index ) = 0;
};

#endif
