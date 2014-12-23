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

#include <iostream>
#include <ctime>

namespace bson {

class string_or_literal;

class oid {
   public:
    struct init_tag_t {};
    static constexpr init_tag_t init_tag{};

    oid();

    explicit oid(init_tag_t tag);
    explicit oid(const char* bytes, std::size_t len);
    explicit oid(const string_or_literal& sol);

    string_or_literal to_string() const;

    friend bool operator<(const oid& lhs, const oid& rhs);
    friend bool operator>(const oid& lhs, const oid& rhs);
    friend bool operator<=(const oid& lhs, const oid& rhs);
    friend bool operator>=(const oid& lhs, const oid& rhs);
    friend bool operator==(const oid& lhs, const oid& rhs);
    friend bool operator!=(const oid& lhs, const oid& rhs);

    explicit operator bool() const;

    std::time_t get_time_t() const;

    friend std::ostream& operator<<(std::ostream& out, const oid& rhs);

    const char* bytes() const;

   private:
    friend int oid_compare(const oid& lhs, const oid& rhs);

    bool _is_valid;
    char _bytes[12];
};

}  // namespace bson

#include "driver/config/postlude.hpp"
