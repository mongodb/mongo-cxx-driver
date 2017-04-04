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
#include <bsoncxx/test_util/catch.hh>

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_array;
using bsoncxx::builder::basic::make_document;

namespace {
constexpr auto k_invalid_json = R"({])";
constexpr auto k_valid_json = R"({ "a" : 1, "b" : 2.0 })";
}

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
