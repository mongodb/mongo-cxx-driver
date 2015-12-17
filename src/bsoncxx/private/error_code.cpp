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

#include <bsoncxx/config/prelude.hpp>

#include <system_error>

#include <bsoncxx/exception/error_code.hpp>
#include <bsoncxx/private/error_category.hpp>

namespace bsoncxx {
BSONCXX_INLINE_NAMESPACE_BEGIN

std::error_code make_error_code(bsoncxx::error_code error) {
    return std::error_code(static_cast<int>(error), bsoncxx_error_category());
}

BSONCXX_INLINE_NAMESPACE_END
}  // namespace bsoncxx
