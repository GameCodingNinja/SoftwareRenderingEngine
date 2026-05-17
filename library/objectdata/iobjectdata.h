
/************************************************************************
*    FILE NAME:       iobjectdata.h
*
*    DESCRIPTION:     iObjectData Interface
************************************************************************/

#ifndef __iobject_data_h__
#define __iobject_data_h__

// Forward declaration(s)
class CObjectVisualData2D;

class iObjectData
{
public:

    // Constructor
    iObjectData(){}

    // Destructor
    virtual ~iObjectData(){}

    // Access functions for the visual data
    virtual const CObjectVisualData2D & GetVisualData() const = 0;

    // Is this 2D data?
    virtual bool Is2D() const
    { return false; }

    // Is this 3D data?
    virtual bool Is3D() const
    { return false; }
};

#endif  // __iobject_data_h__
