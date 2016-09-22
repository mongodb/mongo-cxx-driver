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
#include "helpers.hpp"

#include <bsoncxx/builder/stream/document.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/model/replace_one.hpp>

using namespace bsoncxx::builder::stream;

TEST_CASE("replace_one model tests", "[replace_one][model]") {
    mongocxx::instance::current();

    auto filter = document{} << "a" << 1 << finalize;
    auto replacement = document{} << "b" << 1 << finalize;
    auto collation = document{} << "locale"
                                << "en_US" << finalize;

    mongocxx::model::replace_one ro(filter.view(), replacement.view());

    SECTION("stores required arguments") {
        REQUIRE(ro.filter().view() == filter.view());
        REQUIRE(ro.replacement().view() == replacement.view());
    }

    CHECK_OPTIONAL_ARGUMENT(ro, upsert, true);
    CHECK_OPTIONAL_ARGUMENT(ro, collation, collation.view());
}
