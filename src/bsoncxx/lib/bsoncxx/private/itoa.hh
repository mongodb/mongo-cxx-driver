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

#include <bsoncxx/private/export.hh>

namespace bsoncxx {

class itoa {
   private:
    // Exact value of UINT32_MAX for `sizeof()` below.
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers).
    static_assert(UINT32_MAX == std::uint32_t{4294967295}, "");

    std::uint32_t _val = {};
    char const* _str = {};
    std::uint8_t _len = {};
    char _buf[sizeof("4294967295")] = {};

   public:
    ~itoa() = default;
    itoa(itoa&& rhs) = delete;
    itoa& operator=(itoa&&) = delete;
    itoa(itoa const& rhs) = delete;
    itoa& operator=(itoa const&) = delete;

    itoa() : itoa(0u) {}

    explicit BSONCXX_ABI_EXPORT_CDECL_TESTING() itoa(std::uint32_t i);

    itoa& operator=(std::uint32_t new_value);

    std::uint32_t val() const {
        return _val;
    }

    char const* c_str() const {
        return _str;
    }

    std::size_t length() const {
        return _len;
    }

   private:
    void _init();
};

} // namespace bsoncxx
