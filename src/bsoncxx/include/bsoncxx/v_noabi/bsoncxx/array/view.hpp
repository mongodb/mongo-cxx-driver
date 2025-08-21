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

#include <bsoncxx/array/view-fwd.hpp>

//

#include <bsoncxx/v1/array/view.hpp>

#include <cstddef>
#include <cstdint>
#include <iterator>

#include <bsoncxx/types/bson_value/view-fwd.hpp>

#include <bsoncxx/array/element.hpp>
#include <bsoncxx/document/view.hpp>

#include <bsoncxx/config/prelude.hpp>

namespace bsoncxx {
namespace v_noabi {
namespace array {

///
/// A read-only, non-owning view of a BSON document.
///
class view {
   private:
    v_noabi::document::view _view;

   public:
    class const_iterator;

    ///
    /// Equivalent to @ref const_iterator.
    ///
    using iterator = const_iterator;

    ///
    /// Default constructs a view. The resulting view will be initialized to point at
    /// an empty BSON array.
    ///
    view() = default;

    ///
    /// Construct with the @ref bsoncxx::v1 equivalent.
    ///
    /* explicit(false) */ view(v1::array::view const& v) : _view{v} {}

    ///
    /// Constructs a view from a buffer. The caller is responsible for ensuring that
    /// the lifetime of the resulting view is a subset of the buffer's.
    ///
    /// @param data
    ///   A buffer containing a valid BSON array.
    /// @param length
    ///   The size of the buffer, in bytes.
    ///
    view(std::uint8_t const* data, std::size_t length) : _view{data, length} {}

    ///
    /// Convert to the @ref bsoncxx::v1 equivalent.
    ///
    /// @par Preconditions:
    /// - If `this->data()` is not null, the size of the storage region pointed to by `data` must be greater than or
    ///   equal to 5.
    /// - The "total number of bytes comprising the document" as indicated by the BSON bytes pointed-to by
    ///   `this->data()` must be less than or equal to the size of the storage region pointed to by `data`.
    ///
    /// @note `this->size()` is ignored.
    ///
    explicit operator v1::array::view() const {
        return v1::array::view{_view.data()};
    }

    ///
    /// @returns A const_iterator to the first element of the array.
    ///
    BSONCXX_ABI_EXPORT_CDECL(const_iterator) cbegin() const;

    ///
    /// @returns A const_iterator to the past-the-end element of the array.
    ///
    const_iterator cend() const;

    ///
    /// @returns A const_iterator to the first element of the array.
    ///
    const_iterator begin() const;

    ///
    /// @returns A const_iterator to the past-the-end element of the array.
    ///
    const_iterator end() const;

    ///
    /// Indexes into this BSON array. If the index is out-of-bounds, a past-the-end iterator
    /// will be returned. As BSON represents arrays as documents, the runtime of find() is
    /// linear in the length of the array.
    ///
    /// @param i
    ///   The index of the element.
    ///
    /// @return An iterator to the element if it exists, or the past-the-end iterator.
    ///
    BSONCXX_ABI_EXPORT_CDECL(const_iterator) find(std::uint32_t i) const;

    ///
    /// Indexes into this BSON array. If the index is out-of-bounds, the invalid array::element
    /// will be returned. As BSON represents arrays as documents, the runtime of operator[] is
    /// linear in the length of the array.
    ///
    /// @param i
    ///   The index of the element.
    ///
    /// @return The element if it exists, or the invalid element.
    ///
    v_noabi::array::element operator[](std::uint32_t i) const;

    ///
    /// Access the raw bytes of the underlying array.
    ///
    /// @return A (non-owning) pointer to the view's buffer.
    ///
    std::uint8_t const* data() const {
        return _view.data();
    }

    ///
    /// Gets the length of the underlying buffer.
    ///
    /// @remark This is not the number of elements in the array.
    /// To compute the number of elements, use std::distance.
    ///
    /// @return The length of the array, in bytes.
    ///
    std::size_t size() const {
        return _view.size();
    }

    /// @copydoc size() const
    std::size_t length() const {
        return _view.length();
    }

    ///
    /// Checks if the underlying buffer is empty, i.e. it is equivalent to
    /// the trivial array '[]'.
    ///
    /// @return true if the underlying document is empty.
    ///
    bool empty() const {
        return _view.empty();
    }

    ///
    /// Conversion operator unwrapping a document::view
    ///
    operator v_noabi::document::view() const {
        return _view;
    }

    ///
    /// @relates bsoncx::v_noabi::document::view
    ///
    /// Compare two views for (in)-equality
    ///
    /// @{
    friend bool operator==(view lhs, view rhs) {
        return lhs._view == rhs._view;
    }

    friend bool operator!=(view lhs, view rhs) {
        return !(lhs == rhs);
    }
    /// @}
    ///
};

///
/// A const iterator over the contents of an array view.
///
/// This iterator type provides a const forward iterator interface to array
/// view elements.
///
class view::const_iterator {
   public:
    ///
    /// std::iterator_traits
    ///
    using value_type = element;
    using reference = element&;
    using pointer = element*;
    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;

    const_iterator() = default;

    explicit const_iterator(element const& element) : _element{element} {}

    reference operator*() {
        return _element;
    }

    pointer operator->() {
        return &_element;
    }

    BSONCXX_ABI_EXPORT_CDECL(const_iterator&) operator++();

    const_iterator operator++(int) {
        const_iterator before(*this);
        operator++();
        return before;
    }

    ///
    /// @relates bsoncxx::v_noabi::array::view::const_iterator
    ///
    /// Compare two const_iterators for (in)-equality.
    ///
    /// @{
    friend bool operator==(const_iterator const& lhs, const_iterator const& rhs) {
        return lhs._element.raw() == rhs._element.raw() && lhs._element.offset() == rhs._element.offset();
    }

    friend bool operator!=(const_iterator const& lhs, const_iterator const& rhs) {
        return !(lhs == rhs);
    }
    /// @}
    ///

   private:
    element _element;
};

inline v_noabi::array::element view::operator[](std::uint32_t i) const {
    return *(this->find(i));
}

inline view::const_iterator view::cend() const {
    return {};
}

inline view::const_iterator view::begin() const {
    return this->cbegin();
}

inline view::const_iterator view::end() const {
    return this->cend();
}

} // namespace array
} // namespace v_noabi
} // namespace bsoncxx

namespace bsoncxx {
namespace v_noabi {

///
/// Convert to the @ref bsoncxx::v_noabi equivalent of `v`.
///
inline v_noabi::array::view from_v1(v1::array::view const& v) {
    return {v};
}

///
/// Convert to the @ref bsoncxx::v1 equivalent of `v`.
///
inline v1::array::view to_v1(v_noabi::array::view const& v) {
    return v1::array::view{v};
}

} // namespace v_noabi
} // namespace bsoncxx

#include <bsoncxx/config/postlude.hpp>

///
/// @file
/// Provides @ref bsoncxx::v_noabi::array::view.
///
/// @par Includes
/// - @ref bsoncxx/v1/array/view.hpp
///
