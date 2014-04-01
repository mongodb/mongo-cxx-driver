/*    Copyright 2013 10gen Inc.
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

#include "mongo/bson/optime.h"

#include "mongo/unittest/unittest.h"

namespace {
  
    using mongo::OpTime;

    TEST(asDate, Simple) {
        OpTime o(0xc2345678ULL);
        ASSERT_EQUALS(o.asDate(), 0xc2345678ULL);
    }

    TEST(asLL, Simple) {
        OpTime o(0xc2345678L);
        ASSERT_EQUALS(o.asLL(), 0xc2345678LL);
    }

} // unnamed namespace
