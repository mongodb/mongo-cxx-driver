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

#include <cstddef>
#include <string>
#include <iostream>

namespace bson {

class string_or_literal {
   public:
    template <std::size_t n>
    constexpr string_or_literal(const char (&v)[n])
        : _len(n - 1), _is_owning(false), _literal(v) {}

    string_or_literal(std::string v);
    string_or_literal(const char* str, std::size_t len);

    string_or_literal();

    string_or_literal(const string_or_literal& rhs);
    string_or_literal(string_or_literal&& rhs);
    string_or_literal& operator=(const string_or_literal& rhs);
    string_or_literal& operator=(string_or_literal&& rhs);

    ~string_or_literal();

    std::size_t length() const;
    const char* c_str() const;

    friend std::ostream& operator<<(std::ostream& out, const string_or_literal& rhs);

   private:
    std::size_t _len;
    bool _is_owning;

    union {
        std::string _string;
        const char* _literal;
    };
};

}  // namespace bson

#include "driver/config/postlude.hpp"
