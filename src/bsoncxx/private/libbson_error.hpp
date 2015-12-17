// Copyright 2015 MongoDB Inc.
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

#include <bsoncxx/config/prelude.hpp>

#include <limits>
#include <system_error>

namespace bsoncxx {
BSONCXX_INLINE_NAMESPACE_BEGIN

///
/// Translate a libbson error into a std::error_code.
///
/// @param code A libbson error code
/// @param domain A libbson error domain
///
/// @return an error_coderepresenting a libbson error
///
std::error_code make_error_code(int code, int domain);

BSONCXX_INLINE_NAMESPACE_END
}  // namespace bsoncxx

#include <bsoncxx/config/postlude.hpp>
