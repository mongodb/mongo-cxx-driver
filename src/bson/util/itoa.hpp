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

#include "driver/config/prelude.hpp"

#include <cstddef>
#include <cstdint>

namespace bson {
namespace util {

class itoa {
   public:
    itoa();
    itoa(uint32_t i);
    itoa(const itoa& rhs);
    itoa& operator=(const itoa& rhs);
    itoa& operator=(uint32_t i);

    const char* c_str() const;
    std::size_t length() const;

   private:
    void init();

    uint32_t _val;
    const char* _str;
    uint8_t _len;
    char _buf[11];
};

}  // namespace util
}  // namespace bson

#include "driver/config/postlude.hpp"
