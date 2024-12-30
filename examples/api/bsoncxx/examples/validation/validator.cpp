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

#include <cstddef>

#include <bsoncxx/validate.hpp>

#include <examples/api/runner.hh>
#include <examples/macros.hh>

namespace {

// [Example]
// {
//     "x": "a\0b",
//     "a.b": 1,
//     "v": {
//         "$numberInt": "123"
//     }
// }
void example(std::uint8_t const* bytes, std::size_t length) {
    // Default options.
    {
        bsoncxx::validator options;
        std::size_t offset;

        EXPECT(bsoncxx::validate(bytes, length, options, &offset));
    }

    // Validate UTF-8 strings.
    {
        bsoncxx::validator options;
        std::size_t offset;

        options.check_utf8(true);

        EXPECT(!bsoncxx::validate(bytes, length, options, &offset));

        // Offset of `"x": "\0"` relative to start of the document.
        EXPECT(offset == 4u);

        options.check_utf8_allow_null(true);

        EXPECT(bsoncxx::validate(bytes, length, options, &offset));
    }

    // Validate dot keys.
    {
        bsoncxx::validator options;
        std::size_t offset;

        options.check_dot_keys(true);

        EXPECT(!bsoncxx::validate(bytes, length, options, &offset));

        // Offset of `"a.b": 1` relative to start of the document.
        EXPECT(offset == 15u);
    }

    // Validate dollar keys.
    {
        bsoncxx::validator options;
        std::size_t offset;

        options.check_dollar_keys(true);

        EXPECT(!bsoncxx::validate(bytes, length, options, &offset));

        // Offset of `"$numberInt": "123"` relative to start of the sub-document. (CDRIVER-5710)
        EXPECT(offset == 4u);
    }
}
// [Example]

} // namespace

RUNNER_REGISTER_COMPONENT() {
    // clang-format off
    std::uint8_t bytes[] = {
        // Total Bytes: 53
        0x35, 0x00, 0x00, 0x00,
            // Element: String (Offset: 4)
            0x02,
                0x78, 0x00,             // Key: "x"
                0x04, 0x00, 0x00, 0x00, // Length: 4
                0x61, 0x00, 0x62, 0x00, // Value: "a\0b"

            // Element: Int32 (Offset: 15)
            0x10,
                0x61, 0x2e, 0x62, 0x00, // Key: "a.b"
                0x01, 0x00, 0x00, 0x00, // Value: 1

            // Element: Document (Offset: 24)
            0x03,
                0x76, 0x00,             // Key: "v"
                0x19, 0x00, 0x00, 0x00, // Total Bytes: 25
                    // Element: String (Offset: 31)
                    0x02,
                        // Key: "$numberInt"
                        0x24, 0x6e, 0x75, 0x6d, 0x62, 0x65, 0x72, 0x49, 0x6e, 0x74, 0x00,
                        0x04, 0x00, 0x00, 0x00, // Length: 4
                        0x31, 0x32, 0x33, 0x00, // Value: "123"
                0x00, // End of Document.
        0x00, // End of Document.
    };
    // clang-format on

    example(bytes, sizeof(bytes));
}
