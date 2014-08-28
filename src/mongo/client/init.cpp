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

#include "mongo/platform/basic.h"

#include "mongo/client/init.h"

#include <cstdlib>

#include "mongo/base/initializer.h"
#include "mongo/client/connpool.h"
#include "mongo/client/private/options.h"
#include "mongo/client/replica_set_monitor.h"
#include "mongo/platform/atomic_word.h"
#include "mongo/util/background.h"

namespace mongo {
namespace client {

    AtomicWord<int> isInitialized;

    namespace {
        void callShutdownAtExit() {
            // We can't really do anything if this returns a non-OK status.
            mongo::client::shutdown();
        }
    } // namespace

    Status initialize(const Options& options) {

        if (isInitialized.compareAndSwap(0, 1) == 0) {

            // Copy in the provided options.
            setOptions(options);

            if (options.callShutdownAtExit()) {
                if (std::atexit(&callShutdownAtExit) != 0) {
                    return Status(
                        ErrorCodes::InternalError,
                        "Failed setting client driver atexit shutdown handler");
                }
            }

            Status result = runGlobalInitializers(0, NULL, NULL);
            if (!result.isOK())
                return result;

            // Setup default pool parameters
            mongo::pool.setName("connection pool");
            mongo::pool.setMaxPoolSize(50);

            PeriodicTask::startRunningPeriodicTasks();

            return Status::OK();
        }
        else {
            return Status(
                ErrorCodes::IllegalOperation,
                "Initialize() may only be called once");
        }
    }

    Status shutdown() {

        if (isInitialized.compareAndSwap(1, 0) == 1) {

            ReplicaSetMonitor::cleanup();

            const unsigned int gracePeriod = Options::current().autoShutdownGracePeriodMillis();
            Status s = PeriodicTask::stopRunningPeriodicTasks(gracePeriod);

            shutdownNetworking();
            return s;
        }
        else {
            return Status(
                ErrorCodes::IllegalOperation,
                "Shutdown() cannot be called before initialize()");
        }
    }

} // namespace client
} // namespace mongo
