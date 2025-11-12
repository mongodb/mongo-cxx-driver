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

#include <string>
#include <system_error>

#include <bsoncxx/test/stringify.hh>

#include <catch2/catch_test_macros.hpp> // IWYU pragma: export
#include <catch2/catch_tostring.hpp>

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

template <>
struct Catch::StringMaker<std::error_category> {
    static std::string convert(std::error_category const& value) {
        return value.name();
    }
};

template <>
struct Catch::StringMaker<std::error_condition> {
    static std::string convert(std::error_condition const& value) {
        std::string res;

        res += value.category().name();
        res += ':';
        res += bsoncxx::test::stringify(value.value());

        return res;
    }
};
