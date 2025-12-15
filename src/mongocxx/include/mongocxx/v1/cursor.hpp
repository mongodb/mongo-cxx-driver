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

#include <mongocxx/v1/cursor-fwd.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/detail/prelude.hpp>

#include <bsoncxx/v1/document/view-fwd.hpp>

#include <mongocxx/v1/config/export.hpp>

#include <cstddef>
#include <iterator>

namespace mongocxx {
namespace v1 {

///
/// A MongoDB cursor.
///
/// @see
/// - [Cursors (MongoDB Manual)](https://www.mongodb.com/docs/manual/core/cursors/)
///
/// @attention This feature is experimental! It is not ready for use!
///
class cursor {
   private:
    class impl;
    void* _impl;

   public:
    ///
    /// Enumeration identifying the type of the cursor.
    ///
    enum class type {
        ///
        /// [A non-tailable
        /// cursor](https://specifications.readthedocs.io/en/latest/client-side-operations-timeout/client-side-operations-timeout/).
        ///
        k_non_tailable,

        ///
        /// [A tailable
        /// cursor](https://specifications.readthedocs.io/en/latest/client-side-operations-timeout/client-side-operations-timeout/).
        ///
        k_tailable,

        ///
        /// [A tailable "awaitData"
        /// cursor](https://specifications.readthedocs.io/en/latest/client-side-operations-timeout/client-side-operations-timeout/).
        ///
        k_tailable_await,
    };

    class iterator;

    ///
    /// Destroy this object.
    ///
    /// @warning Invalidates all associated views and iterators.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~cursor();

    ///
    /// Move constructor.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() cursor(cursor&& other) noexcept;

    ///
    /// Move assignment.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(cursor&) operator=(cursor&& other) noexcept;

    ///
    /// This class is not copyable.
    ///
    cursor(cursor const&) = delete;

    ///
    /// This class is not copyable.
    ///
    cursor& operator=(cursor const&) = delete;

    ///
    /// Return an iterator over the results of the associated cursor.
    ///
    /// @important All iterators associated with the same cursor object share the same state.
    ///
    /// This function advances the underlying cursor to obtain the first available result document. The underlying
    /// cursor is only advanced at most once: consecutive calls to `this->begin()` do not advance the underlying cursor
    /// state. To obtain subsequent available result documents, the resulting iterator must be incremented instead.
    ///
    /// When an iterator compares equal to `this->end()`, a non-tailable cursor will no longer return any documents:
    /// `this->begin() == this->end()` will always be true. However, a tailable cursor may request additional result
    /// documents by calling `this->begin()` again: the behavior is the same as the first call to `this->begin()`.
    ///
    /// @throws mongocxx::v1::server_error when a server-side error is encountered and a raw server error is available.
    /// @throws mongocxx::v1::exception for all other runtime errors.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(iterator) begin();

    ///
    /// Return an end iterator.
    ///
    /// @important The end iterator has no associated cursor.
    ///
    iterator end() const;
};

///
/// An iterator over the results of an associated cursor.
///
/// @attention This feature is experimental! It is not ready for use!
///
/// @important All iterators associated with the same cursor object share the same state.
/// @important The end iterator has no associated cursor.
///
/// @note This iterator almost satisfies Cpp17ForwardIterator, but `std::iterator_traits<T>::reference` is defined as
/// `value_type`, similar to `std::vector<bool>::iterator` and `std::istreambuf_iterator<T>`. Therefore, this iterator
/// only fully satisfies Cpp17InputIterator.
///
/// @see
/// - [Cursors (MongoDB Manual)](https://www.mongodb.com/docs/manual/core/cursors/)
///
class cursor::iterator {
   private:
    void* _impl; // v1::cursor

   public:
    ///
    /// Provide `std::iterator_traits<T>::iterator_category`.
    ///
    using iterator_category = std::input_iterator_tag;

    ///
    /// Provide `std::iterator_traits<T>::value_type`.
    ///
    using value_type = bsoncxx::v1::document::view;

    ///
    /// Provide `std::iterator_traits<T>::difference_type`.
    ///
    using difference_type = std::ptrdiff_t;

    ///
    /// Provide `std::iterator_traits<T>::pointer`.
    ///
    using pointer = value_type const*;

    ///
    /// Provide `std::iterator_traits<T>::reference`.
    ///
    using reference = value_type;

    ///
    /// Destroy this object.
    ///
    /// @warning Invalidates all associated views.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~iterator();

    ///
    /// Move construction.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() iterator(iterator&& other) noexcept;

    ///
    /// Move assignment.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(iterator&) operator=(iterator&& other) noexcept;

    ///
    /// Copy construction.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() iterator(iterator const& other);

    ///
    /// Copy assignment.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(iterator&) operator=(iterator const& other);

    ///
    /// Initialize as an end iterator.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() iterator();

    ///
    /// Access the current cursor result document.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(value_type) operator*() const;

    ///
    /// Access the current result document.
    ///
    /// @par Preconditions:
    /// - `*this` does not compare equal to the end iterator.
    ///
    /// @returns Empty when there is no result document available.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(pointer) operator->() const;

    ///
    /// Increment this iterator.
    ///
    /// Advance the underlying cursor to obtain the next result document.
    /// Compare equal to the end iterator when there are no result documents available.
    ///
    /// @note Pre-increment and post-increment are equivalent.
    ///
    /// @warning Invalidates all views to the current result document.
    ///
    /// @throws mongocxx::v1::server_error when a server-side error is encountered and a raw server error is available.
    /// @throws mongocxx::v1::exception for all other runtime errors.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(iterator&) operator++();

    /// @copydoc operator++()
    iterator& operator++(int) {
        return this->operator++();
    }

    ///
    /// Equality comparison.
    ///
    /// `lhs` and `rhs` compare equal when:
    /// - both are associated with the same cursor object, or
    /// - both are end iterators, or
    /// - one is an end iterator and the other has no result document available.
    ///
    friend MONGOCXX_ABI_EXPORT_CDECL(bool) operator==(iterator const& lhs, iterator const& rhs);

    ///
    /// Equivalent to `!(lhs == rhs)`.
    ///
    friend bool operator!=(iterator const& lhs, iterator const& rhs) {
        return !(lhs == rhs);
    }
};

inline cursor::iterator cursor::end() const {
    return {};
}

} // namespace v1
} // namespace mongocxx

#include <mongocxx/v1/detail/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v1::cursor.
///
