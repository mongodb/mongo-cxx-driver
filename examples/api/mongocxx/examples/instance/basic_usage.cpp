// Copyright 2009-present MongoDB, Inc.
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

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>

#include <examples/api/runner.hh>
#include <examples/macros.hh>

namespace {

template <typename... Args>
void use(Args&&...) {}

// [Example]
void example() {
    // Do not use mongocxx library interfaces at this point!

    {
        // Initialize the MongoDB C++ Driver by constructing the instance object.
        mongocxx::instance instance;

        EXPECT(&mongocxx::instance::current() == &instance);

        // Use mongocxx library interfaces at this point.
        use(mongocxx::client{});

        // Cleanup the MongoDB C++ Driver by destroying the instance object.
    }

    // Do not use mongocxx library interfaces at this point!
}
// [Example]

} // namespace

RUNNER_REGISTER_FORKING_COMPONENT() {
    example();
}
