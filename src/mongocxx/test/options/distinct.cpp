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

#include "helpers.hpp"
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/test_util/catch.hh>
#include <mongocxx/instance.hpp>
#include <mongocxx/options/distinct.hpp>

namespace {
using namespace bsoncxx::builder::basic;
using namespace mongocxx;

TEST_CASE("distinct", "[distinct][option]") {
    instance::current();

    options::distinct dist;

    auto collation = make_document(kvp("locale", "en_US"));

    CHECK_OPTIONAL_ARGUMENT(dist, collation, collation.view());
    CHECK_OPTIONAL_ARGUMENT(dist, max_time, std::chrono::milliseconds{1000});
    CHECK_OPTIONAL_ARGUMENT(dist, read_preference, read_preference{});
}
}  // namespace
