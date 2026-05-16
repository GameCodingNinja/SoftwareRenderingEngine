/************************************************************************
*    FILE NAME:       genfunc.cpp
*
*    DESCRIPTION:     General mutipurpose functions
************************************************************************/           

#ifdef _WIN32
#define STRICT
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

// Physical component dependency
#include <utilities/genfunc.h>

// Game lib dependencies
#include <system/eventqueue.h>
#include <system/event.h>

// Standard lib dependencies
#include <stdlib.h>  
#include <stdio.h>
#include <iostream>
#include <algorithm>
#include <cstring>

// Game lib dependencies
#include <utilities/smartpointers.h>
#include <utilities/exceptionhandling.h>

// disable warning about unsafe functions. Can't use allocated arrays with the safe functions
#ifdef _MSC_VER
#pragma warning(disable : 4996)
#endif

namespace NGenFunc
{

    /************************************************************************
    *    desc:  Count the number of occurrences of sub string
    ************************************************************************/
    int CountStrOccurrence( const std::string & searchStr, const std::string & subStr )
    {
        int result = 0;
        size_t found = std::string::npos;

        do
        {
            found = searchStr.find(subStr, found+1);

            if( found != std::string::npos )
                ++result;
        }
        while( found != std::string::npos );

        return result;

    }	// CountStrOccurrence


    /************************************************************************
    *    desc:  Read in a file and return it as a buffer
    ************************************************************************/
    std::shared_ptr<char[]> FileToBuf( const std::string & file, size_t & sizeInBytes )
    {
        const size_t TERMINATOR_SIZE(1);

        // Open file for reading
        NSmart::scoped_filehandle_ptr<FILE> scpFile( fopen(file.c_str(), "rb") );
        if( scpFile.isNull() )
            throw NExcept::CCriticalException("File Load Error!",
                NGenFunc::FormatString("Error Loading file (%s).\n\n%s\nLine: %d", file, __FUNCTION__, __LINE__));

        // Seek to the end of the file to find out how many 
        // bytes into the file we are and add one for temination
        fseek( scpFile.get(), 0, SEEK_END );
        sizeInBytes = ftell( scpFile.get() ) + TERMINATOR_SIZE;

        // Allocate a buffer for the entire length
        // of the file and a null termination
        std::shared_ptr<char[]> spChar( new char[sizeInBytes] );

        // zero out the string
        memset(spChar.get(), 0, sizeInBytes);

        // Go back to the beginning of the file and 
        // read the contents of the file in to the buffer
        fseek( scpFile.get(), 0, SEEK_SET );
        fread( spChar.get(), sizeInBytes-TERMINATOR_SIZE, 1, scpFile.get() );

        return spChar;

    }	// FileToBuf

    std::shared_ptr<char[]> FileToBuf( const std::string & file )
    {
        size_t sizeInBytes;
        return FileToBuf( file, sizeInBytes );
    }


    /************************************************************************
    *    desc:  Dispatch and event
    *
    * _event.user.code = (code4 << 24) | (code3 << 16) | (code2 << 8) | code1;
    ************************************************************************/
    int DispatchEvent( int type, int code, void * pData1, void * pData2 )
    {
        CEvent event;
        std::memset(&event, 0, sizeof(event));
        event.user.type = static_cast<EEventType>(type);
        event.user.code = code;
        event.user.data1 = pData1;
        event.user.data2 = pData2;
        CEventQueue::Instance().PushEvent(event);
        return 0;

    }   // DispatchEvent


    /************************************************************************
    *    desc: Output string info
    ************************************************************************/
    void PostDebugMsg( const std::string & msg )
    {
    #ifdef _WIN32
        std::string tmp = msg + "\n";
        OutputDebugString( tmp.c_str() );
    #else
        std::cout << msg << std::endl;
    #endif

    }   // PostDebugMsg

}	// NGenFunc
