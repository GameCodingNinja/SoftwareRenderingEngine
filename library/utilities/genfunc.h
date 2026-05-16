/************************************************************************
*    FILE NAME:       genfunc.h
*
*    DESCRIPTION:     General mutipurpose functions
************************************************************************/           

#ifndef __genfunc_h__
#define __genfunc_h__

// Standard lib dependencies
#include <string>
#include <memory>
#include <cstdio>
#include <cassert>

namespace NGenFunc
{
    namespace detail
    {
        // Convert std::string to const char* for printf-style formatting
        inline const char* toFmtArg(const std::string& s) { return s.c_str(); }

        // Pass all other types through unchanged
        template<typename T>
        inline T toFmtArg(T v) { return v; }
    }

    // Printf-style string formatting
    template<typename... Args>
    std::string FormatString(const char* fmt, Args&&... args)
    {
        int size = std::snprintf(nullptr, 0, fmt, detail::toFmtArg(args)...);
        if(size < 0) return fmt;
        std::string result(size + 1, '\0');
        std::snprintf(&result[0], size + 1, fmt, detail::toFmtArg(args)...);
        result.resize(size);
        return result;
    }

    // Count the number of occurrences of sub string
    int CountStrOccurrence( const std::string & searchStr, const std::string & subStr );

    // Read in a file and return it as a buffer
    std::shared_ptr<char[]> FileToBuf( const std::string & file );
    std::shared_ptr<char[]> FileToBuf( const std::string & file, size_t & sizeInBytes );

    // Output string info
    void PostDebugMsg( const std::string & msg );

    // Dispatch and event
    int DispatchEvent( int type, int code = 0, void * pData1 = nullptr, void * pData2 = nullptr );
    
    // dynamic_cast type with error checking
    template <class target, class source>
    inline target DynCast( source * ptr )
    {
        target result = dynamic_cast<target>(ptr);
        assert(result != nullptr);
        return result;
    }

    // dynamic_cast type with error checking
    template <class target, class source>
    inline target ReintCast( source * ptr )
    {
        target result = reinterpret_cast<target>(ptr);
        assert(result != nullptr);
        return result;
    }
}

#endif  // __genfunc_h__
