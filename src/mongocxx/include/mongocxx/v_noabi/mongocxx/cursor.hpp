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

#include <mongocxx/cursor-fwd.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/cursor.hpp> // IWYU pragma: export

#include <cstddef>
#include <iterator>
#include <memory> // IWYU pragma: keep: backward compatibility, to be removed.
#include <utility>

#include <mongocxx/client-fwd.hpp>            // IWYU pragma: keep: backward compatibility, to be removed.
#include <mongocxx/client_encryption-fwd.hpp> // IWYU pragma: keep: backward compatibility, to be removed.
#include <mongocxx/collection-fwd.hpp>        // IWYU pragma: keep: backward compatibility, to be removed.
#include <mongocxx/database-fwd.hpp>          // IWYU pragma: keep: backward compatibility, to be removed.
#include <mongocxx/index_view-fwd.hpp>        // IWYU pragma: keep: backward compatibility, to be removed.
#include <mongocxx/search_index_view-fwd.hpp> // IWYU pragma: keep: backward compatibility, to be removed.

#include <bsoncxx/document/view.hpp>
#include <bsoncxx/stdx/optional.hpp> // IWYU pragma: keep: backward compatibility, to be removed.

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
   private:
    v1::cursor _cursor;
    bsoncxx::v_noabi::document::view _doc;

   public:
    using type = v1::cursor::type;

    class iterator;

    ///
    /// Move constructs a cursor.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() cursor(cursor&& other) noexcept;

    ///
    /// Move assigns a cursor.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(cursor&) operator=(cursor&& other) noexcept;

    ///
    /// Destroys a cursor.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~cursor();

    ///
    /// This class is not copyable.
    ///
    cursor(cursor const& other) = delete;
    ///
    /// This class is not copyable.
    ///
    cursor& operator=(cursor const& other) = delete;

    ///
    /// Construct with the @ref mongocxx::v1 equivalent.
    ///
    /* explicit(false) */ cursor(v1::cursor cursor) : _cursor{std::move(cursor)} {}

    ///
    /// Convert to the @ref mongocxx::v1 equivalent.
    ///
    /// @par Postconditions:
    /// - `*this` is in an assign-or-destroy-only state.
    ///
    /// @warning Invalidates all associated iterators and views.
    ///
    explicit operator v1::cursor() && {
        return std::move(_cursor);
    }

    ///
    /// This class is not copyable.
    ///
    explicit operator v1::cursor() const& = delete;

    ///
    /// A cursor::iterator points to the beginning of any available results.
    ///
    /// The first call to begin() advances to the next available document. Consecutive calls to begin() only advance to
    /// the next available document at most once. The state of all iterators is tracked by the cursor itself, so
    /// advancing one iterator advances all iterators.
    ///
    /// For a non-tailable cursor, when cursor.begin() == cursor.end(), no more documents can be obtained with the
    /// cursor. Calling begin() will always return end().
    ///
    /// For a tailable cursor, when cursor.begin() == cursor.end(), no document is currently available. However, a
    /// subsequent call to begin() will request for more available documents.
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

    class internal;
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
    void operator++(int) {
        this->operator++();
    }

    ///
    /// @relates mongocxx::v_noabi::mongocxx::cursor::iterator
    ///
    /// Compare two iterators for (in)-equality. Iterators compare equal if
    /// they point to the same underlying cursor or if both are exhausted.
    ///
    /// @{
    friend MONGOCXX_ABI_EXPORT_CDECL(bool) operator==(iterator const&, iterator const&);

    friend bool operator!=(cursor::iterator const& lhs, cursor::iterator const& rhs) noexcept {
        return !(lhs == rhs);
    }
    /// @}
    ///

   private:
    friend cursor;

    explicit iterator(cursor* cursor);

    bool is_exhausted() const;

    // If this pointer is null, the iterator is considered "past-the-end".
    cursor* _cursor;
};

} // namespace v_noabi
} // namespace mongocxx

namespace mongocxx {
namespace v_noabi {

///
/// Convert to the @ref mongocxx::v_noabi equivalent of `v`.
///
inline v_noabi::cursor from_v1(v1::cursor v) {
    return {std::move(v)};
}

///
/// Convert to the @ref mongocxx::v1 equivalent of `v`.
///
inline v1::cursor to_v1(v_noabi::cursor v) {
    return v1::cursor{std::move(v)};
}

} // namespace v_noabi
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::cursor.
///
