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

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <iterator>
#include <type_traits>

#include <mongo/bson/document/element.hpp>
#include <mongo/bson/string_or_literal.hpp>

namespace mongo {
namespace bson {
namespace document {

class LIBBSONCXX_API view {

   public:
    class iterator;
    class const_iterator;

    const_iterator cbegin() const;
    const_iterator cend() const;

    iterator begin() const;
    iterator end() const;

    iterator find(const string_or_literal& key) const;
    element operator[](const string_or_literal& key) const;

    view();

    view(const std::uint8_t* data, std::size_t length);

    const std::uint8_t* data() const;
    std::size_t length() const;

   private:
    const std::uint8_t* _data;
    std::size_t _length;
};

class view::iterator : public std::iterator<std::forward_iterator_tag, element> {
   public:
    iterator();
    explicit iterator(const element& element);

    reference operator*();
    pointer operator->();

    iterator& operator++();
    iterator operator++(int);

    friend bool operator==(const iterator&, const iterator&);
    friend bool operator!=(const iterator&, const iterator&);

   private:
    element _element;
};

class view::const_iterator : public std::iterator<std::forward_iterator_tag, element, ptrdiff_t,
                                                  const element*, const element&> {
   public:
    const_iterator();
    explicit const_iterator(const element& element);

    reference operator*();
    pointer operator->();

    const_iterator& operator++();
    const_iterator operator++(int);

    friend bool operator==(const const_iterator&, const const_iterator&);
    friend bool operator!=(const const_iterator&, const const_iterator&);

   private:
    element _element;
};

}  // namespace document
}  // namespace bson
}  // namespace mongo

#include <mongo/bson/config/postlude.hpp>
