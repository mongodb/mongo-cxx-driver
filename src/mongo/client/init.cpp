/* Copyright 2013 10gen Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define MONGO_LOG_DEFAULT_COMPONENT ::mongo::logger::LogComponent::kDefault;

#include "mongo/platform/basic.h"

#include "mongo/client/init.h"

#include <cstdlib>

#include "mongo/base/initializer.h"
#include "mongo/client/private/options.h"
#include "mongo/client/replica_set_monitor.h"
#include "mongo/platform/atomic_word.h"
#include "mongo/stdx/functional.h"
#include "mongo/util/background.h"
#include "mongo/util/log.h"
#include "mongo/util/net/sock.h"

namespace mongo {
namespace client {

namespace {

// 0 = not yet initialized
// 1 = initialized, not yet shut down
// -1 = terminated
AtomicWord<int> isInitialized;

void callShutdownAtExit() {
    // We can't really do anything if this returns a non-OK status.
    mongo::client::shutdown();
}

void configureLogging(const Options& opts) {
    const mongo::client::Options::LogAppenderFactory& appenderFactory = opts.logAppenderFactory();

    if (appenderFactory) {
        logger::ComponentMessageLogDomain* globalLogDomain =
            logger::globalLogManager()->getGlobalDomain();

        globalLogDomain->attachAppender(appenderFactory());
        globalLogDomain->setMinimumLoggedSeverity(opts.minLoggedSeverity());
    }
}

}  // namespace

Status initialize(const Options& options) {
    int initStatus = isInitialized.compareAndSwap(0, 1);

    if (initStatus == 0) {
        // Copy in the provided options.
        setOptions(options);
        configureLogging(options);

#if defined(_WIN32) && !defined(STATIC_LIBMONGOCLIENT)
        if (options.callShutdownAtExit()) {
            return Status(
                ErrorCodes::IllegalOperation,
                "The DLL build of the MongoDB C++ driver does not support shutdown at exit");
        }
#endif

        if (options.callShutdownAtExit()) {
            if (std::atexit(&callShutdownAtExit) != 0) {
                return Status(ErrorCodes::InternalError,
                              "Failed setting client driver atexit shutdown handler");
            }
        }

        enableIPv6(options.IPv6Enabled());

        Status result = runGlobalInitializers(0, NULL, NULL);
        if (!result.isOK())
            return result;

        result = ReplicaSetMonitor::initialize();
        if (!result.isOK()) {
            return result;
        }

        return Status::OK();
    } else if (initStatus == 1) {
        return Status(ErrorCodes::AlreadyInitialized, "Initialize() may only be called once");
    } else {
        return Status(ErrorCodes::IllegalOperation, "The driver has been terminated.");
    }
}

Status shutdown() {
    int initStatus = isInitialized.compareAndSwap(1, -1);

    if (initStatus == 1) {
        Status result =
            ReplicaSetMonitor::shutdown(Options::current().autoShutdownGracePeriodMillis());
        if (!result.isOK()) {
            if (result == ErrorCodes::ExceededTimeLimit) {
                return result;
            }
            warning() << "The ReplicaSetMonitor was shutdown prior to driver termination. "
                      << "This is a non-fatal error that can occur if you are calling "
                      << "ReplicaSetMonitor::shutdown() manually." << std::endl;
        }
        shutdownNetworking();
        return Status::OK();
    } else if (initStatus == 0) {
        return Status(
            ErrorCodes::IllegalOperation,
            "mongo::client::shutdown() cannot be called before mongo::client::initialize()");
    } else {
        return Status(ErrorCodes::IllegalOperation, "The driver has been terminated.");
    }
}

GlobalInstance::GlobalInstance(const Options& options)
    : _terminateNeeded(false), _status(initialize(options)) {
    if (initialized())
        _terminateNeeded = !Options::current().callShutdownAtExit();
}

GlobalInstance::~GlobalInstance() {
    if (_terminateNeeded && !client::shutdown().isOK())
        std::abort();
}

void GlobalInstance::assertInitialized() const {
    uassertStatusOK(status());
}

Status GlobalInstance::shutdown() {
    const Status result = client::shutdown();
    if (result.isOK())
        _terminateNeeded = false;
    return result;
}

}  // namespace client
}  // namespace mongo
