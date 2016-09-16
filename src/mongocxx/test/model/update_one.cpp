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
#include <mongocxx/model/update_one.hpp>

using namespace bsoncxx::builder::stream;

TEST_CASE("update_one model tests", "[update_one][model]") {
    mongocxx::instance::current();

    auto filter = document{} << "a" << 1 << finalize;
    auto update = document{} << "$set" << open_document << "b" << 1 << close_document << finalize;
    auto collation = document{} << "locale"
                                << "en_US" << finalize;

    mongocxx::model::update_one uo(filter.view(), update.view());

    SECTION("stores required arguments") {
        REQUIRE(uo.filter().view() == filter.view());
        REQUIRE(uo.update().view() == update.view());
    }

    CHECK_OPTIONAL_ARGUMENT(uo, upsert, true);
    CHECK_OPTIONAL_ARGUMENT(uo, collation, collation.view());
}
