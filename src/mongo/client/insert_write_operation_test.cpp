/*    Copyright 2015 MongoDB Inc.
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

#include "mongo/client/insert_write_operation.h"

#include "mongo/bson/bsonobj.h"
#include "mongo/bson/bsonobjbuilder.h"
#include "mongo/util/assert_util.h"

#include "mongo/unittest/unittest.h"

namespace {

    using namespace mongo;

    TEST(InsertWriteOperation, IdFieldCannotContainDollarSignTopLevel) {
        BSONObjBuilder bob;
        bob.append("foo", "bar");
        BSONObjBuilder subbob(bob.subobjStart("_id"));
        subbob.append("$bad", "nogood");
        subbob.done();
        ASSERT_THROWS(InsertWriteOperation w(bob.done()), UserException);
    }

    TEST(InsertWriteOperation, IdFieldCannotContainDollarSignNested) {
        BSONObjBuilder bob;
        bob.append("garply", "nnnnoooo");
        BSONObjBuilder subbob(bob.subobjStart("_id"));
        BSONObjBuilder subsubbob(subbob.subobjStart("foo"));
        BSONObjBuilder subsubsubbob(subbob.subobjStart("baz"));
        subsubsubbob.append("$blah", "borked");
        subsubsubbob.done();
        subsubbob.done();
        subbob.done();
        ASSERT_THROWS(InsertWriteOperation w(bob.done()), UserException);
    }

}  // namespace
