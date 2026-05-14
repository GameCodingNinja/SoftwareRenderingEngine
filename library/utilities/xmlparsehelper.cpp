/************************************************************************
*    FILE NAME:       xmlparsehelper.cpp
*
*    DESCRIPTION:     General xml parse helper functions
************************************************************************/            

// Physical component dependency
#include <utilities/xmlparsehelper.h>

namespace NParseHelper
{
    /************************************************************************
    *    desc:  Load the 2d vertex
    *
    *    param: node - passed in node
    ************************************************************************/
    /*CVertex2D LoadVertex2d( const XMLNode & node )
    {
        CVertex2D vert2D;

        if( !node.isEmpty() )
        {
            if( node.isAttributeSet( "x" ) )
                vert2D.vert.x = std::atof( node.getAttribute( "x" ) );

            if( node.isAttributeSet( "y" ) )
                vert2D.vert.y = std::atof( node.getAttribute( "y" ) );

            if( node.isAttributeSet( "z" ) )
                vert2D.vert.z = std::atof( node.getAttribute( "z" ) );

            if( node.isAttributeSet( "u" ) )
                vert2D.uv.u = std::atof( node.getAttribute( "u" ) );

            if( node.isAttributeSet( "v" ) )
                vert2D.uv.v = std::atof( node.getAttribute( "v" ) );
        }

        return vert2D;

    }*/	// LoadVertex2d


    /************************************************************************
    *    desc:  Load the position
    *
    *    param: node - passed in node
    ************************************************************************/
    CPoint<CWorldValue> LoadPosition( const XMLNode & node, bool & loaded )
    {
        CPoint<CWorldValue> point;

        loaded = false;

            XMLNode positionNode = node.getChildNode( "position" );

        if( !positionNode.isEmpty() )
        {
            loaded = true;

            if( positionNode.isAttributeSet( "xi" ) )
                point.x.i = std::atoi( positionNode.getAttribute( "xi" ) );

            if( positionNode.isAttributeSet( "yi" ) )
                point.y.i = std::atoi( positionNode.getAttribute( "yi" ) );

            if( positionNode.isAttributeSet( "zi" ) )
                point.z.i = std::atoi( positionNode.getAttribute( "zi" ) );

            if( positionNode.isAttributeSet( "xf" ) )
                point.x.f = std::atof( positionNode.getAttribute( "xf" ) );
            else if( positionNode.isAttributeSet( "x" ) )
                point.x.f = std::atof( positionNode.getAttribute( "x" ) );

            if( positionNode.isAttributeSet( "yf" ) )
                point.y.f = std::atof( positionNode.getAttribute( "yf" ) );
            else if( positionNode.isAttributeSet( "y" ) )
                    point.y.f = std::atof( positionNode.getAttribute( "y" ) );

            if( positionNode.isAttributeSet( "zf" ) )
                point.z.f = std::atof( positionNode.getAttribute( "zf" ) );
            else if( positionNode.isAttributeSet( "z" ) )
                point.z.f = std::atof( positionNode.getAttribute( "z" ) );
        }

        return point;

    }	// LoadPosition

    CPoint<CWorldValue> LoadPosition( const XMLNode & node )
    {
        bool dummy;
        return LoadPosition( node, dummy );
    }


    /************************************************************************
    *    desc:  Load the rotation
    *
    *    param: node - passed in node
    ************************************************************************/
    CPoint<float> LoadRotation( const XMLNode & node, bool & loaded )
    {
        CPoint<float> point;

        loaded = false;

        XMLNode rotationNode = node.getChildNode( "rotation" );

        if( !rotationNode.isEmpty() )
        {
            loaded = true;

            if( rotationNode.isAttributeSet( "x" ) )
                point.x = std::atof( rotationNode.getAttribute( "x" ) );

            if( rotationNode.isAttributeSet( "y" ) )
                point.y = std::atof( rotationNode.getAttribute( "y" ) );

            if( rotationNode.isAttributeSet( "z" ) )
                point.z = std::atof( rotationNode.getAttribute( "z" ) );
        }

        return point;

    }	// LoadPosition

    CPoint<float> LoadRotation( const XMLNode & node )
    {
        bool dummy;
        return LoadRotation( node, dummy );
    }


