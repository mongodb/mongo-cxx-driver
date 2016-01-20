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

#include <bsoncxx/validate.hpp>

#include <array>

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>

using namespace bsoncxx;

// These are basic sanity tests to make sure the API works.
// Libbson has a much more complete suite of tests that covers various corrupt BSON
// inputs.

namespace {
// polymorphic lambdas would be nice here.
template <typename T>
bool is_engaged(const stdx::optional<T>& opt) {
    return opt != stdx::nullopt;
}

template <typename T>
bool is_disengaged(const stdx::optional<T>& opt) {
    return opt == stdx::nullopt;
}
}  // namespace

TEST_CASE("validate accepts bson we produce", "[bsoncxx::validate]") {
    builder::stream::document doc{};
    doc << "hello"
        << "world";
    auto view = doc.view();
    // option has explicit bool conversion operator
    REQUIRE(is_engaged(validate(view.data(), view.length())));
}

TEST_CASE("validate doesn't accept random bytes", "[bsoncxx::validate]") {
    std::array<uint8_t, 12> arr{{0xDE, 0xAD, 0xBE, 0xEF, 0xF0, 0x0B, 0x45}};
    REQUIRE(is_disengaged(validate(arr.data(), arr.size())));
}

TEST_CASE("configuring optional validations", "[bsoncxx::validate]") {
    builder::stream::document doc{};
    validator vtor{};

    SECTION("we can check for valid utf8") {
        vtor.check_utf8(true);

        // an invalid 3 octet sequence - thanks stackoverflow!
        std::string invalid_utf8("\0xF0\0x28\0x8C\0x28", 3);
        doc << "bar" << invalid_utf8;
        auto view = doc.view();
        REQUIRE(is_disengaged(validate(view.data(), view.length(), vtor)));
    }

    SECTION("we can check for valid utf8, but allow null bytes") {
        vtor.check_utf8_allow_null(true);
        std::string valid_utf8_with_null("foo\0\0bar", 8);
        doc << "bar" << valid_utf8_with_null;
        auto view = doc.view();
        REQUIRE(is_engaged(validate(view.data(), view.length(), vtor)));
    }

    SECTION("we can check for dollar keys") {
        vtor.check_dollar_keys(true);

        SECTION("at top level") {
            doc << "$foo"
                << "bar";
            auto view = doc.view();
            REQUIRE(is_disengaged(validate(view.data(), view.length(), vtor)));
        }

        SECTION("and in nested documents") {
            using namespace bsoncxx::builder::stream;
            doc << "foo" << open_array << open_document << "garply" << open_array << open_document
                << "$bar"
                << "baz" << close_document << close_array << close_document << close_array;

            auto view = doc.view();
            REQUIRE(is_disengaged(validate(view.data(), view.length(), vtor)));
        }
    }

    SECTION("we can check for dot keys") {
        vtor.check_dot_keys(true);

        SECTION("at top level") {
            doc << "foo.noooo"
                << "bar";
            auto view = doc.view();
            REQUIRE(is_disengaged(validate(view.data(), view.length(), vtor)));
        }

        SECTION("and in nested documents") {
            using namespace bsoncxx::builder::stream;
            doc << "foo" << open_array << open_document << "garply" << open_array << open_document
                << "bad.dot"
                << "baz" << close_document << close_array << close_document << close_array;

            auto view = doc.view();
            REQUIRE(is_disengaged(validate(view.data(), view.length(), vtor)));
        }
    }

    SECTION("we can get the invalid offset") {
        doc << "foo"
            << "bar"
            << "baz"
            << "garply";
        auto view = doc.view();

        // write a null byte at offset 9
        const_cast<uint8_t*>(view.data())[9] = '\0';

        std::size_t invalid_offset{0u};

        REQUIRE(is_disengaged(validate(view.data(), view.length(), vtor, &invalid_offset)));

        REQUIRE(invalid_offset == std::size_t{9});
    }
}
