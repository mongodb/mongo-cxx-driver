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

#include <cstddef>
#include <cstdint>
#include <limits>
#include <memory>

#include <bsoncxx/stdx/string_view.hpp>

namespace examples {

// Used to trigger builder append failure.
struct big_string {
    // BSON_SIZE_MAX == 0x7FFFFFFF
    // Leave some room for CDRIVER-5732.
    std::size_t length{static_cast<std::size_t>(std::numeric_limits<std::int32_t>::max() - 32u)};

    // Allocate an UNINITIALIZED blob of data that will not be accessed due to length checks.
    // Leaving memory unitialized (rather than zero-init) should hopefully avoid slow and expensive
    // physical memory allocation at runtime.
    std::unique_ptr<char[]> data{new char[length]};

    bsoncxx::stdx::string_view view() const {
        return bsoncxx::stdx::string_view(data.get(), length);
    }
};

}  // namespace examples
