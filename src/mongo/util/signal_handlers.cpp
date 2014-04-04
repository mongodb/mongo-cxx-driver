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

#include "mongo/platform/basic.h"

#include "mongo/util/signal_handlers.h"

#include <boost/thread.hpp>

#include "mongo/db/client.h"
#include "mongo/db/log_process_details.h"
#include "mongo/util/assert_util.h"
#include "mongo/util/exit_code.h"
#include "mongo/util/log.h"
#include "mongo/util/scopeguard.h"
#include "mongo/util/stacktrace.h"

#if defined(_WIN32)
#  include "mongo/util/signal_win32.h"
#  include "mongo/util/exception_filter_win32.h"
#else
#  include <signal.h>
#  include <unistd.h>
#endif

#if defined(_WIN32)
namespace {
    const char* strsignal(int signalNum) {
        // should only see SIGABRT on windows
        switch (signalNum) {
        case SIGABRT: return "SIGABRT";
        default: return "UNKNOWN";
        }
    }
}
#endif

namespace mongo {

    /*
     * WARNING: PLEASE READ BEFORE CHANGING THIS MODULE
     *
     * All code in this module must be signal-friendly. Before adding any system
     * call or other dependency, please make sure that this still holds.
     *
     * All code in this file follows this pattern:
     *   Generic code
     *   #ifdef _WIN32
     *       Windows code
     *   #else
     *       Posix code
     *   #endif
     *
     */

    // everything provides this, but only header is instance.h
    void exitCleanly(ExitCode exitCode);

namespace {

    // this will be called in certain c++ error cases, for example if there are two active
    // exceptions
    void myTerminate() {
        printStackTrace(severe().stream()
                        << "terminate() called, printing stack (if implemented for platform):\n");
        ::_exit(EXIT_ABRUPT);
    }

    // this gets called when new fails to allocate memory
    void myNewHandler() {
        printStackTrace(severe().stream() << "out of memory, printing stack and exiting:\n");
        ::_exit(EXIT_ABRUPT);
    }

    void abruptQuit(int signalNum) {
        {
            LogstreamBuilder logBuilder = severe();
            logBuilder <<
                "Got signal: " << signalNum << " (" << strsignal(signalNum) << ").\nBacktrace:";
            printStackTrace(logBuilder.stream());
        }

        // Don't go through normal shutdown procedure. It may make things worse.
        ::_exit(EXIT_ABRUPT);
    }

#ifdef _WIN32

    void consoleTerminate( const char* controlCodeName ) {
        Client::initThread( "consoleTerminate" );
        log() << "got " << controlCodeName << ", will terminate after current cmd ends" << endl;
        exitCleanly( EXIT_KILL );
    }

    BOOL WINAPI CtrlHandler( DWORD fdwCtrlType ) {

        switch( fdwCtrlType ) {

        case CTRL_C_EVENT:
            log() << "Ctrl-C signal";
            consoleTerminate( "CTRL_C_EVENT" );
            return TRUE ;

        case CTRL_CLOSE_EVENT:
            log() << "CTRL_CLOSE_EVENT signal";
            consoleTerminate( "CTRL_CLOSE_EVENT" );
            return TRUE ;

        case CTRL_BREAK_EVENT:
            log() << "CTRL_BREAK_EVENT signal";
            consoleTerminate( "CTRL_BREAK_EVENT" );
            return TRUE;

        case CTRL_LOGOFF_EVENT:
            // only sent to services, and only in pre-Vista Windows; FALSE means ignore
            return FALSE;

        case CTRL_SHUTDOWN_EVENT:
            log() << "CTRL_SHUTDOWN_EVENT signal";
            consoleTerminate( "CTRL_SHUTDOWN_EVENT" );
            return TRUE;

        default:
            return FALSE;
        }
    }

    void eventProcessingThread() {
        std::string eventName = getShutdownSignalName(ProcessId::getCurrent().asUInt32());

        HANDLE event = CreateEventA(NULL, TRUE, FALSE, eventName.c_str());
        if (event == NULL) {
            warning() << "eventProcessingThread CreateEvent failed: "
                << errnoWithDescription();
            return;
        }

        ON_BLOCK_EXIT(CloseHandle, event);

        int returnCode = WaitForSingleObject(event, INFINITE);
        if (returnCode != WAIT_OBJECT_0) {
            if (returnCode == WAIT_FAILED) {
                warning() << "eventProcessingThread WaitForSingleObject failed: "
                    << errnoWithDescription();
                return;
            }
            else {
                warning() << "eventProcessingThread WaitForSingleObject failed: "
                    << errnoWithDescription(returnCode);
                return;
            }
        }

        Client::initThread("eventTerminate");
        log() << "shutdown event signaled, will terminate after current cmd ends";
        exitCleanly(EXIT_CLEAN);
    }

#else

