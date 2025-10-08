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

#include <exception>
#include <iostream>

#include <examples/macros.hh>

void runner_register_component(void (*fn)(), char const* name);

void runner_register_component_with_instance(void (*fn)(), char const* name);

void runner_register_component_for_single(void (*fn)(), char const* name);
void runner_register_component_for_replica(void (*fn)(), char const* name);
void runner_register_component_for_sharded(void (*fn)(), char const* name);

void runner_register_forking_component(void (*fn)(), char const* name);

// Defined by examples/CMakeLists.txt.
#if !defined(EXAMPLES_COMPONENT_NAME)
#error "EXAMPLES_COMPONENT_NAME is not defined!"
#endif // !defined(EXAMPLES_COMPONENT_NAME)

#define EXAMPLES_COMPONENT_NAME_STR EXAMPLES_COMPONENT_NAME_STR_IMPL(EXAMPLES_COMPONENT_NAME)
#define EXAMPLES_COMPONENT_NAME_STR_IMPL(name) EXAMPLES_STR(name)

#define RUNNER_REGISTER_COMPONENT_IMPL(name, register_fn)                                                              \
    static void EXAMPLES_CONCAT3(name, _entry_point_, __LINE__)(void);                                                 \
    static void EXAMPLES_CONCAT4(name, _entry_point_, __LINE__, _guarded)(void) try {                                  \
        EXAMPLES_CONCAT3(name, _entry_point_, __LINE__)();                                                             \
    } catch (std::exception const& ex) {                                                                               \
        std::cout << EXAMPLES_STR(name) ":" << __LINE__ << ": failed: uncaught exception: " << ex.what() << std::endl; \
        throw;                                                                                                         \
    } catch (...) {                                                                                                    \
        std::cout << EXAMPLES_STR(name) ":" << __LINE__ << ": failed: uncaught exception" << std::endl;                \
        throw;                                                                                                         \
    }                                                                                                                  \
    static int EXAMPLES_CONCAT2(name, _registrator) =                                                                  \
        ((register_fn)(&EXAMPLES_CONCAT4(name, _entry_point_, __LINE__, _guarded), EXAMPLES_STR(name)), 0);            \
    static void EXAMPLES_CONCAT3(EXAMPLES_COMPONENT_NAME, _entry_point_, __LINE__)(void)

#define RUNNER_REGISTER_COMPONENT() RUNNER_REGISTER_COMPONENT_IMPL(EXAMPLES_COMPONENT_NAME, ::runner_register_component)

#define RUNNER_REGISTER_COMPONENT_WITH_INSTANCE() \
    RUNNER_REGISTER_COMPONENT_IMPL(EXAMPLES_COMPONENT_NAME, ::runner_register_component_with_instance)

#define RUNNER_REGISTER_COMPONENT_FOR_SINGLE() \
    RUNNER_REGISTER_COMPONENT_IMPL(EXAMPLES_COMPONENT_NAME, ::runner_register_component_for_single)

#define RUNNER_REGISTER_COMPONENT_FOR_REPLICA() \
    RUNNER_REGISTER_COMPONENT_IMPL(EXAMPLES_COMPONENT_NAME, ::runner_register_component_for_replica)

#define RUNNER_REGISTER_COMPONENT_FOR_SHARDED() \
    RUNNER_REGISTER_COMPONENT_IMPL(EXAMPLES_COMPONENT_NAME, ::runner_register_component_for_sharded)

#define RUNNER_REGISTER_FORKING_COMPONENT() \
    RUNNER_REGISTER_COMPONENT_IMPL(EXAMPLES_COMPONENT_NAME, ::runner_register_forking_component)
