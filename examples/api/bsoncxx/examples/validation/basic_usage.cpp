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
#include <cstddef>
#include <cstdint>

#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/validate.hpp>

#include <examples/api/runner.hh>
#include <examples/macros.hh>

namespace {

// [Example]
void example() {
    {
        std::uint8_t bytes[1]{};  // Invalid.

        assert(!bsoncxx::validate(bytes, sizeof(bytes)));

        std::size_t offset;

        assert(!bsoncxx::validate(bytes, sizeof(bytes), bsoncxx::validator{}, &offset));

        // Set to `0` for an invalid BSON document.
        assert(offset == 0u);
    }

    bsoncxx::document::value owner = bsoncxx::from_json(R"({"x": 1})");
    const std::uint8_t* data = owner.data();
    const std::size_t length = owner.length();

    {
        auto doc_opt = bsoncxx::validate(data, length);
        assert(doc_opt);

        bsoncxx::document::view doc = *doc_opt;

        assert(doc.data() == data);
        assert(doc.length() == length);
        assert(doc == owner.view());
    }

    {
        bsoncxx::validator options;
        std::size_t offset = 123u;

        assert(bsoncxx::validate(data, length) ==
               bsoncxx::validate(data, length, options, &offset));

        // Not set when valid.
        assert(offset == 123u);
    }
}
// [Example]

}  // namespace

RUNNER_REGISTER_COMPONENT() {
    example();
}
