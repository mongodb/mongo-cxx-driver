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

#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/builder/basic/sub_array.hpp>
#include <bsoncxx/exception/exception.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/private/libbson.hh>
#include <bsoncxx/test_util/catch.hh>

namespace {
using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_array;
using bsoncxx::builder::basic::make_document;

constexpr auto k_invalid_json = R"({])";
constexpr auto k_valid_json = R"({ "a" : 1, "b" : 2.0 })";

TEST_CASE("invalid json throws") {
    using namespace bsoncxx;
    REQUIRE_THROWS_AS(from_json(k_invalid_json), bsoncxx::exception);
}

TEST_CASE("valid json does not throw") {
    using namespace bsoncxx;
    REQUIRE_NOTHROW(from_json(k_valid_json));
}

TEST_CASE("valid json is converted to equivalent BSON") {
    using namespace bsoncxx;

    const auto expected = make_document(kvp("a", 1), kvp("b", 2.0));
    const auto expected_view = expected.view();

    const auto actual_doc = from_json(k_valid_json);
    const auto actual_view = actual_doc.view();

    REQUIRE(expected_view.length() == actual_view.length());
    REQUIRE(0 == memcmp(expected_view.data(), actual_view.data(), expected_view.length()));
}

TEST_CASE("empty document is converted correctly to json string") {
    using namespace bsoncxx;
    REQUIRE(0 == to_json(make_document().view()).compare("{ }"));
}

TEST_CASE("empty array is converted correctly to json string") {
    using bsoncxx::to_json;

    auto doc = make_document(kvp("array", make_array()));
    REQUIRE(0 == to_json(doc.view()).compare(R"({ "array" : [  ] })"));
}

TEST_CASE("CXX-941 is resolved") {
    std::string obj_value = R"({"id1":"val1", "id2":"val2"})";
    auto doc = make_document(kvp("obj_name", obj_value));
    std::string output = bsoncxx::to_json(doc.view());
    REQUIRE(output ==
            "{ \"obj_name\" : \"{\\\"id1\\\":\\\"val1\\\", \\\"id2\\\":\\\"val2\\\"}\" }");
}

TEST_CASE("CXX-1246: Legacy Extended JSON (Implicit)") {
    using namespace bsoncxx;
    types::b_binary bin_val{
        binary_sub_type::k_uuid, 8, reinterpret_cast<const uint8_t*>("deadbeef")};
    auto doc = make_document(kvp("number", 42), kvp("bin", bin_val));
    auto output = bsoncxx::to_json(doc.view());
    REQUIRE(output ==
            R"({ "number" : 42, "bin" : { "$binary" : "ZGVhZGJlZWY=", "$type" : "04" } })");
}

TEST_CASE("CXX-1246: Legacy Extended JSON (Explicit)") {
    using namespace bsoncxx;
    types::b_binary bin_val{
        binary_sub_type::k_uuid, 8, reinterpret_cast<const uint8_t*>("deadbeef")};
    auto doc = make_document(kvp("number", 42), kvp("bin", bin_val));
    auto output = to_json(doc.view(), ExtendedJsonMode::k_legacy);
    REQUIRE(output ==
            R"({ "number" : 42, "bin" : { "$binary" : "ZGVhZGJlZWY=", "$type" : "04" } })");
}

TEST_CASE("CXX-1246: Relaxed Extended JSON") {
    using namespace bsoncxx;
    types::b_binary bin_val{
        binary_sub_type::k_uuid, 8, reinterpret_cast<const uint8_t*>("deadbeef")};
    auto doc = make_document(kvp("number", 42), kvp("bin", bin_val));
    auto output = to_json(doc.view(), ExtendedJsonMode::k_relaxed);

    // As of libbson 1.18.0, "base64" has correct spacing (see CDRIVER-3958) after extJSON
    // marshalling.
    const char* expected;
    if ((BSON_MAJOR_VERSION == 1 && BSON_MINOR_VERSION >= 18) || BSON_MAJOR_VERSION > 1) {
        expected =
            R"({ "number" : 42, "bin" : { "$binary" : { "base64" : "ZGVhZGJlZWY=", "subType" : "04" } } })";
    } else {
        expected =
            R"({ "number" : 42, "bin" : { "$binary" : { "base64": "ZGVhZGJlZWY=", "subType" : "04" } } })";
    }
    REQUIRE(output == expected);
}

TEST_CASE("CXX-1246: Canonical Extended JSON") {
    using namespace bsoncxx;
    types::b_binary bin_val{
        binary_sub_type::k_uuid, 8, reinterpret_cast<const uint8_t*>("deadbeef")};
    auto doc = make_document(kvp("number", 42), kvp("bin", bin_val));
    auto output = to_json(doc.view(), ExtendedJsonMode::k_canonical);

    // As of libbson 1.18.0, "base64" has correct spacing (see CDRIVER-3958) after extJSON
    // marshalling.
    const char* expected;
    if ((BSON_MAJOR_VERSION == 1 && BSON_MINOR_VERSION >= 18) || BSON_MAJOR_VERSION > 1) {
        expected =
            R"({ "number" : { "$numberInt" : "42" }, "bin" : { "$binary" : { "base64" : "ZGVhZGJlZWY=", "subType" : "04" } } })";
    } else {
        expected =
            R"({ "number" : { "$numberInt" : "42" }, "bin" : { "$binary" : { "base64": "ZGVhZGJlZWY=", "subType" : "04" } } })";
    }
    REQUIRE(output == expected);
}
}  // namespace
