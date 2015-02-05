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

#include <mongo/driver/config/prelude.hpp>

#include <memory>

#include <mongo/bson/document.hpp>

namespace mongo {
namespace driver {

class collection;

///
/// Class representing a pointer to the result set of a query on a MongoDB server.
///
/// Clients can iterate through a cursor::iterator to retrieve results.
///
/// @note By default, cursors timeout after 10 minutes of inactivity.
///
class LIBMONGOCXX_API cursor {

   public:

    class iterator;

    ///
    /// Move constructs a cursor.
    ///
    cursor(cursor&&) noexcept;

    ///
    /// Move assigns a cursor.
    ///
    cursor& operator=(cursor&&) noexcept;

    ///
    /// Destroys a cursor.
    ///
    ~cursor();

    /// A cursor::iterator that points to the begining of the results.
    ///
    /// @return the cursor::iterator
    iterator begin();

    /// A cursor::iterator that points to the end of the results.
    ///
    /// @return the cursor::iterator
    iterator end();

   private:
    friend class collection;

    cursor(void* cursor_ptr);

    class impl;
    std::unique_ptr<impl> _impl;

};

///
/// Class representing an input iterator of documents in a MongoDB cursor result set.
///
class cursor::iterator : public std::iterator<
    std::input_iterator_tag,
    bson::document::view
> {

   public:

    ///
    /// Dereferences the view for the document currently being pointed to.
    ///
    const bson::document::view& operator*() const;

    ///
    /// Accesses a member of the dereferenced document currently being pointed to.
    ///
    const bson::document::view* operator->() const;

    ///
    /// Postfix increments the iterator to move to the next document.
    ///
    iterator& operator++();

    ///
    /// Prefix increments the iterator to move to the next document.
    ///
    void operator++(int);

   private:
    friend class cursor;
    friend bool operator==(const iterator&, const iterator&);
    friend bool operator!=(const iterator&, const iterator&);

    explicit iterator(cursor* cursor);

    cursor* _cursor;
    bson::document::view _doc;

};

bool operator==(const cursor::iterator& lhs, const cursor::iterator& rhs);
bool operator!=(const cursor::iterator& lhs, const cursor::iterator& rhs);

}  // namespace driver
}  // namespace mongo

#include <mongo/driver/config/postlude.hpp>
