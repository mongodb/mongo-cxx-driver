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
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/test_util/catch.hh>
#include <mongocxx/instance.hpp>
#include <mongocxx/options/aggregate.hpp>

namespace {
using namespace bsoncxx::builder::basic;
using namespace mongocxx;

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

TEST_CASE("aggregate", "[aggregate][option]") {
    instance::current();

    options::aggregate agg;

    auto collation = make_document(kvp("locale", "en_US"));

    bsoncxx::document::view_or_value hint{make_document(kvp("_id", 1))};

    CHECK_OPTIONAL_ARGUMENT(agg, allow_disk_use, true);
    CHECK_OPTIONAL_ARGUMENT(agg, batch_size, 500);
    CHECK_OPTIONAL_ARGUMENT(agg, bypass_document_validation, true);
    CHECK_OPTIONAL_ARGUMENT(agg, collation, collation.view());
    CHECK_OPTIONAL_ARGUMENT(agg, max_time, std::chrono::milliseconds{1000});
    CHECK_OPTIONAL_ARGUMENT(agg, read_preference, read_preference{});
    CHECK_OPTIONAL_ARGUMENT(agg, hint, hint);
}
}  // namespace
