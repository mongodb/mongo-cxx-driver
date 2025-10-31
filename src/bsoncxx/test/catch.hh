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

#pragma once

#include <bsoncxx/v1/detail/macros.hpp>

#include <bsoncxx/test/v1/stdx/optional.hh>
#include <bsoncxx/test/v1/stdx/string_view.hh>
#include <bsoncxx/test/v_noabi/to_string.hh>

#include <bsoncxx/document/view_or_value.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/oid.hpp>
#include <bsoncxx/stdx/operators.hpp>
#include <bsoncxx/stdx/optional.hpp>

#include <bsoncxx/test/stringify.hh>    // IWYU pragma: export
#include <bsoncxx/test/system_error.hh> // IWYU pragma: export

#include <catch2/catch_test_macros.hpp> // IWYU pragma: export // TEST_CASE, SECTION, CHECK, etc.
#include <catch2/catch_tostring.hpp>    // IWYU pragma: export // Catch::StringMaker

namespace Catch {

template <>
struct StringMaker<bsoncxx::oid> {
    static std::string convert(bsoncxx::oid const& value) {
        return value.to_string();
    }
};

template <>
struct StringMaker<bsoncxx::document::view> {
    static std::string convert(bsoncxx::document::view const& value) {
        return bsoncxx::to_json(value, bsoncxx::ExtendedJsonMode::k_relaxed);
    }
};

template <>
struct StringMaker<bsoncxx::document::view_or_value> {
    static std::string convert(bsoncxx::document::view_or_value const& value) {
        return bsoncxx::test::stringify(value.view());
    }
};

template <>
struct StringMaker<bsoncxx::document::value> {
    static std::string convert(bsoncxx::document::value const& value) {
        return bsoncxx::test::stringify(value.view());
    }
};

template <>
struct StringMaker<bsoncxx::types::bson_value::view> {
    static std::string convert(bsoncxx::types::bson_value::view const& value) {
        using bsoncxx::to_string;
        return '{' + to_string(value.type()) + ": " + to_string(value) + '}';
    }
};

template <>
struct StringMaker<bsoncxx::types::bson_value::value> {
    static std::string convert(bsoncxx::types::bson_value::value const& value) {
        return bsoncxx::test::stringify(value.view());
    }
};

template <>
struct StringMaker<bsoncxx::types::bson_value::view_or_value> {
    static std::string convert(bsoncxx::types::bson_value::view_or_value const& value) {
        return bsoncxx::test::stringify(value.view());
    }
};

template <>
struct StringMaker<bsoncxx::detail::strong_ordering> {
    static std::string convert(bsoncxx::detail::strong_ordering o) {
        if (o < nullptr) {
            return "[less-than]";
        } else if (o > nullptr) {
            return "[greater-than]";
        } else {
            return "[equal/equivalent]";
        }
    }
};

} // namespace Catch