    void abruptQuitWithAddrSignal( int signalNum, siginfo_t *siginfo, void * ) {
        {
            LogstreamBuilder logBuilder = severe();

            logBuilder << "Invalid";
            if ( signalNum == SIGSEGV || signalNum == SIGBUS ) {
                logBuilder << " access";
            } else {
                logBuilder << " operation";
            }
            logBuilder << " at address: " << siginfo->si_addr;
        }
        abruptQuit( signalNum );
    }

    // The signals in asyncSignals will be processed by this thread only, in order to
    // ensure the db and log mutexes aren't held.
    sigset_t asyncSignals;
    void signalProcessingThread() {
        Client::initThread( "signalProcessingThread" );
        while (true) {
            int actualSignal = 0;
            int status = sigwait( &asyncSignals, &actualSignal );
            fassert(16781, status == 0);
            switch (actualSignal) {
            case SIGUSR1:
                // log rotate signal
                fassert(16782, rotateLogs());
                logProcessDetailsForLogRotate();
                break;
            case SIGQUIT:
                log() << "Received SIGQUIT; terminating.";
                _exit(EXIT_ABRUPT);
            default:
                // interrupt/terminate signal
                log() << "got signal " << actualSignal << " (" << strsignal( actualSignal )
                      << "), will terminate after current cmd ends" << endl;
                exitCleanly( EXIT_CLEAN );
                break;
            }
        }
    }
#endif
} // namespace

    void setupSignalHandlers() {
        set_terminate( myTerminate );
        set_new_handler( myNewHandler );

        // SIGABRT is the only signal we want handled by signal handlers on both windows and posix.
        invariant( signal(SIGABRT, abruptQuit) != SIG_ERR );

#ifdef _WIN32
        _set_purecall_handler( ::abort ); // TODO improve?
        setWindowsUnhandledExceptionFilter();
        massert(10297,
                "Couldn't register Windows Ctrl-C handler",
                SetConsoleCtrlHandler(static_cast<PHANDLER_ROUTINE>(CtrlHandler), TRUE));

#else
        invariant( signal(SIGHUP , SIG_IGN ) != SIG_ERR );
        invariant( signal(SIGUSR2, SIG_IGN ) != SIG_ERR );
        invariant( signal(SIGPIPE, SIG_IGN) != SIG_ERR );

        struct sigaction addrSignals;
        memset( &addrSignals, 0, sizeof( struct sigaction ) );
        addrSignals.sa_sigaction = abruptQuitWithAddrSignal;
        sigemptyset( &addrSignals.sa_mask );
        addrSignals.sa_flags = SA_SIGINFO;

        invariant( sigaction(SIGSEGV, &addrSignals, 0) == 0 );
        invariant( sigaction(SIGBUS, &addrSignals, 0) == 0 );
        invariant( sigaction(SIGILL, &addrSignals, 0) == 0 );
        invariant( sigaction(SIGFPE, &addrSignals, 0) == 0 );


        setupSIGTRAPforGDB();

        // asyncSignals is a global variable listing the signals that should be handled by the
        // interrupt thread, once it is started via startSignalProcessingThread().
        sigemptyset( &asyncSignals );
        sigaddset( &asyncSignals, SIGHUP );
        sigaddset( &asyncSignals, SIGINT );
        sigaddset( &asyncSignals, SIGTERM );
        sigaddset( &asyncSignals, SIGQUIT );
        sigaddset( &asyncSignals, SIGUSR1 );
        sigaddset( &asyncSignals, SIGXCPU );
#endif
    }

    void startSignalProcessingThread() {
#ifdef _WIN32
        boost::thread(eventProcessingThread).detach();
#else
        // Mask signals in the current (only) thread. All new threads will inherit this mask.
        invariant( pthread_sigmask( SIG_SETMASK, &asyncSignals, 0 ) == 0 );
        // Spawn a thread to capture the signals we just masked off.
        boost::thread( signalProcessingThread ).detach();
#endif
    }

} // namespace mongo
