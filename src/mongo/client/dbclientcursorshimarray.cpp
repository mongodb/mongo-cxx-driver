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

#include "mongo/client/dbclientcursorshimarray.h"
#include "mongo/client/dbclientcursor.h"

namespace mongo  {

    DBClientCursorShimArray::DBClientCursorShimArray(DBClientCursor& c) :
        cursor(c),
        iter(NULL, NULL),
        has_array(false) {}

    bool DBClientCursorShimArray::more() {
        bool r = false;

        if (!has_array) {
            if (cursor.rawMore()) {
                BSONObj val = cursor.rawNext();
                BSONElement result = val["result"];

                if (!result.eoo()) {
                    iter = BSONObjIterator(result.Obj());
                    r = true;
                }
            }

            has_array = true;
        }
        else {
            r = iter.more();
        }

        return r;
    }

    BSONObj DBClientCursorShimArray::next() {
        BSONObj b;

        if (!has_array) {
            uassert(0, "DBClientCursorShimArray next() called but more() is false", more());
        }
        else {
            b = iter.next().Obj();
        }

        return b;
    }
}
