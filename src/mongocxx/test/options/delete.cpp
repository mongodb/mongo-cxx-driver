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

#include "helpers.hpp"

#include <chrono>

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/test_util/catch.hh>
#include <mongocxx/instance.hpp>
#include <mongocxx/options/delete.hpp>

namespace {
using namespace bsoncxx::builder::basic;
using namespace mongocxx;

TEST_CASE("delete_options", "[delete][option]") {
    instance::current();

    options::delete_options del;

    auto collation = make_document(kvp("locale", "en_US"));

    CHECK_OPTIONAL_ARGUMENT(del, collation, collation.view());
    CHECK_OPTIONAL_ARGUMENT(del, write_concern, write_concern{});
}

TEST_CASE("delete_options equals", "[delete][option]") {
    instance::current();

    options::delete_options del1;
    options::delete_options del2;

    REQUIRE(del1 == del2);
}

TEST_CASE("delete_options inequals", "[delete][option]") {
    using bsoncxx::builder::basic::make_document;
    using bsoncxx::builder::basic::kvp;
    instance::current();

    options::delete_options del1;
    del1.collation(make_document(kvp("locale", "simple")).view());
    options::delete_options del2;

    REQUIRE(del1 != del2);
}
}  // namespace
