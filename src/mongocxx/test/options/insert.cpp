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
#include <bsoncxx/test_util/catch.hh>
#include <mongocxx/instance.hpp>
#include <mongocxx/options/insert.hpp>

namespace {
using namespace mongocxx;

TEST_CASE("insert opts", "[insert][option]") {
    instance::current();

    options::insert ins;

    CHECK_OPTIONAL_ARGUMENT(ins, bypass_document_validation, true);
    CHECK_OPTIONAL_ARGUMENT(ins, write_concern, write_concern{});
}
}  // namespace
