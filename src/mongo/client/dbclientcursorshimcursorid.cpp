/*    Copyright 2014 Mongodb Inc.
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

#include "mongo/client/dbclientcursor.h"
#include "mongo/client/dbclientcursorshimarray.h"
#include "mongo/client/dbclientcursorshimcursorid.h"

namespace mongo {

DBClientCursorShimCursorID::DBClientCursorShimCursorID(DBClientCursor& c)
    : cursor(c), iter(NULL, NULL), in_first_batch(false) {}

BSONObj DBClientCursorShimCursorID::get_cursor() {
    BSONObj b = cursor.rawNext();

    BSONElement ele = b["cursor"];

    if (!ele.eoo()) {
        cursor.cursorId = ele["id"].Long();
        cursor.ns = ele["ns"].String();

        if (!ele["firstBatch"].eoo()) {
            iter = BSONObjIterator(ele["firstBatch"].Obj());
            in_first_batch = true;
        }
    }

    return b;
}

bool DBClientCursorShimCursorID::more() {
    if (in_first_batch) {
        if (iter.more())
            return true;

        in_first_batch = false;

        if (!cursor.cursorId)
            return false;
    }

    return cursor.rawMore();
}

BSONObj DBClientCursorShimCursorID::next() {
    if (in_first_batch && iter.more())
        return iter.next().Obj();

    return cursor.rawNext();
}
}
