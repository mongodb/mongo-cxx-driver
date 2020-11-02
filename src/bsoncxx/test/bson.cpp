// Copyright 2020 MongoDB Inc.
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

#include <bsoncxx/builder/bson/bson.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/test_util/catch.hh>
#include <bsoncxx/json.hpp>
#include <bsoncxx/types/bson_value/value.hpp>

using bsoncxx::builder::bson;
using namespace bsoncxx::types;
using namespace bsoncxx;

namespace {
TEST_CASE("implicit type deduction") {
    SECTION("object") {
        bson doc{"three", 3};
        std::cout << "doc=" << bsoncxx::to_json(((bson_value::value)doc).view().get_document()) << std::endl;
        auto expected = builder::basic::make_document(builder::basic::kvp("three", 3));
        REQUIRE(static_cast<bson_value::value>(doc).view().get_document() == expected.view());
    }

    SECTION("array") {
        // bson doc{1, 2, 3};
    }
}
}
