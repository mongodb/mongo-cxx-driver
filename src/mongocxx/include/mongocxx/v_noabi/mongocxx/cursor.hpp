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

#include <memory>

#include <mongocxx/client-fwd.hpp>
#include <mongocxx/client_encryption-fwd.hpp>
#include <mongocxx/collection-fwd.hpp>
#include <mongocxx/cursor-fwd.hpp>
#include <mongocxx/database-fwd.hpp>
#include <mongocxx/index_view-fwd.hpp>
#include <mongocxx/search_index_view-fwd.hpp>

#include <bsoncxx/document/view.hpp>
#include <bsoncxx/stdx/optional.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
namespace v_noabi {

///
/// A cursor over the documents returned by a query to a MongoDB server.
///
/// Clients can iterate through a cursor::iterator to retrieve results.
///
/// @note By default, cursors timeout after 10 minutes of inactivity.
///
class cursor {
   public:
    enum class type { k_non_tailable, k_tailable, k_tailable_await };

    class iterator;

    ///
    /// Move constructs a cursor.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() cursor(cursor&&) noexcept;

    ///
    /// Move assigns a cursor.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(cursor&) operator=(cursor&&) noexcept;

    ///
    /// Destroys a cursor.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~cursor();

    cursor(cursor const&) = delete;
    cursor& operator=(cursor const&) = delete;

    ///
    /// A cursor::iterator that points to the beginning of any available
    /// results.  If begin() is called more than once, the cursor::iterator
    /// returned points to the next remaining result, not the result of
    /// the original call to begin().
    ///
    /// For a tailable cursor, when cursor.begin() == cursor.end(), no
    /// documents are available.  Each call to cursor.begin() checks again
    /// for newly-available documents.
    ///
    /// @return the cursor::iterator
    ///
    /// @throws mongocxx::v_noabi::query_exception if the query failed
    ///
    MONGOCXX_ABI_EXPORT_CDECL(iterator) begin();

    ///
    /// A cursor::iterator indicating cursor exhaustion, meaning that
    /// no documents are available from the cursor.
    ///
    /// @return the cursor::iterator
    ///
    MONGOCXX_ABI_EXPORT_CDECL(iterator) end();

   private:
    friend ::mongocxx::v_noabi::client_encryption;
    friend ::mongocxx::v_noabi::client;
    friend ::mongocxx::v_noabi::collection;
    friend ::mongocxx::v_noabi::database;
    friend ::mongocxx::v_noabi::index_view;
    friend ::mongocxx::v_noabi::search_index_view;

    friend ::mongocxx::v_noabi::cursor::iterator;

    cursor(void* cursor_ptr, bsoncxx::v_noabi::stdx::optional<type> cursor_type = bsoncxx::v_noabi::stdx::nullopt);

    class impl;
    std::unique_ptr<impl> _impl;
};

///
/// An input iterator of documents in a MongoDB cursor
/// result set.
///
/// All non-end iterators derived from the same mongocxx::v_noabi::cursor move in
/// lock-step.  Dereferencing any non-end() iterator always gives the first
/// remaining document in the cursor.  Incrementing one non-end iterator is
/// equivalent to incrementing them all.
///
/// An iterator is 'exhausted' when no documents are available. An
/// end-iterator is always exhausted. A non-end iterator is exhausted when the
/// originating mongocxx::v_noabi::cursor has no more documents.  When an iterator is
/// exhausted, it must not be dereferenced or incremented.
///
/// For iterators of a tailable cursor, calling cursor.begin() may revive an
/// exhausted iterator so that it no longer compares equal to the
/// end-iterator.
///
class cursor::iterator {
   public:
    ///
    /// std::iterator_traits
    ///
    using value_type = bsoncxx::v_noabi::document::view;
    using reference = bsoncxx::v_noabi::document::view&;
    using pointer = bsoncxx::v_noabi::document::view*;
    using iterator_category = std::input_iterator_tag;
    using difference_type = std::ptrdiff_t;

    ///
    /// Dereferences the view for the document currently being pointed to.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::document::view const&) operator*() const;

    ///
    /// Accesses a member of the dereferenced document currently being pointed to.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::document::view const*) operator->() const;

    ///
    /// Pre-increments the iterator to move to the next document.
    ///
    /// @throws mongocxx::v_noabi::query_exception if the query failed
    ///
    MONGOCXX_ABI_EXPORT_CDECL(iterator&) operator++();

    ///
    /// Post-increments the iterator to move to the next document.
    ///
    /// @throws mongocxx::v_noabi::query_exception if the query failed
    ///
    MONGOCXX_ABI_EXPORT_CDECL(void) operator++(int);

   private:
    friend ::mongocxx::v_noabi::cursor;

    ///
    /// @relates mongocxx::v_noabi::mongocxx::cursor::iterator
    ///
    /// Compare two iterators for (in)-equality. Iterators compare equal if
    /// they point to the same underlying cursor or if both are exhausted.
    ///
    /// @{
    friend MONGOCXX_ABI_EXPORT_CDECL(bool) operator==(iterator const&, iterator const&);
    friend MONGOCXX_ABI_EXPORT_CDECL(bool) operator!=(iterator const&, iterator const&);
    /// @}
    ///

    bool is_exhausted() const;

    explicit iterator(cursor* cursor);

    // If this pointer is null, the iterator is considered "past-the-end".
    cursor* _cursor;
};

} // namespace v_noabi
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::cursor.
///
