// Copyright 2016 MongoDB Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "catch.hpp"

#include <bsoncxx/private/itoa.hpp>

#if defined(__CYGWIN__)
#include <cstdio>
#endif // __CYGWIN__

TEST_CASE("util::itoa is equivalent to to_string(int)", "[bsoncxx::util::itoa]") {
#if defined(__CYGWIN__)
    char _cygwin_to_string_buf[128];
#endif //__CYGWIN__
    
    for (int i = 0; i <= 10000; i++) {
        bsoncxx::itoa val(i);
#if defined(__CYGWIN__)
        snprintf(_cygwin_to_string_buf, 128, "%d", i);
        std::string str(_cygwin_to_string_buf);
#else // __CYGWIN__
        std::string str = std::to_string(i);
#endif // __CYGWIN__
        REQUIRE(val.length() == str.length());
        REQUIRE(std::string(val.c_str()) == str);
    }
}
