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

#include <examples/macros.hh>

void runner_register_fn(void (*fn)());

#if !defined(EXAMPLES_COMPONENT_NAME)
#error "EXAMPLES_COMPONENT_NAME is not defined!"
#endif  // !defined(EXAMPLES_COMPONENT_NAME)

#define RUNNER_REGISTER_COMPONENT()                                                               \
    static void EXAMPLES_CONCAT(EXAMPLES_COMPONENT_NAME, _entry_point)(void);                     \
    static int EXAMPLES_CONCAT(EXAMPLES_COMPONENT_NAME, _registerator) =                          \
        ([] { ::runner_register_fn(&EXAMPLES_CONCAT(EXAMPLES_COMPONENT_NAME, _entry_point)); }(), \
         0);                                                                                      \
    static void EXAMPLES_CONCAT(EXAMPLES_COMPONENT_NAME, _entry_point)(void)
