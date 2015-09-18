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

/** DBClientCursorShimCursorID implements the shim interface over a cursor
 * reply document, rather than the traditional OP_REPLY.  It additionally
 * handles a possible initial firstBatch which is part of the 2.6 aggregate
 * command api. */
class DBClientCursorShimCursorID : public DBClientCursorShim {
public:
    DBClientCursorShimCursorID(DBClientCursor& c);

    virtual BSONObj next();
    virtual bool more();

    BSONObj get_cursor();

private:
    DBClientCursor& cursor;
    BSONObjIterator iter;
    bool in_first_batch;
};
}