    /************************************************************************
    *    desc:  Load the scale
    *
    *    param: node - passed in node
    ************************************************************************/
    CPoint<float> LoadScale( const XMLNode & node, bool & loaded )
    {
        CPoint<float> point(1,1,1);

        loaded = false;

        XMLNode scaleNode = node.getChildNode( "scale" );
        if( !scaleNode.isEmpty() )
        {
            loaded = true;

            if( scaleNode.isAttributeSet( "uniform" ) )
                    point.Set( std::atof( scaleNode.getAttribute( "uniform" ) ) );

            else
            {
                if( scaleNode.isAttributeSet( "x" ) )
                    point.x = std::atof( scaleNode.getAttribute( "x" ) );

                if( scaleNode.isAttributeSet( "y" ) )
                    point.y = std::atof( scaleNode.getAttribute( "y" ) );

                if( scaleNode.isAttributeSet( "z" ) )
                    point.z = std::atof( scaleNode.getAttribute( "z" ) );
            }
        }

        return point;

    }	// LoadScale

    CPoint<float> LoadScale( const XMLNode & node )
    {
        bool dummy;
        return LoadScale( node, dummy );

    }   // LoadScale


    /************************************************************************
    *    desc:  Load the color
    *
    *    param: node - passed in node
    ************************************************************************/
    CColor LoadColor( const XMLNode & node, const CColor & currentColor )
    {
        CColor color( currentColor );

        XMLNode colorNode = node.getChildNode( "color" );
        if( !colorNode.isEmpty() )
        {
            if( colorNode.isAttributeSet( "r" ) )
                color.r = std::atof(colorNode.getAttribute( "r" ));

            if( colorNode.isAttributeSet( "g" ) )
                color.g = std::atof(colorNode.getAttribute( "g" ));

            if( colorNode.isAttributeSet( "b" ) )
                color.b = std::atof(colorNode.getAttribute( "b" ));

            if( colorNode.isAttributeSet( "a" ) )
                color.a = std::atof(colorNode.getAttribute( "a" ));

            // Convert if in RGBA format
            color.Convert();
        }

        return color;

    }	// LoadScale


    /************************************************************************
    *    desc:  Load the size
    *
    *    param: node - passed in node
    ************************************************************************/
    CSize<int> LoadSize( const XMLNode & node )
    {
        CSize<int> size;

        XMLNode sizeNode = node.getChildNode( "size" );
        if( sizeNode.isAttributeSet("width") )
            size.w = std::atoi(sizeNode.getAttribute( "width" ));

        if( sizeNode.isAttributeSet("height") )
            size.h = std::atoi(sizeNode.getAttribute( "height" ));

        return size;

    }	// LoadSize


    /************************************************************************
    *    desc:  Load the rect
    *
    *    param: node - passed in node
    ************************************************************************/
    CRect<int> LoadRect( const XMLNode & node )
    {
        CRect<int> rect;

        XMLNode sizeNode = node.getChildNode( "rect" );
        if( sizeNode.isAttributeSet("x1") )
            rect.x1 = std::atoi(sizeNode.getAttribute( "x1" ));

        if( sizeNode.isAttributeSet("y1") )
            rect.y1 = std::atoi(sizeNode.getAttribute( "y1" ));

        if( sizeNode.isAttributeSet("x2") )
            rect.x2 = std::atoi(sizeNode.getAttribute( "x2" ));

        if( sizeNode.isAttributeSet("y2") )
            rect.y2 = std::atoi(sizeNode.getAttribute( "y2" ));

        return rect;

    }	// LoadRect


    /************************************************************************
    *    desc:  Load the rotation
    *
    *    param: node - passed in node
    ************************************************************************/
    /*pair<int,int> LoadDivisions( const XMLNode & node )
    {
        pair<int,int> divisions;

        if( node.isAttributeSet("rows") )
            divisions.first = std::atof(node.getAttribute( "rows" ));

        if( node.isAttributeSet("columns") )
            divisions.second = std::atof(node.getAttribute( "columns" ));

        return divisions;

    }*/	// LoadSize


