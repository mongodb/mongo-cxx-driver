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

#include <chrono>

#include "catch.hpp"
#include "helpers.hpp"

#include <mongocxx/instance.hpp>
#include <mongocxx/options/aggregate.hpp>

using namespace mongocxx;

TEST_CASE("aggregate", "[aggregate][option]") {
    instance::current();

    options::aggregate agg;

    CHECK_OPTIONAL_ARGUMENT(agg, allow_disk_use, true);
    CHECK_OPTIONAL_ARGUMENT(agg, batch_size, 500);
    CHECK_OPTIONAL_ARGUMENT(agg, max_time, std::chrono::milliseconds{1000});
    CHECK_OPTIONAL_ARGUMENT(agg, use_cursor, true);
    CHECK_OPTIONAL_ARGUMENT(agg, bypass_document_validation, true);
    CHECK_OPTIONAL_ARGUMENT_WITHOUT_EQUALITY(agg, read_preference, read_preference{});
}
