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

#include "mongo/bson/bsonobj.h"

namespace mongo {

/** DBClientCursorShim provides an abstract interface for providing
 * polymorphic cursors.  When a shim is attached to a DBClientCursor, the
 * shim intercepts next() and more() calls.  These methods may call through
 * to the underlying cursor object via rawNext() and rawMore(), or handle
 * them entirely themselves. */
class DBClientCursorShim {
public:
    virtual BSONObj next() = 0;
    virtual bool more() = 0;
    virtual ~DBClientCursorShim() {}
};
}
