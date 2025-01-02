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

#include <cstdio>
#include <cstdlib>

#if defined(_MSC_VER)
#define EXAMPLES_CDECL __cdecl
#else
#define EXAMPLES_CDECL
#endif

#define EXAMPLES_CONCAT2(a, b) EXAMPLES_CONCAT_IMPL(a, b)
#define EXAMPLES_CONCAT3(a, b, c) EXAMPLES_CONCAT2(EXAMPLES_CONCAT2(a, b), c)
#define EXAMPLES_CONCAT4(a, b, c, d) EXAMPLES_CONCAT2(EXAMPLES_CONCAT2(a, b), EXAMPLES_CONCAT2(c, d))
#define EXAMPLES_CONCAT_IMPL(a, b) a##b

#define EXAMPLES_STR(e) #e

// Unconditionally `assert()` expectations in examples.
#define EXPECT(...)                                                                                     \
    if (!static_cast<bool>(__VA_ARGS__)) {                                                              \
        std::printf("%s:%d: %s: expectation failed: %s\n", __FILE__, __LINE__, __func__, #__VA_ARGS__); \
        std::fflush(stdout);                                                                            \
        std::abort();                                                                                   \
    } else                                                                                              \
        ((void)0)
