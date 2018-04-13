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

#include <iosfwd>
#include <string>

#include "mongo/base/string_data.h"
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
        kCommand,
        kControl,
        kGeo,
        kIndex,
        kNetwork,
        kQuery,
        kReplication,
        kSharding,
        kStorage,
        kJournal,
        kWrite,
        kNumLogComponents
    };

    /* implicit */ LogComponent(Value value) : _value(value) {}

    operator Value() const {
        return _value;
    }

    /**
     * Returns parent component.
     * Returns kNumComponents if parent component is not defined (for kDefault or
     * kNumLogComponents).
     */
    LogComponent parent() const;

    /**
     * Returns short name as a StringData.
     */
    StringData toStringData() const;

    /**
     * Returns short name of log component.
     * Used to generate server parameter names in the format "logLevel_<component short name>".
     */
    std::string getShortName() const;

    /**
     * Returns dotted name of log component - short name prefixed by dot-separated names of
     * ancestors.
     * Used to generate command line and config file option names.
     */
    std::string getDottedName() const;

    /**
     * Returns name suitable for inclusion in formatted log message.
     * This is derived from upper-casing the short name with some padding to
     * fit into a fixed length field.
     */
    StringData getNameForLog() const;

private:
    Value _value;
};

MONGO_CLIENT_API std::ostream& MONGO_CLIENT_FUNC
operator<<(std::ostream& os, LogComponent component);

}  // namespace logger
}  // namespace mongo
