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

#define BSONCXX_SUPPRESS_DEPRECATION_WARNINGS_BEGIN                     \
    BSONCXX_PRIVATE_WARNINGS_PUSH();                                    \
    BSONCXX_PRIVATE_WARNINGS_DISABLE(GNU("-Wdeprecated-declarations")); \
    BSONCXX_PRIVATE_WARNINGS_DISABLE(MSVC(4996));

#define BSONCXX_SUPPRESS_DEPRECATION_WARNINGS_END BSONCXX_PRIVATE_WARNINGS_POP();
