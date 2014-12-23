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

#include <memory>

#include "bson/document.hpp"

namespace mongo {
namespace driver {

class collection;

class LIBMONGOCXX_EXPORT cursor {

   public:
    class iterator;

    cursor(cursor&& other) noexcept;
    cursor& operator=(cursor&& rhs) noexcept;

    ~cursor();

    iterator begin();
    iterator end();

   private:
    friend class collection;

    cursor(void* cursor_ptr);

    class impl;
    std::unique_ptr<impl> _impl;

}; // class cursor

class cursor::iterator : public std::iterator<
    std::input_iterator_tag,
    bson::document::view
> {

   public:
    const bson::document::view& operator*() const;
    const bson::document::view* operator->() const;

    iterator& operator++();
    void operator++(int);

   private:
    friend class cursor;
    friend bool operator==(const iterator&, const iterator&);
    friend bool operator!=(const iterator&, const iterator&);

    explicit iterator(cursor* cursor);

    cursor* _cursor;
    bson::document::view _doc;

}; // class iterator

bool operator==(const cursor::iterator& lhs, const cursor::iterator& rhs);
bool operator!=(const cursor::iterator& lhs, const cursor::iterator& rhs);

}  // namespace driver
}  // namespace mongo

#include "driver/config/postlude.hpp"
