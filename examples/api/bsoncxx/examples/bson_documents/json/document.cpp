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

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/types.hpp>

#include <examples/api/runner.hh>
#include <examples/macros.hh>

namespace {

// [Example]
void example() {
    using bsoncxx::ExtendedJsonMode;
    using bsoncxx::builder::basic::kvp;

    std::uint8_t data[]{"three"};                                  // Base64: dGhyZWU=
    auto data_len = static_cast<std::uint32_t>(sizeof(data) - 1u); // Exclude null terminator.
    bsoncxx::types::b_binary binary{bsoncxx::binary_sub_type::k_binary, data_len, data};

    bsoncxx::document::value owner = bsoncxx::builder::basic::make_document(
        kvp("a", std::int32_t{1}), // "$numberInt": "1"
        kvp("b", std::int64_t{2}), // "$numberLong": "2"
        kvp("c", binary)           // "$binary": { "$base64": "dGhyZWU=", "subType": 00 }
    );
    bsoncxx::document::view doc = owner.view();

    {
        // Canonical Extended JSON:
        // {
        //     "a": { "$numberInt": "1" },
        //     "b": { "$numberLong": "2" },
        //     "c": {
        //         "$binary": {
        //             "base64": "dGhyZWU=",
        //             "subType": "00"
        //         }
        //     }
        // }
        std::string json = bsoncxx::to_json(doc, ExtendedJsonMode::k_canonical);
        EXPECT(
            json ==
            R"({ "a" : { "$numberInt" : "1" }, "b" : { "$numberLong" : "2" }, "c" : { "$binary" : { "base64" : "dGhyZWU=", "subType" : "00" } } })");
    }

    {
        // Relaxed Extended JSON
        // {
        //     "a": 1,
        //     "b": 2,
        //     "c": {
        //         "$binary": {
        //             "base64": "dGhyZWU=",
        //             "subType": "00"
        //         }
        //     }
        // }
        std::string json = bsoncxx::to_json(doc, ExtendedJsonMode::k_relaxed);
        EXPECT(json == R"({ "a" : 1, "b" : 2, "c" : { "$binary" : { "base64" : "dGhyZWU=", "subType" : "00" } } })");
    }

    {
        // Legacy Extended JSON
        // {
        //     "a": 1,
        //     "b": 2,
        //     "c": {
        //         "$binary": "dGhyZWU=",
        //         "$type": "00"
        //     }
        // }
        std::string json = bsoncxx::to_json(doc);
        EXPECT(json == R"({ "a" : 1, "b" : 2, "c" : { "$binary" : "dGhyZWU=", "$type" : "00" } })");
    }

    {
        std::string a = bsoncxx::to_json(doc);
        std::string b = bsoncxx::to_json(doc, ExtendedJsonMode::k_legacy);

        EXPECT(a == b);
    }
}
// [Example]

} // namespace

RUNNER_REGISTER_COMPONENT() {
    example();
}
