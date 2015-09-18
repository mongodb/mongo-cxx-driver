/*    Copyright 2014 10gen Inc.
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

#include "mongo/bson/bsontypes.h"

#include "mongo/db/jsobj.h"

namespace mongo {

/* take a BSONType and return the name of that type as a char* */
const char* typeName(BSONType type) {
    switch (type) {
        case MinKey:
            return "MinKey";
        case EOO:
            return "EOO";
        case NumberDouble:
            return "NumberDouble";
        case String:
            return "String";
        case Object:
            return "Object";
        case Array:
            return "Array";
        case BinData:
            return "BinaryData";
        case Undefined:
            return "Undefined";
        case jstOID:
            return "OID";
        case Bool:
            return "Bool";
        case Date:
            return "Date";
        case jstNULL:
            return "NULL";
        case RegEx:
            return "RegEx";
        case DBRef:
            return "DBRef";
        case Code:
            return "Code";
        case Symbol:
            return "Symbol";
        case CodeWScope:
            return "CodeWScope";
        case NumberInt:
            return "NumberInt32";
        case Timestamp:
            return "Timestamp";
        case NumberLong:
            return "NumberLong64";
        // JSTypeMax doesn't make sense to turn into a string; overlaps with highest-valued type
        case MaxKey:
            return "MaxKey";
        default:
            return "Invalid";
    }
}

}  // namespace mongo
