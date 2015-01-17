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

#pragma once

#include "mongo/base/init.h"
#include "mongo/util/fail_point_registry.h"

namespace mongo {

    /**
     * @return the global fail point registry.
     */
    FailPointRegistry* getGlobalFailPointRegistry();

    /**
     * Convenience macro for declaring a fail point. Must be used in global scope and never in a
     * block with limited scope (ie, inside functions, loops, etc.).
     *
     * NOTE: Never use in header files, only sources.
     */
    #define MONGO_FP_DECLARE(fp) FailPoint fp; \
        MONGO_INITIALIZER_GENERAL(fp, ("FailPointRegistry"), ("AllFailPointsRegistered")) \
                (::mongo::InitializerContext* context) { \
            return getGlobalFailPointRegistry()->addFailPoint(#fp, &fp); \
        }

    /**
     * Convenience macro for defining a fail point in a header scope.
     */
    #define MONGO_FP_FORWARD_DECLARE(fp) extern FailPoint fp;

}
