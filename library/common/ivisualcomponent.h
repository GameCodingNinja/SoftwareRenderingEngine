
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
    virtual void Render( const CMatrix & matrix ) = 0;

    // Is this component active?
    virtual bool IsActive() = 0;

    // Set/Get the color
    virtual void SetColor( const CColor<float> & color ) = 0;
    virtual const CColor<float> & GetColor() const = 0;

    // Set the texture ID from index
    virtual void SetTextureID( uint index ) = 0;
};

#endif  // __ivisual_component_h__
