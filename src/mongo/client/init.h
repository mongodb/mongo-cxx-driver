// @file init.h

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

#pragma once

#include "mongo/base/status.h"
#include "mongo/client/export_macros.h"
#include "mongo/client/options.h"

// NOTE: These functions are only intended to be used when linking against the libmongoclient
// library. The below functions are not defined in servers like mongos or mongod, which have
// their own initialization strategy.

/**
 * @namespace mongo
 * @brief the main MongoDB namespace
 */
namespace mongo {

/**
 * @namespace mongo::client
 * @brief the MongoDB C++ driver namespace
 */
namespace client {

    /**
     *  Initializes the client driver, possibly with custom options. See the Options class for
     *  details on the various fields.
     *
     *  NOTE: Do not call 'initialize' before entering 'main' (i.e. from a static initializer),
     *  as it relies on all static initialization having been completed.
     *
     *  NOTE: Do not call 'initialize' more than once.
     */
    MONGO_CLIENT_API Status MONGO_CLIENT_FUNC initialize(const Options& options = Options());

    /**
     *  Terminates the client driver. If the driver does not terminate within the currently
     *  configured grace period in the driver options, an 'ExceededTimeLimit' Status will be
     *  returned, in which case it is legal to retry 'shutdown'. Other non-OK status values do
     *  not admit retrying the operation. A permanent failure to terminate the driver should be
     *  logged, and it may be unsafe to exit the process by any mechanism which causes normal
     *  destruction of static objects.
     */
    MONGO_CLIENT_API Status MONGO_CLIENT_FUNC shutdown();

} // namespace client
} // namespace mongo
