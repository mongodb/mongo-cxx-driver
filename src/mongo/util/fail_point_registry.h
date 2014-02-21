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

/**
 * Should NOT be included by other header files.  Include only in source files.
 */

#pragma once

#include "mongo/base/status.h"
#include "mongo/platform/unordered_map.h"
#include "mongo/util/fail_point.h"

namespace mongo {
    /**
     * Class for storing FailPoint instances.
     */
    class FailPointRegistry {
    public:
        FailPointRegistry();

        /**
         * Adds a new fail point to this registry. Duplicate names are not allowed.
         *
         * @return the status code under these circumstances:
         *     OK - if successful.
         *     DuplicateKey - if the given name already exists in this registry.
         *     CannotMutateObject - if this registry is already frozen.
         */
        Status addFailPoint(const std::string& name, FailPoint* failPoint);

        /**
         * @return the fail point object registered. Returns NULL if it was not registered.
         */
        FailPoint* getFailPoint(const std::string& name) const;

        /**
         * Freezes this registry from being modified.
         */
        void freeze();

    private:
        bool _frozen;
        unordered_map<std::string, FailPoint*> _fpMap;
    };
}

