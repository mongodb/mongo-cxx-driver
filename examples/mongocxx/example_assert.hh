// Copyright 2018-present MongoDB Inc.
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

// Avoid unused variable warnings when we assign a var just to assert on it in a debug build.
#ifdef NDEBUG
#define MONGOCXX_EXAMPLE_ASSERT(x) \
    do {                           \
        (void)sizeof(x);           \
    } while (0)
#else
#include <cassert>
#define MONGOCXX_EXAMPLE_ASSERT(x) assert(x)
#endif
