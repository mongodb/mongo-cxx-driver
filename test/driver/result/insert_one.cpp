// Copyright 2014 MongoDB Inc.
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

#include "bson/builder.hpp"
#include "driver/result/insert_one.hpp"

using namespace mongo::driver;

TEST_CASE("insert_one", "[insert_one][result]") {
    bson::builder::document build;
    build << "_id" << bson::oid{bson::oid::init_tag} << "x" << 1;

    bson::document::element g_oid{};

    result::bulk_write b(bson::document::value(build.view()));

    result::insert_one insert_one(std::move(b), g_oid);

    SECTION("returns correct response") {
        REQUIRE(insert_one.inserted_id() == g_oid);
    }

}

