// Copyright 2009-present MongoDB, Inc.
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

#include <bsoncxx/test/v1/stdx/string_view.hh>

//

#include <initializer_list>
#include <list>
#include <stdexcept>
#include <string>
#include <type_traits>

#ifdef __has_include
#if __has_include(<version>)
#include <version>
#endif
#endif

#if defined(__cpp_lib_string_view)
#include <string_view>
#endif

#include <bsoncxx/v1/detail/compare.hpp>
#include <bsoncxx/v1/detail/type_traits.hpp>

#include <bsoncxx/test/catch.hh>

namespace {

namespace stdx = bsoncxx::v1::stdx;
using stdx::string_view;

static_assert(!std::is_constructible<string_view, std::vector<int>>::value, "fail");
static_assert(!std::is_constructible<string_view, double>::value, "fail");
static_assert(std::is_constructible<string_view, std::string>::value, "fail");
static_assert(std::is_convertible<std::string, string_view>::value, "fail");
static_assert(std::is_constructible<std::string, string_view>::value, "fail");

TEST_CASE("string_view: Default constructor") {
    (void)string_view();
    string_view s;
    CHECK(s.size() == 0);
    CHECK(s.length() == 0);
    CHECK(s.empty());
    CHECK(s.data() == nullptr);
    CHECK(s.begin() == s.end());
    CHECK(s == "");
}

static char const HELLO[] = "Hello, string_view!";
TEST_CASE("string_view: Pointer+size construct") {
    auto s = string_view(HELLO, sizeof HELLO - 1);
    CHECK(s.length() == s.size());
    CHECK(s.data() == HELLO + 0);
    CHECK(s[0] == 'H');
    CHECK(s == "Hello, string_view!");
    CHECK(s == HELLO);

    s = string_view(HELLO + 7, sizeof HELLO - 8);
    CHECK(s.size() == sizeof HELLO - 8);
    CHECK(s == "string_view!");
    s = string_view(HELLO + 7, 11);
    CHECK(s == "string_view");
    CHECK_FALSE(s != "string_view");
}

TEST_CASE("string_view: Pointer construct") {
    string_view s = "hello";
    CHECK(s.size() == 5);
    CHECK(s[4] == 'o');
    CHECK(s == "hello");
    CHECK(s == s);
    CHECK_FALSE(s != s);
    CHECK("hello" == s);
    CHECK_FALSE(s < s);
}

TEST_CASE("string_view: Reversal") {
    string_view string = "Hello!";
    auto rit = string.rbegin();
    CHECK(*rit++ == '!');
    CHECK(*rit++ == 'o');
    CHECK(*rit++ == 'l');
    CHECK(*rit++ == 'l');
    CHECK(*rit++ == 'e');
    CHECK(*rit++ == 'H');
    CHECK(rit == string.rend());
}

TEST_CASE("string_view: Get chars") {
    string_view string = "Hello, world";
    CHECK(string[0] == 'H');
    CHECK(string[0] == string.at(0));
    CHECK(string[1] == 'e');
    CHECK(string[1] == string.at(1));
    CHECK(string[11] == 'd');
    CHECK(string[11] == string.at(11));
    CHECK_THROWS_AS(string.at(12), std::out_of_range);
    CHECK(string.front() == string[0]);
    CHECK(string.back() == string[11]);
}

TEST_CASE("string_view: Substrings") {
    string_view str = "0123456789";
    CHECK(str.substr(0) == str);
    CHECK(str.substr(1) == "123456789");
    // Remove an empty prefix
    auto dup = str;
    dup.remove_prefix(0);
    CHECK(str == dup);
    // Remove one
    dup.remove_prefix(1);
    CHECK(dup == str.substr(1));
    // Remove from the end
    dup = str;
    dup.remove_suffix(1);
    CHECK(dup == "012345678");
    // Remove all
    dup = str;
    dup.remove_prefix(10);
    CHECK(dup.empty());
    dup = str;
    dup.remove_suffix(10);
    CHECK(dup.empty());

    dup = str.substr(10);
    CHECK(dup.empty());
    CHECK(dup.data() == str.data() + 10);

    dup = str.substr(0, 0);
    CHECK(dup.empty());
    CHECK(dup.data() == str.data());

    CHECK_THROWS_AS(dup.substr(500), std::out_of_range);

    // Substr of empty
    dup = "";
    dup = dup.substr(0);
    CHECK(dup.empty());
    dup = dup.substr(0, 1000);
    CHECK(dup.empty());
}

TEST_CASE("string_view: Compare") {
    string_view str = "abc";
    CHECK(str == "abc");
    CHECK_FALSE(str < "abc");
    CHECK(str < "abcd");
    CHECK(str.compare("abc") == 0);
    CHECK(str.compare("abcd") < 0);
}

TEST_CASE("string_view: Overloading safety") {
    std::vector<string_view> vec;
    CHECK(vec == vec);
}

TEST_CASE("string_view: find") {
    string_view sv = "abc123abc123";
    std::string str{sv};
    auto pos = sv.find("abc");
    CHECK(pos == 0);
    pos = sv.find("bc1");
    CHECK(pos == 1);
    pos = sv.find("bc1", 1);
    CHECK(pos == 1);
    pos = sv.find("bc1", 2);
    CHECK(pos == 7);
    pos = sv.find("", 4);
    CHECK(pos == 4);
    CHECK(sv.find("") == str.find(""));
    CHECK(sv.find("", 5000) == str.find("", 5000));
    CHECK(sv.find("nowhere") == sv.npos);
    CHECK(sv.rfind("nowhere") == str.rfind("nowhere"));
    CHECK(sv.find("123") == str.find("123"));
    CHECK(sv.find("123", 88888) == str.find("123", 88888));
    CHECK(sv.rfind("123") == str.rfind("123"));
    CHECK(sv.rfind("abc", 8) == str.rfind("abc", 8));
    CHECK(sv.rfind("abc", 888888) == str.rfind("abc", 888888));
    CHECK(sv.rfind("") == str.rfind(""));
    CHECK(sv.rfind("", 5000) == str.rfind("", 5000));

    CHECK(string_view("").find("") == std::string("").find(""));
    CHECK(string_view("").rfind("") == std::string("").rfind(""));

    CHECK(sv.find_first_of("54321") == str.find_first_of("54321"));
    CHECK(sv.find_first_of("nope") == str.find_first_of("nope"));
    CHECK(sv.find_last_of("fedcba") == str.find_last_of("fedcba"));
    CHECK(sv.find_last_of("nope") == str.find_last_of("nope"));

    CHECK(sv.find_first_of("54321", 5) == str.find_first_of("54321", 5));
    CHECK(sv.find_first_of("nope", 5) == str.find_first_of("nope", 5));
    CHECK(sv.find_last_of("fedcba", 5) == str.find_last_of("fedcba", 5));
    CHECK(sv.find_last_of("fedcba", 2) == str.find_last_of("fedcba", 2));
    CHECK(sv.find_last_of("nope", 5) == str.find_last_of("nope", 5));

    CHECK(sv.find_first_not_of("abcdef") == str.find_first_not_of("abcdef"));
    CHECK(sv.find_first_not_of("123456") == str.find_first_not_of("123456"));
    CHECK(sv.find_last_not_of("abcdef") == str.find_last_not_of("abcdef"));
    CHECK(sv.find_last_not_of("123456") == str.find_last_not_of("123456"));
    CHECK(sv.find_first_not_of("abcdef123456") == str.find_first_not_of("abcdef123456"));
    CHECK(sv.find_last_not_of("abcdef123456") == str.find_last_not_of("abcdef123456"));

    CHECK(sv.find_first_not_of("abcdef", 5) == str.find_first_not_of("abcdef", 5));
    CHECK(sv.find_first_not_of("123456", 5) == str.find_first_not_of("123456", 5));
    CHECK(sv.find_last_not_of("abcdef", 5) == str.find_last_not_of("abcdef", 5));
    CHECK(sv.find_last_not_of("123456", 5) == str.find_last_not_of("123456", 5));
    CHECK(sv.find_first_not_of("abcdef123456", 5) == str.find_first_not_of("abcdef123456", 5));
    CHECK(sv.find_last_not_of("abcdef123456", 5) == str.find_last_not_of("abcdef123456", 5));
}

#ifdef __cpp_lib_string_view
static_assert(std::is_constructible<std::string_view, string_view>::value, "fail");

TEST_CASE("Convert to/from std::string_view") {
    std::string std_str = "Hello!";
    string_view bson_sv = std_str;
    std::string_view std_sv = std::string_view(bson_sv);
    CHECK(bson_sv == std_str);
    CHECK(std_sv == bson_sv);
    CHECK(std_sv == std_str);
    bson_sv = std_sv;
    CHECK(bson_sv == std_sv);
}
#endif

TEST_CASE("StringMaker", "[bsoncxx][test][v1][stdx][string_view]") {
    string_view sv;

    CHECK(bsoncxx::test::stringify(sv) == R"("")");

    sv = "abc";

    CHECK(bsoncxx::test::stringify(sv) == R"("abc")");
}

} // namespace
