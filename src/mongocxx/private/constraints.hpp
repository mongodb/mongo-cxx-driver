// Copyright 2014 MongoDB Inc.
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

#include <mongocxx/config/private/prelude.hpp>

#include <cstdint>
#include <limits>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

template <typename T>
bool is_int32_duration(const T& duration) {
    const auto count = duration.count();
    return (count >= 0) && (count <= std::numeric_limits<std::int32_t>::max());
}

MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx

#include <mongocxx/config/private/postlude.hpp>
