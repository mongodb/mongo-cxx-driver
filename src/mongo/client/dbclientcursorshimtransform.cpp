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

#include "mongo/client/dbclientcursorshimtransform.h"

#include "mongo/client/dbclientcursor.h"

namespace mongo {

DBClientCursorShimTransform::DBClientCursorShimTransform(
    DBClientCursor& c,
    const stdx::function<bool(const BSONObj& input, BSONObj* output)>& transformation)
    : cursor(c), transformation(transformation) {}

bool DBClientCursorShimTransform::more() {
    while (cursor.rawMore()) {
        if (transformation(cursor.rawNext(), &nextDoc))
            return true;
    }
    return false;
}

BSONObj DBClientCursorShimTransform::next() {
    return nextDoc;
}

}  // namespace mongo
