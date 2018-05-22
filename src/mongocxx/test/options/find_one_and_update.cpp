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

#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/test_util/catch.hh>
#include <mongocxx/instance.hpp>
#include <mongocxx/options/find_one_and_update.hpp>

namespace {
using namespace bsoncxx::builder::basic;
using namespace mongocxx;

TEST_CASE("find_one_and_update", "[find_one_and_update][option]") {
    instance::current();

    options::find_one_and_update opts{};

    auto collation = make_document(kvp("locale", "en_US"));
    std::chrono::milliseconds ms{400};
    auto projection = make_document(kvp("_id", false));
    auto sort = make_document(kvp("x", -1));
    auto array_filters = make_array("a", "b");

    CHECK_OPTIONAL_ARGUMENT(opts, bypass_document_validation, true);
    CHECK_OPTIONAL_ARGUMENT(opts, collation, collation.view());
    CHECK_OPTIONAL_ARGUMENT(opts, max_time, ms);
    CHECK_OPTIONAL_ARGUMENT(opts, projection, projection.view());
    CHECK_OPTIONAL_ARGUMENT(opts, return_document, options::return_document::k_before);
    CHECK_OPTIONAL_ARGUMENT(opts, sort, sort.view());
    CHECK_OPTIONAL_ARGUMENT(opts, upsert, true);
    CHECK_OPTIONAL_ARGUMENT(opts, array_filters, array_filters.view());
}

TEST_CASE("find_one_and_update equals", "[find_one_and_update][options]") {
    instance::current();

    options::find_one_and_update opts1{};
    options::find_one_and_update opts2{};

    REQUIRE(opts1 == opts2);
}

TEST_CASE("find_one_and_update inequals", "[find_one_and_update][options]") {
    instance::current();

    options::find_one_and_update opts1{};
    opts1.max_time(std::chrono::milliseconds{500});
    options::find_one_and_update opts2{};

    REQUIRE(opts1 != opts2);
}
}  // namespace
