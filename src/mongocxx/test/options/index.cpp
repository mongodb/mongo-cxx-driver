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

#include <bsoncxx/stdx/make_unique.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/options/index.hpp>
#include <mongocxx/stdx.hpp>

using namespace mongocxx;
using namespace mongocxx::options;

TEST_CASE("index", "[index][option]") {
    instance::current();

    options::index idx;
    std::unique_ptr<index::wiredtiger_storage_options> storage =
        stdx::make_unique<index::wiredtiger_storage_options>();

    CHECK_OPTIONAL_ARGUMENT(idx, background, true);
    CHECK_OPTIONAL_ARGUMENT(idx, unique, true);
    CHECK_OPTIONAL_ARGUMENT(idx, name, "name");
    CHECK_OPTIONAL_ARGUMENT(idx, sparse, true);
    CHECK_OPTIONAL_ARGUMENT(idx, expire_after, std::chrono::seconds(3600));
    CHECK_OPTIONAL_ARGUMENT(idx, version, 540);
    CHECK_OPTIONAL_ARGUMENT(idx, default_language, "en");
    CHECK_OPTIONAL_ARGUMENT(idx, language_override, "lang");
    CHECK_OPTIONAL_ARGUMENT(idx, twod_sphere_version, 4);
    CHECK_OPTIONAL_ARGUMENT(idx, twod_bits_precision, 4);
    CHECK_OPTIONAL_ARGUMENT(idx, twod_location_min, 90.0);
    CHECK_OPTIONAL_ARGUMENT(idx, twod_location_max, 90.0);
    CHECK_OPTIONAL_ARGUMENT(idx, haystack_bucket_size, 90.0);
    CHECK_OPTIONAL_ARGUMENT_WITHOUT_EQUALITY(idx, weights, bsoncxx::document::view{});
    CHECK_OPTIONAL_ARGUMENT_WITHOUT_EQUALITY(idx, partial_filter_expression,
                                             bsoncxx::document::view{});
    REQUIRE_NOTHROW(idx.storage_options(std::move(storage)));
}
