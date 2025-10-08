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

#include <bsoncxx/document/view_or_value.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/oid.hpp>
#include <bsoncxx/stdx/operators.hpp>
#include <bsoncxx/stdx/optional.hpp>

#include <bsoncxx/test/to_string.hh>

#include <catch2/catch_test_macros.hpp> // TEST_CASE, SECTION, CHECK, etc.
#include <catch2/catch_tostring.hpp>    // Catch::StringMaker

#define THROWS_WITH_CODE_IMPL(_assertion, _expr, _code)                               \
    if (1) {                                                                          \
        try {                                                                         \
            (void)(_expr);                                                            \
            INFO("expected an exception to be thrown: " #_expr);                      \
            _assertion(false);                                                        \
        } catch (Catch::TestFailureException const&) {                                \
            throw; /* Propagate Catch exceptions. */                                  \
        } catch (Catch::TestSkipException const&) {                                   \
            throw; /* Propagate Catch exceptions. */                                  \
        } catch (std::system_error const& ex) {                                       \
            using std::make_error_code;                                               \
            (void)ex; /* Avoid unused variable warnings. */                           \
            _assertion(ex.code() == (_code));                                         \
        } catch (...) {                                                               \
            /* Reuse `*_THROWS_AS` to handle the unexpected exception type. */        \
            BSONCXX_PRIVATE_CONCAT(_assertion, _THROWS_AS)(throw, std::system_error); \
        }                                                                             \
    } else                                                                            \
        ((void)0)

#define CHECK_THROWS_WITH_CODE(_expr, _code) THROWS_WITH_CODE_IMPL(CHECK, _expr, _code)
#define REQUIRE_THROWS_WITH_CODE(_expr, _code) THROWS_WITH_CODE_IMPL(REQUIRE, _expr, _code)

namespace Catch {

template <>
struct StringMaker<std::error_condition> {
    static std::string convert(std::error_condition const& value) {
        std::string res;

        res += value.category().name();
        res += ':';
        res += Catch::StringMaker<int>::convert(value.value());

        return res;
    }
};

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
        return StringMaker<bsoncxx::document::view>::convert(value.view());
    }
};

template <>
struct StringMaker<bsoncxx::document::value> {
    static std::string convert(bsoncxx::document::value const& value) {
        return StringMaker<bsoncxx::document::view>::convert(value.view());
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
        return StringMaker<bsoncxx::types::bson_value::view>::convert(value.view());
    }
};

template <>
struct StringMaker<bsoncxx::types::bson_value::view_or_value> {
    static std::string convert(bsoncxx::types::bson_value::view_or_value const& value) {
        return StringMaker<bsoncxx::types::bson_value::view>::convert(value.view());
    }
};

template <typename T>
struct StringMaker<bsoncxx::stdx::optional<T>> {
    static std::string convert(bsoncxx::stdx::optional<T> const& value) {
        if (value) {
            return StringMaker<T>::convert(value.value());
        }

        return "{nullopt}";
    }
};

template <>
struct StringMaker<bsoncxx::stdx::optional<bsoncxx::stdx::nullopt_t>> {
    static std::string convert(bsoncxx::stdx::optional<bsoncxx::stdx::nullopt_t> const&) {
        return "{nullopt}";
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
