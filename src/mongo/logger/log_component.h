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

#include <string>

#include "mongo/client/export_macros.h"

namespace mongo {
namespace logger {

    /**
     * Log components.
     * Debug messages logged using the LOG() or MONGO_LOG_COMPONENT().
     * Macros may be associated with one or more log components.
     */
    class MONGO_CLIENT_API LogComponent {
    public:
        enum Value {
            kDefault = 0,
            kAccessControl,
            kCommands,
            kIndexing,
            kJournalling,
            kNetworking,
            kQuery,
            kReplication,
            kSharding,
            kStorage,
            kWrites,
            kNumLogComponents
        };

        /* implicit */ LogComponent(Value value) : _value(value) {}

        operator Value() const { return _value; }

        /**
         * Returns short name of log component.
         * Used to generate server parameter names in the format "logLevel_<component short name>".
         */
        std::string getShortName() const;

    private:
        Value _value;
    };

}  // namespace logger
}  // namespace mongo
