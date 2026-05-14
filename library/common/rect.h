/************************************************************************
*    FILE NAME:       rect.h
*
*    DESCRIPTION:     Rect class
*
************************************************************************/  

#ifndef __rect_h__
#define __rect_h__

#pragma warning(disable : 4244)

template <class type>
class CRect
{
public:

    // size values.
    type x1, y1, x2, y2;

    /************************************************************************                                                             
    *    desc:  Constructor
    ************************************************************************/

    CRect():x1(0),y1(0),x2(0),y2(0)
    { }

    // Copy constructor
    template <typename U>
    CRect( const CRect<U> &rect ):x1(rect.x1),y1(rect.y1),x2(rect.x2),y2(rect.y2)
    { }

    // Copy constructor
    CRect( type _x1, type _y1, type _x2, type _y2 ):x1(_x1),y1(_y1),x2(_x2),y2(_y2)
    { }

    /************************************************************************                                                             
    *    desc:  Get functions
    ************************************************************************/
    type GetX1() const
    { return x1; }

    type GetY1() const
    { return y1; }

    type GetX2() const
    { return x2; }

    type GetY2() const
    { return y2; }

    // Sets the rect's x and y value based on an index and three counts
    void SetRectXY( int index, int columnCount, int rowCount, type textureHeight, type textureWidth )
    {
        int heightIndex = 0;
        int widthIndex = 0;

        for( int i = 0; i < rowCount; ++i )
        {
            if( columnCount <= index )
            {
                index -= columnCount;
            }
            else
            {
                heightIndex = i;
                widthIndex = index;
                break;
            }
        }

        type frameWidth = textureWidth / columnCount;
        type frameHeight = textureHeight / rowCount ;

        x1 = widthIndex * frameWidth;
        y1 = heightIndex * frameHeight;

        x2 = x1 + frameWidth;
        y2 = y1 + frameHeight;
    }

    /************************************************************************                                                             
    *    desc:  Does this size not have any data? 
    *
    *    return: bool
    ************************************************************************/
    bool IsEmpty() const
    {
        return ( IsX1Empty() && IsX2Empty() && IsY1Empty() &&  IsY2Empty() );
    }

    bool IsX1Empty() const
    {
        // This looks at the bits of the data as an int
        return ( (0 == x1) || (*(int*)&x1 == 0) );

    }   // IsX1Empty

    bool IsX2Empty() const
    {
        // This looks at the bits of the data as an int
        return ( (0 == x2) || (*(int*)&x2 == 0) );

    }   // IsX1Empty

    bool IsY1Empty() const
    {
        // This looks at the bits of the data as an int
        return ( (0 == y1) || (*(int*)&y1 == 0) );

    }   // IsX1Empty

    bool IsY2Empty() const
    {
        // This looks at the bits of the data as an int
        return ( (0 == y2) || (*(int*)&y2 == 0) );

    }   // IsX1Empty

    /************************************************************************                                                             
    *    desc:  Is the point in the rect
    *           Note: Fast but does not work when rect is rotated
    ************************************************************************/
    bool IsPointInRect( type x, type y )
    {
        if( ((x > x1) && (x < x2)) && ((y > y1) && (y < y2)) )
            return true;

        return false;
    }
};

#endif  // __rect_h__
