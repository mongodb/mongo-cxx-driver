/*    Copyright 2013 10gen Inc.
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

#pragma once

#include "mongo/db/jsobj.h"
#include "mongo/platform/process_id.h"

namespace mongo {

    const int DEFAULT_MAX_CONN = 1000000;

    struct ServerGlobalParams {

        ServerGlobalParams() :
            port(DefaultDBPort), rest(false), jsonp(false), indexBuildRetry(true), quiet(false),
            configsvr(false), cpu(false), objcheck(true), defaultProfile(0),
            slowMS(100), defaultLocalThresholdMillis(15), moveParanoia(true),
            noUnixSocket(false), doFork(0), socket("/tmp"), maxConns(DEFAULT_MAX_CONN),
            logAppend(false), logWithSyslog(false), isHttpInterfaceEnabled(false)
        {
            started = time(0);
        }

        std::string binaryName;     // mongod or mongos
        std::string cwd;            // cwd of when process started

        int port;              // --port
        enum {
            DefaultDBPort = 27017,
            ConfigServerPort = 27019,
            ShardServerPort = 27018
        };
        bool isDefaultPort() const { return port == DefaultDBPort; }

        std::string bind_ip;        // --bind_ip
        bool rest;             // --rest
        bool jsonp;            // --jsonp

        bool indexBuildRetry;  // --noIndexBuildRetry

        bool quiet;            // --quiet

        bool configsvr;        // --configsvr

        bool cpu;              // --cpu show cpu time periodically

        bool objcheck;         // --objcheck

        int defaultProfile;    // --profile
        int slowMS;            // --time in ms that is "slow"
        int defaultLocalThresholdMillis;    // --localThreshold in ms to consider a node local
        bool moveParanoia;     // for move chunk paranoia

        bool noUnixSocket;     // --nounixsocket
        bool doFork;           // --fork
        std::string socket;    // UNIX domain socket directory

        int maxConns;          // Maximum number of simultaneous open connections.

        std::string keyFile;   // Path to keyfile, or empty if none.
        std::string pidFile;   // Path to pid file, or empty if none.

        std::string logpath;   // Path to log file, if logging to a file; otherwise, empty.
        bool logAppend;        // True if logging to a file in append mode.
        bool logWithSyslog;    // True if logging to syslog; must not be set if logpath is set.
        int syslogFacility;    // Facility used when appending messages to the syslog.

        bool isHttpInterfaceEnabled; // True if the dbwebserver should be enabled.

#ifndef _WIN32
        ProcessId parentProc;      // --fork pid of initial process
        ProcessId leaderProc;      // --fork pid of leader process
#endif

        /**
         * Switches to enable experimental (unsupported) features.
         */
        struct ExperimentalFeatures {
            ExperimentalFeatures()
                : indexStatsCmdEnabled(false)
                , storageDetailsCmdEnabled(false)
            {}
            bool indexStatsCmdEnabled; // -- enableExperimentalIndexStatsCmd
            bool storageDetailsCmdEnabled; // -- enableExperimentalStorageDetailsCmd
        } experimental;

        time_t started;

        BSONArray argvArray;
        BSONObj parsedOpts;
        AtomicInt32 clusterAuthMode;    // --clusterAuthMode, the internal cluster auth mode

        enum ClusterAuthModes {
            ClusterAuthMode_undefined,
            /** 
            * Authenticate using keyfile, accept only keyfiles
            */
            ClusterAuthMode_keyFile,

            /**
            * Authenticate using keyfile, accept both keyfiles and X.509
            */
            ClusterAuthMode_sendKeyFile,

            /**
            * Authenticate using X.509, accept both keyfiles and X.509
            */
            ClusterAuthMode_sendX509,

            /**
            * Authenticate using X.509, accept only X.509
            */
            ClusterAuthMode_x509
        };
    };

    extern ServerGlobalParams serverGlobalParams;
}
