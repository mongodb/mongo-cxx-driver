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

#include <bsoncxx/v1/stdx/string_view.hpp>

#include <functional>

namespace mongocxx {
namespace test {

// Execute `fn` in a forked subprocess.
//
// Should be invoked directly from the body of a TEST_CASE(), not within a SECTION(). Keep it simple!
//
// @param fn The function to be invoked within a subprocess.
// @param is_signal_ptr When not null, `*is_signal_ptr` is set to `true` if the return value is a signal rather than an
// exit code.
//
// @returns The exit code of the subprocess (`*is_signal_ptr` is `false`) or the signal used to kill the subprocess
// (`*is_signal_ptr` is `true`) .
int subprocess(std::function<void()> fn, bool* is_signal_ptr = nullptr);

#if !defined(_WIN32)

#define CHECK_SUBPROCESS(...)                                    \
    if (1) {                                                     \
        int const ret = mongocxx::test::subprocess(__VA_ARGS__); \
        CHECK(ret == 0);                                         \
    } else                                                       \
        ((void)0)

#define CHECK_FALSE_SUBPROCESS(...)                              \
    if (1) {                                                     \
        int const ret = mongocxx::test::subprocess(__VA_ARGS__); \
        CHECK(ret != 0);                                         \
    } else                                                       \
        ((void)0)

#else

#define CHECK_SUBPROCESS(...) SKIP("mongocxx::test::subprocess() is not supported")
#define CHECK_FALSE_SUBPROCESS(...) SKIP("mongocxx::test::subprocess() is not supported")

#endif // !defined(_WIN32)

} // namespace test
} // namespace mongocxx
