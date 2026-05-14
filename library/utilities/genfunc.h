/************************************************************************
*    FILE NAME:       genfunc.h
*
*    DESCRIPTION:     General mutipurpose functions
************************************************************************/           

#ifndef __genfunc_h__
#define __genfunc_h__

// Standard lib dependencies
#include <string>

// Boost lib dependencies
#include <boost/shared_array.hpp>

// Standard lib dependencies
#include <assert.h>

namespace NGenFunc
{
    // Count the number of occurrences of sub string
    int CountStrOccurrence( const std::string & searchStr, const std::string & subStr );

    // Read in a file and return it as a buffer
    boost::shared_array<char> FileToBuf( const std::string & file );
    boost::shared_array<char> FileToBuf( const std::string & file, size_t & sizeInBytes );

    // Output string info
    void PostDebugMsg( const std::string & msg );

    // Dispatch and event
    int DispatchEvent( int type, int code = 0, void * pData1 = NULL, void * pData2 = NULL );
    
    // dynamic_cast type with error checking
    template <class target, class source>
    inline target DynCast( source * ptr )
    {
        target result = dynamic_cast<target>(ptr);
        assert(result != NULL);
        return result;
    }

    // dynamic_cast type with error checking
    template <class target, class source>
    inline target ReintCast( source * ptr )
    {
        target result = reinterpret_cast<target>(ptr);
        assert(result != NULL);
        return result;
    }
}

#endif  // __genfunc_h__
