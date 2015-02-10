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

#include <bsoncxx/config/prelude.hpp>

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <iterator>
#include <type_traits>

#include <bsoncxx/document/view.hpp>
#include <bsoncxx/array/element.hpp>
#include <bsoncxx/string_or_literal.hpp>

namespace bsoncxx {
BSONCXX_INLINE_NAMESPACE_BEGIN
namespace array {

class BSONCXX_API view {

   public:
    class iterator;
    class const_iterator;

    const_iterator cbegin() const;
    const_iterator cend() const;

    iterator begin() const;
    iterator end() const;

    iterator find(std::uint32_t i) const;
    element operator[](std::uint32_t i) const;

    view();

    view(const std::uint8_t* data, std::size_t length);

    const std::uint8_t* data() const;
    std::size_t length() const;

    operator document::view() const;

    friend BSONCXX_API bool operator==(view, view);
    friend BSONCXX_API bool operator!=(view, view);

   private:
    document::view _view;
};

class view::iterator : public std::iterator<std::forward_iterator_tag, element> {
   public:
    iterator();
    explicit iterator(const element& element);

    reference operator*();
    pointer operator->();

    iterator& operator++();
    iterator operator++(int);

    friend BSONCXX_API bool operator==(const iterator&, const iterator&);
    friend BSONCXX_API bool operator!=(const iterator&, const iterator&);

   private:
    element _element;
};

class view::const_iterator : public std::iterator<std::forward_iterator_tag, element, std::ptrdiff_t,
                                                  const element*, const element&> {
   public:
    const_iterator();
    explicit const_iterator(const element& element);

    reference operator*();
    pointer operator->();

    const_iterator& operator++();
    const_iterator operator++(int);

    friend BSONCXX_API bool operator==(const const_iterator&, const const_iterator&);
    friend BSONCXX_API bool operator!=(const const_iterator&, const const_iterator&);

   private:
    element _element;
};

}  // namespace array
BSONCXX_INLINE_NAMESPACE_END
}  // namespace bsoncxx

#include <bsoncxx/config/postlude.hpp>
