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
#include <mongocxx/options/pool.hpp>

using namespace mongocxx;

TEST_CASE("pool", "[pool][option]") {
    instance::current();

    {
        options::pool pool_opts{};
        REQUIRE(!pool_opts.client_opts().ssl_opts());
    }

    {
        options::pool pool_opts{options::client().ssl_opts(options::ssl())};
        REQUIRE(pool_opts.client_opts().ssl_opts());
    }
}
