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

#include "mongo/db/jsobj.h"

namespace mongo {

    /**
     * "Types" are the interface to a known data structure that will be serialized to and
     * deserialized from BSON.
     */
    class BSONSerializable {
    public:

        virtual ~BSONSerializable() {}

        /**
         * Returns true if all the mandatory fields are present and have valid
         * representations. Otherwise returns false and fills in the optional 'errMsg' string.
         */
        virtual bool isValid( std::string* errMsg ) const = 0;

        /** Returns the BSON representation of the entry. */
        virtual BSONObj toBSON() const = 0;

        /**
         * Clears and populates the internal state using the 'source' BSON object if the
         * latter contains valid values. Otherwise sets errMsg and returns false.
         */
        virtual bool parseBSON( const BSONObj& source, std::string* errMsg ) = 0;

        /** Clears the internal state. */
        virtual void clear() = 0;

        /** Returns a string representation of the current internal state. */
        virtual std::string toString() const = 0;
    };

} // namespace mongo
