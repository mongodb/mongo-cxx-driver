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

#include <mongocxx/test/v_noabi/catch_helpers.hh>

#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>

#include <mongocxx/instance.hpp>
#include <mongocxx/model/update_one.hpp>

#include <bsoncxx/test/catch.hh>

namespace {
using namespace bsoncxx::builder::basic;

TEST_CASE("update_one model tests", "[update_one][model]") {
    mongocxx::instance::current();

    auto filter = make_document(kvp("a", 1));
    auto update = make_document(kvp("$set", make_document(kvp("b", 1))));
    auto collation = make_document(kvp("locale", "en_US"));
    auto array_filters = make_array("a", "b");

    mongocxx::model::update_one uo(filter.view(), update.view());

    SECTION("stores required arguments") {
        REQUIRE(uo.filter().view() == filter.view());
        REQUIRE(uo.update().view() == update.view());
    }

    CHECK_OPTIONAL_ARGUMENT(uo, upsert, true);
    CHECK_OPTIONAL_ARGUMENT(uo, collation, collation.view());
    CHECK_OPTIONAL_ARGUMENT(uo, array_filters, array_filters.view());
}
} // namespace
