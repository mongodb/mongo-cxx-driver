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

#include <cstdint>
#include <string>

#include <bsoncxx/array/value.hpp>
#include <bsoncxx/array/view.hpp>
#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/types.hpp>

#include <examples/api/runner.hh>
#include <examples/macros.hh>

namespace {

// [Example]
void example() {
    using bsoncxx::ExtendedJsonMode;

    std::uint8_t data[]{"three"}; // Base64: dGhyZWU=
    std::uint32_t data_len{5u};   // Exclude null terminator.
    bsoncxx::types::b_binary binary{bsoncxx::binary_sub_type::k_binary, data_len, data};

    bsoncxx::array::value owner = bsoncxx::builder::basic::make_array(
        std::int32_t{1}, // "$numberInt": "1"
        std::int64_t{2}, // "$numberLong": "2"
        binary           // "$binary": { "$base64": "dGhyZWU=", "subType": 00 }
    );
    bsoncxx::array::view arr = owner.view();

    {
        // Canonical Extended JSON:
        // [
        //     { "$numberInt": "1" },
        //     { "$numberLong": "2" },
        //     {
        //         "$binary": {
        //             "base64": "dGhyZWU=",
        //             "subType": "00"
        //         }
        //     }
        // ]
        std::string json = bsoncxx::to_json(arr, ExtendedJsonMode::k_canonical);
        EXPECT(
            json ==
            R"([ { "$numberInt" : "1" }, { "$numberLong" : "2" }, { "$binary" : { "base64" : "dGhyZWU=", "subType" : "00" } } ])");
    }

    {
        // Relaxed Extended JSON
        // [
        //     1,
        //     2,
        //     {
        //         "$binary": {
        //             "base64": "dGhyZWU=",
        //             "subType": "00"
        //         }
        //     }
        // ]
        std::string json = bsoncxx::to_json(arr, ExtendedJsonMode::k_relaxed);
        EXPECT(json == R"([ 1, 2, { "$binary" : { "base64" : "dGhyZWU=", "subType" : "00" } } ])");
    }

    {
        // Legacy Extended JSON
        // [
        //     1,
        //     2,
        //     {
        //         "$binary": "dGhyZWU=",
        //         "$type": "00"
        //     }
        // ]
        std::string json = bsoncxx::to_json(arr);
        EXPECT(json == R"([ 1, 2, { "$binary" : "dGhyZWU=", "$type" : "00" } ])");
    }

    {
        std::string a = bsoncxx::to_json(arr);
        std::string b = bsoncxx::to_json(arr, ExtendedJsonMode::k_legacy);

        EXPECT(a == b);
    }
}
// [Example]

} // namespace

RUNNER_REGISTER_COMPONENT() {
    example();
}
