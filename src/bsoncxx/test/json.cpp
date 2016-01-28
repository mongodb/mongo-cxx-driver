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

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/exception/exception.hpp>
#include <bsoncxx/json.hpp>

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

    const auto expected = builder::stream::document{} << "a" << 1 << "b" << 2.0
                                                      << builder::stream::finalize;
    const auto expected_view = expected.view();

    const auto actual_doc = from_json(k_valid_json);
    const auto actual_view = actual_doc.view();

    REQUIRE(expected_view.length() == actual_view.length());
    REQUIRE(0 == memcmp(expected_view.data(), actual_view.data(), expected_view.length()));
}

TEST_CASE("empty document is converted correctly to json string") {
    using namespace bsoncxx;

    REQUIRE(0 == to_json(builder::stream::document{}.view()).compare("{\n\n}"));
}

TEST_CASE("empty array is converted correctly to json string") {
    using bsoncxx::to_json;
    using namespace bsoncxx::builder::stream;
    auto doc = document{};
    doc << "array" << open_array << close_array;
    REQUIRE(0 == to_json(doc.view()["array"]).compare("\"array\" : [\n\n]"));
}
