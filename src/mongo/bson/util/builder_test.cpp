// builder_test.h

/*    Copyright 2009 10gen Inc.
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

#include "mongo/unittest/unittest.h"

#include "mongo/bson/util/builder.h"

namespace mongo {
TEST(Builder, String1) {
    const char* big = "eliot was here";
    StringData small(big, 5);
    ASSERT_EQUALS(small, "eliot");

    BufBuilder bb;
    bb.appendStr(small);

    ASSERT_EQUALS(0, strcmp(bb.buf(), "eliot"));
    ASSERT_EQUALS(0, strcmp("eliot", bb.buf()));
}

TEST(Builder, StringBuilderAddress) {
    const void* longPtr = reinterpret_cast<const void*>(-1);
    const void* shortPtr = reinterpret_cast<const void*>(0xDEADBEEFULL);
    const void* nullPtr = NULL;

    StringBuilder sb;
    sb << longPtr;

    if (sizeof(longPtr) == 8) {
        ASSERT_EQUALS("0xFFFFFFFFFFFFFFFF", sb.str());
    } else {
        ASSERT_EQUALS("0xFFFFFFFF", sb.str());
    }

    sb.reset();
    sb << shortPtr;
    ASSERT_EQUALS("0xDEADBEEF", sb.str());

    sb.reset();
    sb << nullPtr;
    ASSERT_EQUALS("0x0", sb.str());
}
}
