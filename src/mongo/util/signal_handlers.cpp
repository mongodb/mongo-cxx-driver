// signal_handlers.cpp

/*    Copyright 2014 MongoDB Inc.
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#include <cstdarg>
#include <cstdio>
#include <cstdlib>

#if !defined(_WIN32)  // TODO: windows support
#include <unistd.h>
#endif

#include "mongo/platform/backtrace.h"
#include "mongo/util/log.h"
#include "mongo/util/signal_handlers.h"

namespace mongo {

    /*
     * WARNING: PLEASE READ BEFORE CHANGING THIS MODULE
     *
     * All code in this module must be signal-friendly. Before adding any system
     * call or other dependency, please make sure that this still holds.
     *
     */

    static int rawWrite( int fd , char* c , int size ) {
#if !defined(_WIN32)

        int toWrite = size;
        int writePos = 0;
        int wrote;
        while ( toWrite > 0 ) {
            wrote = write( fd , &c[writePos] , toWrite );
            if ( wrote < 1 ) break;
            toWrite -= wrote;
            writePos += wrote;
        }
        return writePos;

#else

        return -1;

#endif
    }

    static int formattedWrite( int fd , const char* format, ... ) {
        const int MAX_ENTRY = 256;
        static char entryBuf[MAX_ENTRY];

        va_list ap;
        va_start( ap , format );
        int entrySize = vsnprintf( entryBuf , MAX_ENTRY-1 , format , ap );
        if ( entrySize < 0 ) {
            return -1;
        }

        if ( rawWrite( fd , entryBuf , entrySize ) < 0 ) {
            return -1;
        }

        return 0;
    }

    static void formattedBacktrace( int fd ) {

#if !defined(_WIN32)

        int numFrames;
        const int MAX_DEPTH = 20;
        void* stackFrames[MAX_DEPTH];

        numFrames = backtrace( stackFrames , 20 );
        for ( int i = 0; i < numFrames; i++ ) {
            formattedWrite( fd , "%p " , stackFrames[i] );
        }
        formattedWrite( fd , "\n" );

        backtrace_symbols_fd( stackFrames , numFrames , fd );

#else

        formattedWrite( fd, "backtracing not implemented for this platform yet\n" );

#endif

    }

    void printStackAndExit( int signalNum ) {
        const int fd = 1; // same as STDOUT_FILENO which doesn't exist on windows

        formattedWrite( fd , "Received signal %d\n" , signalNum );
        formattedWrite( fd , "Backtrace: " );
        formattedBacktrace( fd );
        formattedWrite( fd , "===\n" );

        ::_exit( EXIT_ABRUPT );
    }

} // namespace mongo
