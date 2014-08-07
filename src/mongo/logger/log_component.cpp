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

#include <boost/static_assert.hpp>

#include "mongo/base/init.h"
#include "mongo/util/assert_util.h"

namespace mongo {
namespace logger {

namespace {

// Component dotted names.
// Lazily evaluated in LogComponent::getDottedName().
std::string _dottedNames[LogComponent::kNumLogComponents+1];

    //
    // Fully initialize _dottedNames before we enter multithreaded execution.
    //

    MONGO_INITIALIZER_WITH_PREREQUISITES(SetupDottedNames, MONGO_NO_PREREQUISITES)(
            InitializerContext* context) {

        for (int i = 0; i <= int(LogComponent::kNumLogComponents); ++i) {
            logger::LogComponent component = static_cast<logger::LogComponent::Value>(i);
            component.getDottedName();
        }

        return Status::OK();
    }

}  // namespace

// Children always come after parent component.
// This makes it unnecessary to compute children of each component
// when setting/clearing log severities in LogComponentSettings.
#define DECLARE_LOG_COMPONENT_PARENT(CHILD, PARENT) \
    case (CHILD): \
        do { \
            BOOST_STATIC_ASSERT(int(CHILD) > int(PARENT)); \
            return (PARENT); \
        } while (0)

    LogComponent LogComponent::parent() const {
        switch (_value) {
        case kDefault: return kNumLogComponents;
        DECLARE_LOG_COMPONENT_PARENT(kJournaling, kStorage);
        case kNumLogComponents: return kNumLogComponents;
        default: return kDefault;
        }
        invariant(0);
    }

    std::string LogComponent::getShortName() const {
        switch (_value) {
        case kDefault: return "default";
        case kAccessControl: return "accessControl";
        case kCommands: return "commands";
        case kIndexing: return "indexing";
        case kNetworking: return "networking";
        case kQuery: return "query";
        case kReplication: return "replication";
        case kSharding: return "sharding";
        case kStorage: return "storage";
        case kJournaling: return "journaling";
        case kWrites: return "writes";
        case kNumLogComponents: return "total";
        // No default. Compiler should complain if there's a log component that's not handled.
        }
        invariant(0);
    }

    std::string LogComponent::getDottedName() const {
        // Lazily evaluate dotted names in anonymous namespace.
        if (_dottedNames[_value].empty()) {
            switch (_value) {
            case kDefault: _dottedNames[_value] = getShortName(); break;
            case kNumLogComponents: _dottedNames[_value] = getShortName(); break;
            default:
                // Omit short name of 'default' component from dotted name.
                if (parent() == kDefault) {
                    _dottedNames[_value] = getShortName();
                }
                else {
                    _dottedNames[_value] = parent().getDottedName() + "." + getShortName();
                }
                break;
            }
        }
        return _dottedNames[_value];
    }

}  // logger
}  // mongo
