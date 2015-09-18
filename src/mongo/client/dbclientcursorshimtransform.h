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
#include "mongo/stdx/functional.h"

namespace mongo {

class DBClientCursor;

/**
 * DBClientCursorShimTransform implements the shim interface over a cursor
 * reply document by allowing a transformation to be applied.
 */
class DBClientCursorShimTransform : public DBClientCursorShim {
public:
    DBClientCursorShimTransform(
        DBClientCursor& c,
        const stdx::function<bool(const BSONObj& input, BSONObj* output)>& transformation);

    virtual BSONObj next();
    virtual bool more();

private:
    DBClientCursor& cursor;
    stdx::function<bool(const BSONObj&, BSONObj*)> transformation;
    BSONObj nextDoc;
};

}  // namespace mongo
