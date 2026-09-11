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

// Support validation of install prefix with build configuration `BSONCXX_ENABLE_UNSTABLE_ABI=OFF`.
#if !defined(BSONCXX_ENABLE_UNSTABLE_ABI)
#define BSONCXX_ENABLE_UNSTABLE_ABI 1
#endif

#if BSONCXX_ENABLE_UNSTABLE_ABI == 0
#include <bsoncxx/v1/document/value.hpp>
#include <bsoncxx/v1/document/view.hpp>
#include <bsoncxx/v1/element/view.hpp>
#include <bsoncxx/v1/types/view.hpp> // IWYU pragma: keep
#else
#include <bsoncxx/document/element.hpp>
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/types/view.hpp> // IWYU pragma: keep
#endif

#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <system_error>

namespace {

// clang-format off
static constexpr std::array<std::uint8_t, 22> bytes = {{
    0x16, 0x00, 0x00, 0x00,                         // (22 bytes) {
        0x02,                                       //   (string)
            0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x00,     //     "Hello":
            0x06, 0x00, 0x00, 0x00,                 //     (6 bytes)
                0x77, 0x6f, 0x72, 0x6c, 0x64, 0x00, //       "world"
    0x00,                                           // }
}};
// clang-format on

} // namespace

int main() try {
#if BSONCXX_ENABLE_UNSTABLE_ABI == 0
    using view_type = bsoncxx::v1::document::view;
    using value_type = bsoncxx::v1::document::value;
#else
    using view_type = bsoncxx::document::view;
    using value_type = bsoncxx::document::value;
#endif

    // {"Hello": "world"}
    view_type const view{bytes.data(), bytes.size()};
    value_type const value{view};

    auto const element = *value.begin();

    // "Hello, world!"
    std::cout << element.key() << ", " << element.get_string().value << "!" << std::endl;

    return EXIT_SUCCESS;
} catch (std::system_error const& ex) {
    std::cerr << "unexpected failure: " << ex.what() << std::endl;
    return EXIT_FAILURE;
}
