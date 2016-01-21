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

#include <mongocxx/instance.hpp>
#include <mongocxx/model/update_many.hpp>

TEST_CASE("update_many", "[update_many][model]") {
    mongocxx::instance::current();

    const bsoncxx::document::view a((std::uint8_t *)"", 0);
    const bsoncxx::document::view b((std::uint8_t *)"", 0);

    mongocxx::model::update_many um(a, b);

    SECTION("stores required arguments") {
        REQUIRE(um.filter().view().data() == a.data());
        REQUIRE(um.update().view().data() == b.data());
    }

    CHECK_OPTIONAL_ARGUMENT(um, upsert, true)
}
