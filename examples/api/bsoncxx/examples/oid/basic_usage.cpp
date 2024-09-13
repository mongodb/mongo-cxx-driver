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

#include <cassert>
#include <cstring>
#include <ctime>
#include <string>

#include <bsoncxx/oid.hpp>

#include <examples/api/runner.hh>
#include <examples/macros.hh>

namespace {

// [Example]
void example() {
    {
        bsoncxx::oid a;
        bsoncxx::oid b;

        assert(a == b || a != b);  // Random.
    }

    {
        char bytes[bsoncxx::oid::k_oid_length]{};

        bsoncxx::oid oid{bytes, sizeof(bytes)};

        {
            std::time_t time = oid.get_time_t();
            char str[sizeof("YYYY-MM-DD HH:MM:SS")];
            assert(std::strftime(str, sizeof(str), "%F %T", std::gmtime(&time)) ==
                   sizeof(str) - 1u);
            assert(std::string(str) == "1970-01-01 00:00:00");
        }

        assert(oid.bytes() != bytes);  // Seperate storage.
        assert(oid.size() == bsoncxx::oid::k_oid_length);
        assert(std::memcmp(bytes, oid.bytes(), oid.size()) == 0);

        assert(oid.to_string() == "000000000000000000000000");
        assert(oid == bsoncxx::oid{"000000000000000000000000"});
    }

    {
        // Timestamp: 946771199 (0x386e94ff)
        // Value:     286462997 (0x11131415)
        // Counter:   2171427   (0x212223)
        bsoncxx::oid oid{"386e94ff1112131415212223"};

        {
            std::time_t time = oid.get_time_t();
            char str[sizeof("YYYY-MM-DD HH:MM:SS")];
            assert(std::strftime(str, sizeof(str), "%F %T", std::gmtime(&time)) ==
                   sizeof(str) - 1u);
            assert(std::string(str) == "2000-01-01 23:59:59");
        }

        assert(oid < bsoncxx::oid{"389622001112131415212223"});  // Timestamp: 2000-02-01 00:00:00
        assert(oid > bsoncxx::oid{"386d43801112131415212223"});  // Timestamp: 2000-01-01 00:00:00
        assert(oid < bsoncxx::oid{"386e94ffffffffffff212223"});  // Value: 1099511627775
        assert(oid > bsoncxx::oid{"386e94ff0000000000212223"});  // Value: 0
        assert(oid < bsoncxx::oid{"386e94ff1112131415ffffff"});  // Counter: 16777215
        assert(oid > bsoncxx::oid{"386e94ff1112131415000000"});  // Counter: 0
    }
}
// [Example]

}  // namespace

RUNNER_REGISTER_COMPONENT() {
    example();
}
