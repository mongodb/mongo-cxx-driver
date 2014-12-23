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
#include "driver/result/replace_one.hpp"

using namespace mongo::driver;

TEST_CASE("replace_one", "[replace_one][result]") {
    bson::builder::document build;
    build << "_id" << bson::oid{bson::oid::init_tag} << "nMatched" << bson::types::b_int64{2} << "nModified" << bson::types::b_int64{1};

    result::bulk_write b(bson::document::value(build.view()));

    result::replace_one replace_one(std::move(b));

    SECTION("returns correct matched and modified count") {
        REQUIRE(replace_one.matched_count() == 2);
        REQUIRE(replace_one.modified_count() == 1);
    }

}
