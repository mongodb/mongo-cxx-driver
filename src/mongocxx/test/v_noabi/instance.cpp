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

#include <mongocxx/instance.hpp>

//

#include <mongocxx/exception/error_code.hpp>
#include <mongocxx/exception/logic_error.hpp>

#include <bsoncxx/test/system_error.hh>

#include <mongocxx/test/subprocess.hh>

namespace {

using mongocxx::v_noabi::instance;

using code = mongocxx::v_noabi::error_code;

TEST_CASE("basic", "[mongocxx][v_noabi][instance]") {
    auto const ret = mongocxx::test::subprocess([] {
        using mongocxx::v_noabi::logic_error;

        instance* inst = nullptr;

        // instance::current creates instance when one has not already been created
        REQUIRE_NOTHROW(inst = &instance::current());

        // multiple instances cannot be created"
        REQUIRE_THROWS_WITH_CODE(instance{}, code::k_cannot_recreate_instance);
        REQUIRE_THROWS_AS(instance{}, logic_error);

        // instance::current works when instance is alive
        REQUIRE_NOTHROW(instance::current());

        // an instance cannot be created after one has been destroyed
        inst->~instance();
        inst = nullptr;
        REQUIRE_THROWS_WITH_CODE(instance{}, code::k_cannot_recreate_instance);
        REQUIRE_THROWS_AS(instance{}, logic_error);

        // instance::current throws if an instance has already been destroyed
        REQUIRE_THROWS_WITH_CODE(instance::current(), code::k_instance_destroyed);
        REQUIRE_THROWS_AS(instance::current(), logic_error);
    });
    REQUIRE(ret == 0);
}

} // namespace
