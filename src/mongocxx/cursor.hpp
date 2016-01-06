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

#include <mongocxx/config/prelude.hpp>

#include <memory>

#include <bsoncxx/document/view.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

class collection;

///
/// Class representing a pointer to the result set of a query on a MongoDB server.
///
/// Clients can iterate through a cursor::iterator to retrieve results.
///
/// @note By default, cursors timeout after 10 minutes of inactivity.
///
class MONGOCXX_API cursor {

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
    friend class client;
    friend class database;

    MONGOCXX_PRIVATE cursor(void* cursor_ptr);

    class MONGOCXX_PRIVATE impl;
    std::unique_ptr<impl> _impl;

};

///
/// Class representing an input iterator of documents in a MongoDB cursor result set.
///
class cursor::iterator : public std::iterator<
    std::input_iterator_tag,
    bsoncxx::document::view
> {

   public:

    ///
    /// Dereferences the view for the document currently being pointed to.
    ///
    const bsoncxx::document::view& operator*() const;

    ///
    /// Accesses a member of the dereferenced document currently being pointed to.
    ///
    const bsoncxx::document::view* operator->() const;

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
    friend MONGOCXX_API bool operator==(const iterator&, const iterator&);
    friend MONGOCXX_API bool operator!=(const iterator&, const iterator&);

    MONGOCXX_PRIVATE explicit iterator(cursor* cursor);

    cursor* _cursor;
    bsoncxx::document::view _doc;

};

MONGOCXX_API bool operator==(const cursor::iterator& lhs, const cursor::iterator& rhs);
MONGOCXX_API bool operator!=(const cursor::iterator& lhs, const cursor::iterator& rhs);

MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx

#include <mongocxx/config/postlude.hpp>
