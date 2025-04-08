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

#include <vector>

#include <bsoncxx/stdx/string_view.hpp>

#include <bsoncxx/private/export.hh>

namespace bsoncxx {
namespace test_util {

// Convert the given string "1.2.3-suffix" into a vector [1, 2, 3].
BSONCXX_ABI_EXPORT_CDECL_TESTING(std::vector<int>) split_version(bsoncxx::stdx::string_view input);

} // namespace test_util
} // namespace bsoncxx
