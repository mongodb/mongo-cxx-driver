// str_test.cpp

/*    Copyright 2014 MongoDB, Inc.
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

#include "mongo/unittest/unittest.h"
#include "mongo/util/mongoutils/str.h"

namespace {

namespace str = mongoutils::str;

TEST(StripTrailingTests, RemoveFromHead) {
    std::string data("remove from head");
    str::stripTrailing(data, "re");
    ASSERT_EQUALS("mov fom had", data);
}

TEST(StripTrailingTests, RemoveFromTail) {
    std::string data("remove from tail");
    str::stripTrailing(data, "ail");
    ASSERT_EQUALS("remove from t", data);
}

TEST(StripTrailingTests, RemoveSpaces) {
    std::string data("remove spaces");
    str::stripTrailing(data, " a");
    ASSERT_EQUALS("removespces", data);
}

TEST(StripTrailingTests, RemoveFromMiddle) {
    std::string data("remove from middle");
    str::stripTrailing(data, "from");
    ASSERT_EQUALS("eve  iddle", data);
}

TEST(StripTrailingTests, RemoveFromEmpty) {
    std::string data("");
    str::stripTrailing(data, "from");
    ASSERT_EQUALS("", data);
}

TEST(StripTrailingTests, RemoveEmpty) {
    std::string data("remove empty");
    str::stripTrailing(data, "");
    ASSERT_EQUALS("remove empty", data);
}

TEST(StripTrailingTests, RemoveBringsEmptyResult) {
    std::string data("remove");
    str::stripTrailing(data, "remove");
    ASSERT_EQUALS("", data);
}

}  // namespace
