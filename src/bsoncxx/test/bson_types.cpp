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

#include <chrono>

#include <bsoncxx/types.hpp>
#include <bsoncxx/types/bson_value/view.hpp>

#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/stdx/string_view.hpp>
#include <bsoncxx/test_util/catch.hh>

namespace {

using namespace bsoncxx;
using namespace types;
using bsoncxx::builder::basic::make_document;
using bsoncxx::builder::basic::kvp;

TEST_CASE("type to_string", "[bsoncxx::type::to_string]") {
    REQUIRE(to_string(bsoncxx::type::k_bool) == "bool");
}

TEST_CASE("binary_sub_type to_string", "[bsoncxx::type::to_string]") {
    REQUIRE(to_string(bsoncxx::binary_sub_type::k_function) == "function");
}

TEST_CASE("b_double", "[bsoncxx::type::b_double]") {
    b_double a{1.1};
    b_double b{a};
    b_double c{2.1};
    REQUIRE(a == b);
    REQUIRE(!(a == c));
}

TEST_CASE("b_utf8", "[bsoncxx::type::b_utf8]") {
    b_utf8 a{"hello"};
    b_utf8 b{a};
    b_utf8 c{"world"};
    REQUIRE(a == b);
    REQUIRE(!(a == c));
}

TEST_CASE("b_document", "[bsoncxx::type::b_document]") {
    auto doc1 = make_document(kvp("a", 1));
    auto doc2 = make_document(kvp("b", 2));
    b_document a{doc1.view()};
    b_document b{a};
    b_document c{doc2.view()};
    REQUIRE(a == b);
    REQUIRE(!(a == c));
}

TEST_CASE("b_array", "[bsoncxx::type::b_array]") {
    builder::basic::array arr;
    arr.append(b_int32{1}, b_int32{2});
    b_array a{arr.view()};
    b_array b{a};
    arr.append(b_int32{3});
    b_array c{arr.view()};
    REQUIRE(a == b);
    REQUIRE(!(a == c));
}

TEST_CASE("b_binary", "[bsoncxx::type::b_binary]") {
    b_binary a{binary_sub_type::k_binary, 8, reinterpret_cast<const uint8_t*>("deadbeef")};
    b_binary b{binary_sub_type::k_binary, 8, reinterpret_cast<const uint8_t*>("deadbeef")};
    b_binary c{binary_sub_type::k_binary, 8, reinterpret_cast<const uint8_t*>("daedbeef")};
    REQUIRE(a == b);
    REQUIRE(!(a == c));
}

TEST_CASE("b_undefined", "[bsoncxx::type::b_undefined]") {
    b_undefined a;
    REQUIRE(a == a);
}

TEST_CASE("b_oid", "[bsoncxx::type::b_oid]") {
    oid oid1;
    oid oid2;
    b_oid a{bsoncxx::oid{oid1.bytes(), 12}};
    b_oid b{a.value};
    b_oid c{bsoncxx::oid{oid2.bytes(), 12}};
    REQUIRE(a == b);
    REQUIRE(!(a == c));
}

TEST_CASE("b_bool", "[bsoncxx::type::b_undefined]") {
    b_bool a{true};
    b_bool b{a};
    b_bool c{false};
    REQUIRE(a == b);
    REQUIRE(!(a == c));
}

TEST_CASE("b_null", "[bsoncxx::type::b_null]") {
    b_null a;
    REQUIRE(a == a);
}

TEST_CASE("b_regex", "[bsoncxx::type::b_regex]") {
    b_regex a{stdx::string_view{"^foo|bar$"}};
    b_regex b{a.regex};
    b_regex c{stdx::string_view{"^bar|foo$"}};
    REQUIRE(a == b);
    REQUIRE(!(a == c));
}

TEST_CASE("b_code", "[bsoncxx::type::b_code]") {
    b_code a{"var a = {};"};
    b_code b{a};
    b_code c{"var b = 1;"};
    REQUIRE(a == b);
    REQUIRE(!(a == c));
}

TEST_CASE("b_symbol", "[bsoncxx::type::b_symbol]") {
    b_symbol a{"deadbeef"};
    b_symbol b{a};
    b_symbol c{"beefdead"};
    REQUIRE(a == b);
    REQUIRE(!(a == c));
}

TEST_CASE("b_codewscope", "[bsoncxx::type::b_codewscope]") {
    auto scope1 = make_document(kvp("b", 1));
    auto scope2 = make_document(kvp("b", 5));
    b_codewscope a{"var a = b;", scope1.view()};
    b_codewscope b{a.code, a.scope};
    b_codewscope c{"var a = b;", scope2.view()};
    REQUIRE(a == b);
    REQUIRE(!(a == c));
}

TEST_CASE("b_int32", "[bsoncxx::type::b_int32]") {
    b_int32 a{10};
    b_int32 b{a};
    b_int32 c{5};
    REQUIRE(a == b);
    REQUIRE(!(a == c));
}

TEST_CASE("b_timestamp", "[bsoncxx::type::b_timestamp") {
    b_timestamp a{100, 1000};
    b_timestamp b{100, 1000};
    b_timestamp c{200, 1000};
    REQUIRE(a == b);
    REQUIRE(!(a == c));
}

TEST_CASE("b_int64", "[bsoncxx::type::b_int64]") {
    b_int64 a{100};
    b_int64 b{a};
    b_int32 c{200};
    REQUIRE(a == b);
    REQUIRE(!(a == c));
}

TEST_CASE("b_decimal128", "[bsoncxx::type::b_decimal128]") {
    b_decimal128 a{"-1234E+999"};
    b_decimal128 b{a};
    b_decimal128 c{"1234E+999"};
    REQUIRE(a == b);
    REQUIRE(!(a == c));
}

TEST_CASE("b_minkey", "[bsoncxx::type::b_minkey]") {
    b_minkey a;
    REQUIRE(a == a);
}

TEST_CASE("b_maxkey", "[bsoncxx::type::b_maxkey]") {
    b_maxkey a;
    REQUIRE(a == a);
}

TEST_CASE("value returns correct type", "[bsoncxx::type::value]") {
    b_bool bool_val{true};
    REQUIRE(value{bool_val}.type() == bsoncxx::type::k_bool);
}

TEST_CASE("value with b_double", "[bsoncxx::type::value]") {
    b_double double_val{1.1};
    REQUIRE(value{double_val}.get_double() == double_val);
}

TEST_CASE("value with b_utf8", "[bsoncxx::type::value]") {
    b_utf8 utf8_val{"hello"};
    REQUIRE(value{utf8_val}.get_utf8() == utf8_val);
}

TEST_CASE("value with b_document", "[bsoncxx::type::value]") {
    auto doc = make_document(kvp("a", 1));
    b_document doc_val{doc.view()};
    REQUIRE(value{doc_val}.get_document() == doc_val);
}

TEST_CASE("value with b_array", "[bsoncxx::type::value]") {
    builder::basic::array arr;
    arr.append(b_int32{1}, b_int32{2}, b_int32{3});
    b_array arr_val{arr.view()};
    REQUIRE(value{arr_val}.get_array() == arr_val);
}

TEST_CASE("value with b_binary", "[bsoncxx::type::value]") {
    b_binary binary_val{binary_sub_type::k_binary, 8, reinterpret_cast<const uint8_t*>("deadbeef")};
    REQUIRE(value{binary_val}.get_binary() == binary_val);
}

TEST_CASE("value with b_undefined", "[bsoncxx::type::value]") {
    b_undefined undef_val;
    REQUIRE(value{undef_val}.get_undefined() == undef_val);
}

TEST_CASE("value with b_oid", "[bsoncxx::type::value]") {
    oid oid;
    b_oid oid_val{bsoncxx::oid{oid.bytes(), 12}};
    REQUIRE(value{oid_val}.get_oid() == oid_val);
}

TEST_CASE("value with b_bool", "[bsoncxx::type::value]") {
    b_bool bool_val{true};
    REQUIRE(value{bool_val}.get_bool() == bool_val);
}

TEST_CASE("value with b_date", "[bsoncxx::type::value]") {
    using bsoncxx::types::b_date;
    using std::chrono::system_clock;

    b_date date_val{system_clock::now()};
    REQUIRE(value{date_val}.get_date() == date_val);
}

TEST_CASE("value with b_null", "[bsoncxx::type::value]") {
    b_null null_val;
    REQUIRE(value{null_val}.get_null() == null_val);
}

TEST_CASE("value with b_regex", "[bsoncxx::type::value]") {
    b_regex regex_val{stdx::string_view{"^foo|bar$"}};
    REQUIRE(value{regex_val}.get_regex() == regex_val);
}

TEST_CASE("value with b_code", "[bsoncxx::type::value]") {
    b_code code_val{"var a = {};"};
    REQUIRE(value{code_val}.get_code() == code_val);
}

TEST_CASE("value with b_symbol", "[bsoncxx::type::value]") {
    b_symbol symbol_val{"deadbeef"};
    REQUIRE(value{symbol_val}.get_symbol() == symbol_val);
}

TEST_CASE("value with b_codewscope", "[bsoncxx::type::value]") {
    auto scope1 = make_document(kvp("b", 1));
    b_codewscope codewscope_val{"var a = b;", scope1.view()};
    REQUIRE(value{codewscope_val}.get_codewscope() == codewscope_val);
}

TEST_CASE("value with b_int32", "[bsoncxx::type::value]") {
    b_int32 int32_val{10};
    REQUIRE(value{int32_val}.get_int32() == int32_val);
}

TEST_CASE("value with b_timestamp", "[bsoncxx::type::value]") {
    types::b_timestamp timestamp_val{100, 1000};
    REQUIRE(value{timestamp_val}.get_timestamp() == timestamp_val);
}

TEST_CASE("value with b_int64", "[bsoncxx::type::value]") {
    b_int64 int64_val{100};
    REQUIRE(value{int64_val}.get_int64() == int64_val);
}

TEST_CASE("value with b_decimal128", "[bsoncxx::type::value]") {
    b_decimal128 decimal_val{"-1234E+999"};
    REQUIRE(value{decimal_val}.get_decimal128() == decimal_val);
}

TEST_CASE("value with b_minkey", "[bsoncxx::type::value]") {
    b_minkey minkey_val;
    REQUIRE(value{minkey_val}.get_minkey() == minkey_val);
}

TEST_CASE("value with b_maxkey", "[bsoncxx::type::value]") {
    b_maxkey maxkey_val;
    REQUIRE(value{maxkey_val}.get_maxkey() == maxkey_val);
}

TEST_CASE("value with copy constructor", "[bsoncxx::type::value]") {
    b_int32 int32_val{10};
    value original{int32_val};
    value copy{original};
    REQUIRE(copy.get_int32() == int32_val);
}

TEST_CASE("value with assignment operator", "[bsoncxx::type::value]") {
    b_int32 int32_val{10};
    value original{int32_val};
    value copy = original;
    REQUIRE(copy.get_int32() == int32_val);
}

TEST_CASE("value with equality operator", "[bsoncxx::type::value]") {
    value original{b_int32{10}};
    value copy{b_int32{10}};
    REQUIRE(original == copy);
}

TEST_CASE("value with equality for value and non-value", "[bsoncxx::type::value]") {
    b_int64 int64_val{100};
    REQUIRE(value{int64_val} == int64_val);
}

TEST_CASE("value with equality for non-value and value", "[bsoncxx::type::value]") {
    b_int64 int64_val{100};
    REQUIRE(int64_val == value{int64_val});
}

TEST_CASE("value with value inequality operator and same type", "[bsoncxx::type::value]") {
    value first{b_int32{10}};
    value second{b_int32{5}};
    REQUIRE(first != second);
}

TEST_CASE("value with value inequality operator and different type", "[bsoncxx::type::value]") {
    value first{b_int32{10}};
    value second{b_int64{10}};
    REQUIRE(first != second);
}

TEST_CASE("value with inequality for value and non-value", "[bsoncxx::type::value]") {
    b_int64 int64_val{100};
    REQUIRE(value{b_int64{200}} != int64_val);
}

TEST_CASE("value with inequality for non-value and value", "[bsoncxx::type::value]") {
    b_int64 int64_val{100};
    REQUIRE(int64_val != value{b_int64{200}});
}
}