    /************************************************************************
    *    desc:  Load the horizontal alignment
    *
    *    param: node - passed in node
    *           horzAlignment - passed in default alignment
    ************************************************************************/
    NDefs::EHorzAlignment LoadHorzAlignment( const XMLNode & node, NDefs::EHorzAlignment horzAlignment )
    {
        NDefs::EHorzAlignment horzAlign = horzAlignment;

        if( node.isAttributeSet( "horzAlign" ) )
        {
            std::string horzAlignStr = node.getAttribute( "horzAlign" );
            if( horzAlignStr == "left" )
                horzAlign = NDefs::EHA_HORZ_LEFT;

            else if( horzAlignStr == "center" )
                horzAlign = NDefs::EHA_HORZ_CENTER;

            else if( horzAlignStr == "right" )
                horzAlign = NDefs::EHA_HORZ_RIGHT;
        }

        return horzAlign;

    }	// LoadHorzAlignment


    /************************************************************************
    *    desc:  Load the vertical alignment
    *
    *    param: node - passed in node
    *           horzAlignment - passed in default alignment
    ************************************************************************/
    NDefs::EVertAlignment LoadVertAlignment( const XMLNode & node, NDefs::EVertAlignment vertAlignment )
    {
        NDefs::EVertAlignment vertAlign = vertAlignment;

        if( node.isAttributeSet( "vertAlign" ) )
        {
            std::string vertAlignStr = node.getAttribute( "vertAlign" );
            if( vertAlignStr == "top" )
                vertAlign = NDefs::EVA_VERT_TOP;

            else if( vertAlignStr == "center" )
                vertAlign = NDefs::EVA_VERT_CENTER;

            else if( vertAlignStr == "bottom" )
                vertAlign = NDefs::EVA_VERT_BOTTOM;
        }

        return vertAlign;

    }	// LoadVertAlignment


    /************************************************************************
    *    desc:  Load the dynamic offset
    *
    *    param: node - passed in node
    *           dynamicOffset - bitmask to set
    *           dynamicOffsetPt - point to set
    ************************************************************************/
    CDynamicOffset LoadDynamicOffset( const XMLNode & node )
    {
        CDynamicOffset dynamicOffset;

        XMLNode dynamicOffsetNode = node.getChildNode( "dynamicOffset" );

        if( !dynamicOffsetNode.isEmpty() )
        {
            if( dynamicOffsetNode.isAttributeSet( "left" ) )
            {
                dynamicOffset.AndBitMask( CDynamicOffset::EDO_LEFT );
                dynamicOffset.SetX( std::atof( dynamicOffsetNode.getAttribute( "left" ) ) );
            }
            else if( dynamicOffsetNode.isAttributeSet( "right" ) )
            {
                dynamicOffset.AndBitMask( CDynamicOffset::EDO_RIGHT );
                dynamicOffset.SetX( std::atof( dynamicOffsetNode.getAttribute( "right" ) ) );
            }
            else if( dynamicOffsetNode.isAttributeSet( "horzCenter" ) )
            {
                dynamicOffset.AndBitMask( CDynamicOffset::EDO_HORZ_CENTER );
                dynamicOffset.SetX( std::atof( dynamicOffsetNode.getAttribute( "horzCenter" ) ) );
            }

            if( dynamicOffsetNode.isAttributeSet( "top" ) )
            {
                dynamicOffset.AndBitMask( CDynamicOffset::EDO_TOP );
                dynamicOffset.SetY( std::atof( dynamicOffsetNode.getAttribute( "top" ) ) );
            }

            else if( dynamicOffsetNode.isAttributeSet( "bottom" ) )
            {
                dynamicOffset.AndBitMask( CDynamicOffset::EDO_BOTTOM );
                dynamicOffset.SetY( std::atof( dynamicOffsetNode.getAttribute( "bottom" ) ) );
            }
            else if( dynamicOffsetNode.isAttributeSet( "vertCenter" ) )
            {
                dynamicOffset.AndBitMask( CDynamicOffset::EDO_VERT_CENTER );
                dynamicOffset.SetX( std::atof( dynamicOffsetNode.getAttribute( "vertCenter" ) ) );
            }
        }

        return dynamicOffset;

    }	// LoadDynamicOffset

}	// NGenFunc

