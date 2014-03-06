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

#include "mongo/util/fail_point_service.h"

namespace mongo {
    MONGO_FP_DECLARE(dummy); // used by jstests/libs/fail_point.js

    scoped_ptr<FailPointRegistry> _fpRegistry(NULL);

    MONGO_INITIALIZER(FailPointRegistry)(InitializerContext* context) {
        _fpRegistry.reset(new FailPointRegistry());
        return Status::OK();
    }

    MONGO_INITIALIZER_GENERAL(AllFailPointsRegistered,
                              MONGO_NO_PREREQUISITES,
                              MONGO_NO_DEPENDENTS)(InitializerContext* context) {
        _fpRegistry->freeze();
        return Status::OK();
    }

    FailPointRegistry* getGlobalFailPointRegistry() {
        return _fpRegistry.get();
    }
}

MONGO_INITIALIZER_FUNCTION_ASSURE_FILE(util_fail_point_service)
