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

#include "mongo/logger/log_component.h"

#include "mongo/util/assert_util.h"

namespace mongo {
namespace logger {

    std::string LogComponent::getShortName() const {
        switch (_value) {
        case kDefault: return "Default";
        case kAccessControl: return "AccessControl";
        case kCommands: return "Commands";
        case kIndexing: return "Indexing";
        case kJournalling: return "Journalling";
        case kNetworking: return "Networking";
        case kQuery: return "Query";
        case kReplication: return "Replication";
        case kSharding: return "Sharding";
        case kStorage: return "Storage";
        case kWrites: return "Writes";
        case kNumLogComponents: return "Total";
        // No default. Compiler should complain if there's a log component that's not handled.
        }
        invariant(0);
    }

}  // logger
}  // mongo
