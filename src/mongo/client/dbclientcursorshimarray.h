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

#include "mongo/client/dbclientcursorshim.h"
#include "mongo/bson/bsonobjiterator.h"

namespace mongo {

class DBClientCursor;

/** DBClientCursorShimArray implements the shim interface over an array of
 * bson obj's.  This is useful for providing a cursor when we receive only
 * a single batch, as in the 2.4 aggregate command reply */
class DBClientCursorShimArray : public DBClientCursorShim {
public:
    DBClientCursorShimArray(DBClientCursor& c, const std::string& arrayField);

    virtual BSONObj next();
    virtual bool more();

private:
    DBClientCursor& cursor;
    BSONObjIterator iter;
    bool has_array;
    const std::string array_field;
};
}
