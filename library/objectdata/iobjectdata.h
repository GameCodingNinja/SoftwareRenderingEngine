
/************************************************************************
*    FILE NAME:       iobjectdata.h
*
*    DESCRIPTION:     iObjectData Interface
************************************************************************/

#ifndef __iobject_data_h__
#define __iobject_data_h__

class iObjectData
{
public:

    // Constructor
    iObjectData(){}

    // Destructor
    virtual ~iObjectData(){}

    // Is this 2D data?
    virtual bool Is2D() const
    { return false; }

    // Is this 3D data?
    virtual bool Is3D() const
    { return false; }
};

#endif
