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

#include <cstddef>
#include <exception>
#include <mutex>
#include <sstream>
#include <string>
#include <vector>

#include <bsoncxx/stdx/string_view.hpp>

#include <bsoncxx/test/catch.hh>

namespace bsoncxx {
namespace test {

struct exception_guard_state {
    std::mutex m = {};
    std::exception_ptr ptr = {};
    stdx::string_view file = {};
    std::size_t line = {};
    stdx::string_view func = {};
    std::vector<std::string> ignored; // Cannot use INFO() in guarded regions.
};

#define BSONCXX_TEST_EXCEPTION_GUARD_RESET(e)          \
    if (1) {                                           \
        ((void)e);                                     \
        std::lock_guard<std::mutex> _eguard_lock{e.m}; \
        e.ptr = {};                                    \
        e.file = __FILE__;                             \
        e.line = __LINE__;                             \
        e.func = __func__;                             \
    } else                                             \
        ((void)0)

// Marks the beginning of a guarded region wherein any exceptions thrown are stored by exception
// guard state. Only the FIRST exception is stored; any others are caught and ignored.
#define BSONCXX_TEST_EXCEPTION_GUARD_BEGIN(e) \
    try {                                     \
        (void)e;                              \
    ((void)0)

// Marks the end of a guarded region.
#define BSONCXX_TEST_EXCEPTION_GUARD_END(e)                                      \
    (void)e;                                                                     \
    }                                                                            \
    catch (...) {                                                                \
        std::lock_guard<std::mutex> _eguard_lock{e.m};                           \
        if (!e.ptr) {                                                            \
            e.ptr = std::current_exception();                                    \
            e.file = __FILE__;                                                   \
            e.line = __LINE__;                                                   \
            e.func = __func__;                                                   \
        } else {                                                                 \
            std::ostringstream oss;                                              \
            oss << __FILE__ << ":" << __LINE__ << ": exception guard ignored: "; \
            try {                                                                \
                throw;                                                           \
            } catch (std::exception const& exc) {                                \
                oss << exc.what();                                               \
            } catch (Catch::TestFailureException const&) {                       \
                oss << "Catch::TestFailureException";                            \
            } catch (...) {                                                      \
                oss << "unknown exception";                                      \
            }                                                                    \
            e.ignored.push_back(oss.str());                                      \
        }                                                                        \
    }                                                                            \
    ((void)0)

// Rethrow the stored exception if present.
#define BSONCXX_TEST_EXCEPTION_GUARD_CHECK(e)          \
    if (1) {                                           \
        (void)e;                                       \
        std::lock_guard<std::mutex> _eguard_lock{e.m}; \
        for (auto const& log : e.ignored) {            \
            UNSCOPED_INFO(log);                        \
        }                                              \
        if (e.ptr) {                                   \
            std::rethrow_exception(e.ptr);             \
        }                                              \
    } else                                             \
        ((void)0)

} // namespace test
} // namespace bsoncxx
