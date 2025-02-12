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

#include <memory>

namespace bsoncxx {

// Workaround lack of aligned allocation support prior to C++17.
//
// Unlike `std::aligned_storage` and `alignas`, this type does NOT propagate alignment requirements to the parent
// object. This permits correct behavior despite use with regular (alignment-unaware) allocation functions.
//
// `alignof(T)` must be explicitly provided by the user for correct behavior, as template parameters do not necessarily
// inherit attributes such as alignment, e.g. `std::size_t alignment = alignof(T)` does not work as expected.
template <std::size_t size, std::size_t alignment>
class aligned_storage {
    // Sanity check.
    static_assert(size >= alignment, "sizeof(T) >= alignof(T) must be true");

   private:
    unsigned char _storage[2u * size];

   public:
    void* get() BSONCXX_PRIVATE_IF_GNU_LIKE(__attribute__((assume_aligned(alignment))))
    {
        void* ptr = _storage;
        std::size_t space = sizeof(_storage);
        return std::align(alignment, size, ptr, space);
    }

    void const* get() const BSONCXX_PRIVATE_IF_GNU_LIKE(__attribute__((assume_aligned(alignment))))
    {
        return const_cast<aligned_storage*>(this)->get();
    }
};

} // namespace bsoncxx
