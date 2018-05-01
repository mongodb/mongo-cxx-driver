// Copyright 2018-present MongoDB Inc.
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
#include <mongocxx/options/client_session.hpp>

namespace {
using namespace mongocxx;

TEST_CASE("session option", "[session][option]") {
    instance::current();

    options::client_session opts;

    // Causal consistency is enabled by default.
    REQUIRE(opts.causal_consistency());
    opts.causal_consistency(false);
    REQUIRE(!opts.causal_consistency());
}

TEST_CASE("session option equals", "[session][option]") {
    instance::current();

    options::client_session opts1{};
    options::client_session opts2{};
    REQUIRE(opts1 == opts2);
    REQUIRE(opts2 == opts1);

    opts1.causal_consistency(false);
    opts2.causal_consistency(false);
    REQUIRE(opts1 == opts2);
    REQUIRE(opts2 == opts1);
}

TEST_CASE("session option inequals", "[session][option]") {
    instance::current();

    options::client_session opts1{};
    options::client_session opts2{};

    opts1.causal_consistency(false);
    REQUIRE(opts1 != opts2);
    REQUIRE(opts2 != opts1);
}

}  // namespace
