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

#include <mongo/bson/config/prelude.hpp>

#include <iostream>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <type_traits>

#include <mongo/bson/string_or_literal.hpp>
#include <mongo/bson/document/element.hpp>

namespace mongo {
namespace bson {
namespace document {

class LIBBSONCXX_API view {

   public:
    class iterator : public std::iterator<std::forward_iterator_tag, element> {
       public:
        iterator(const void* iter);
        iterator(bool is_end);

        const element& operator*() const;
        const element* operator->() const;

        iterator& operator++();
        iterator operator++(int);

        bool operator==(const iterator& rhs) const;
        bool operator!=(const iterator& rhs) const;

       private:
        element iter;
        bool is_end;
    };

    iterator begin() const;
    iterator end() const;

    bool has_key(const string_or_literal& key) const;

    element operator[](const string_or_literal& key) const;

    view();
    view(const std::uint8_t* b, std::size_t l);

    const std::uint8_t* get_buf() const;
    std::size_t get_len() const;

    friend std::ostream& operator<<(std::ostream& out, const bson::document::view& doc);

   protected:
    const std::uint8_t* buf;
    std::size_t len;

};

}  // namespace document
}  // namespace bson
}  // namespace mongo

#include <mongo/bson/config/postlude.hpp>
