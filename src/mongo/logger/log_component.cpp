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
std::string _dottedNames[LogComponent::kNumLogComponents + 1];

/**
 * Returns StringData created from a string literal
 */
template <size_t N>
StringData createStringData(const char(&val)[N]) {
    return StringData(val, StringData::LiteralTag());
}

//
// Fully initialize _dottedNames before we enter multithreaded execution.
//

MONGO_INITIALIZER_WITH_PREREQUISITES(SetupDottedNames,
                                     MONGO_NO_PREREQUISITES)(InitializerContext* context) {
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
#define DECLARE_LOG_COMPONENT_PARENT(CHILD, PARENT)        \
    case (CHILD):                                          \
        do {                                               \
            BOOST_STATIC_ASSERT(int(CHILD) > int(PARENT)); \
            return (PARENT);                               \
        } while (0)

LogComponent LogComponent::parent() const {
    switch (_value) {
        case kDefault:
            return kNumLogComponents;
            DECLARE_LOG_COMPONENT_PARENT(kJournal, kStorage);
        case kNumLogComponents:
            return kNumLogComponents;
        default:
            return kDefault;
    }
    invariant(false);
}

StringData LogComponent::toStringData() const {
    switch (_value) {
        case kDefault:
            return createStringData("default");
        case kAccessControl:
            return createStringData("accessControl");
        case kCommand:
            return createStringData("command");
        case kControl:
            return createStringData("control");
        case kGeo:
            return createStringData("geo");
        case kIndex:
            return createStringData("index");
        case kNetwork:
            return createStringData("network");
        case kQuery:
            return createStringData("query");
        case kReplication:
            return createStringData("replication");
        case kSharding:
            return createStringData("sharding");
        case kStorage:
            return createStringData("storage");
        case kJournal:
            return createStringData("journal");
        case kWrite:
            return createStringData("write");
        case kNumLogComponents:
            return createStringData("total");
            // No default. Compiler should complain if there's a log component that's not handled.
    }
    invariant(false);
}

std::string LogComponent::getShortName() const {
    return toStringData().toString();
}

std::string LogComponent::getDottedName() const {
    // Lazily evaluate dotted names in anonymous namespace.
    if (_dottedNames[_value].empty()) {
        switch (_value) {
            case kDefault:
                _dottedNames[_value] = getShortName();
                break;
            case kNumLogComponents:
                _dottedNames[_value] = getShortName();
                break;
            default:
                // Omit short name of 'default' component from dotted name.
                if (parent() == kDefault) {
                    _dottedNames[_value] = getShortName();
                } else {
                    _dottedNames[_value] = parent().getDottedName() + "." + getShortName();
                }
                break;
        }
    }
    return _dottedNames[_value];
}

StringData LogComponent::getNameForLog() const {
    switch (_value) {
        case kDefault:
            return createStringData("-       ");
        case kAccessControl:
            return createStringData("ACCESS  ");
        case kCommand:
            return createStringData("COMMAND ");
        case kControl:
            return createStringData("CONTROL ");
        case kGeo:
            return createStringData("GEO     ");
        case kIndex:
            return createStringData("INDEX   ");
        case kNetwork:
            return createStringData("NETWORK ");
        case kQuery:
            return createStringData("QUERY   ");
        case kReplication:
            return createStringData("REPL    ");
        case kSharding:
            return createStringData("SHARDING");
        case kStorage:
            return createStringData("STORAGE ");
        case kJournal:
            return createStringData("JOURNAL ");
        case kWrite:
            return createStringData("WRITE   ");
        case kNumLogComponents:
            return createStringData("TOTAL   ");
            // No default. Compiler should complain if there's a log component that's not handled.
    }
    invariant(false);
}

std::ostream& operator<<(std::ostream& os, LogComponent component) {
    return os << component.getNameForLog();
}

}  // logger
}  // mongo
