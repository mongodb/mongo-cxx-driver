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

#include <bsoncxx/test_util/export_for_testing.hh>

#include <bsoncxx/config/private/prelude.hh>

namespace bsoncxx {

class itoa {
   public:
    explicit BSONCXX_ABI_EXPORT_CDECL_TESTING() itoa(uint32_t i = 0);

    ~itoa() = default;

    itoa(itoa&& rhs) = delete;
    itoa& operator=(itoa&&) = delete;

    itoa(const itoa& rhs) = delete;
    itoa& operator=(const itoa&) = delete;

    BSONCXX_ABI_EXPORT_CDECL_TESTING(itoa&) operator=(uint32_t new_value);

    uint32_t val() const {
        return _val;
    }

    const char* c_str() const {
        return _str;
    }

    std::size_t length() const {
        return _len;
    }

   private:
    void _init();

    uint32_t _val;
    const char* _str;
    uint8_t _len;
    char _buf[11];
};

}  // namespace bsoncxx

#include <bsoncxx/config/private/postlude.hh>
