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

#include <bsoncxx/document/view-fwd.hpp>

//

#include <bsoncxx/v1/document/view.hpp>

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iterator>

#include <bsoncxx/document/element.hpp>
#include <bsoncxx/stdx/string_view.hpp>

#include <bsoncxx/config/prelude.hpp>

namespace bsoncxx {
namespace v_noabi {
namespace document {

///
/// A read-only, non-owning view of a BSON document.
///
class view {
   private:
    v1::document::view _view;
    std::size_t _length;

   public:
    class const_iterator;

    ///
    /// Equivalent to @ref const_iterator.
    ///
    using iterator = const_iterator;

    ///
    /// Default constructs a view. The resulting view will be initialized to point at
    /// an empty BSON document.
    ///
    view() : _view{}, _length{_view.length()} {}

    ///
    /// Construct with the @ref bsoncxx::v1 equivalent.
    ///
    /* explicit(false) */ view(v1::document::view const& v) : _view{v}, _length{v.length()} {}

    ///
    /// Constructs a view from a buffer. The caller is responsible for ensuring that
    /// the lifetime of the resulting view is a subset of the buffer's.
    ///
    /// @param data
    ///   A buffer containing a valid BSON document.
    /// @param length
    ///   The size of the buffer, in bytes.
    ///
    view(std::uint8_t const* data, std::size_t length) : _view{data}, _length{length} {}

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
    explicit operator v1::document::view() const {
        return _view;
    }

    ///
    /// @returns A const_iterator to the first element of the document.
    ///
    BSONCXX_ABI_EXPORT_CDECL(const_iterator) cbegin() const;

    ///
    /// @returns A const_iterator to the past-the-end element of the document.
    ///
    const_iterator cend() const;

    ///
    /// @returns A const_iterator to the first element of the document.
    ///
    const_iterator begin() const;

    ///
    /// @returns A const_iterator to the past-the-end element of the document.
    ///
    const_iterator end() const;

    ///
    /// Finds the first element of the document with the provided key. If there is
    /// no such element, the past-the-end iterator will be returned. The runtime of
    /// find() is linear in the length of the document. This method only searches
    /// the top-level document, and will not recurse to any subdocuments.
    ///
    /// @remark In BSON, keys are not required to be unique. If there are multiple
    /// elements with a matching key in the document, the first matching element from
    /// the start will be returned.
    ///
    /// @param key
    ///   The key to search for.
    ///
    /// @return An iterator to the matching element, if found, or the past-the-end iterator.
    ///
    BSONCXX_ABI_EXPORT_CDECL(const_iterator) find(v1::stdx::string_view key) const;

    ///
    /// Finds the first element of the document with the provided key. If there is no
    /// such element, the invalid document::element will be returned. The runtime of operator[]
    /// is linear in the length of the document.
    ///
    /// @param key
    ///   The key to search for.
    ///
    /// @return The matching element, if found, or the invalid element.
    ///
    v_noabi::document::element operator[](v1::stdx::string_view key) const;

    ///
    /// Access the raw bytes of the underlying document.
    ///
    /// @return A (non-owning) pointer to the view's buffer.
    ///
    std::uint8_t const* data() const {
        return _view.data();
    }

    ///
    /// Gets the length of the underlying buffer.
    ///
    /// @remark This is not the number of elements in the document.
    /// To compute the number of elements, use std::distance.
    ///
    /// @return The length of the document, in bytes.
    ///
    std::size_t size() const {
        return _length; // Do NOT use _view.size().
    }

    /// @copydoc size() const
    std::size_t length() const {
        return _length; // Do NOT use _view.length().
    }

    ///
    /// Checks if the underlying document is empty, i.e. it is equivalent to
    /// the trivial document '{}'.
    ///
    /// @return true if the underlying document is empty.
    ///
    bool empty() const {
        return _length == 5u; // Do NOT use _view.empty().
    }

    ///
    /// @relates bsoncxx::v_noabi::document::view
    ///
    /// Compare two document views for (in)-equality.
    ///
    /// @{
    friend bool operator==(view lhs, view rhs) {
        return (lhs._length == rhs._length) && (std::memcmp(lhs.data(), rhs.data(), lhs._length) == 0);
    }

    friend bool operator!=(view lhs, view rhs) {
        return !(lhs == rhs);
    }
    /// @}
    ///
};

///
/// A const iterator over the contents of a document view.
///
/// This iterator type provides a const forward iterator interface to document
/// view elements.
///
class view::const_iterator {
   private:
    v_noabi::document::element _element;

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

    explicit const_iterator(v_noabi::document::element const& element) : _element(element) {}

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
    /// @relates bsoncxx::v_noabi::document::view::const_iterator
    ///
    /// Compares two const_iterators for (in)-equality.
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
};

inline v_noabi::document::element view::operator[](v1::stdx::string_view key) const {
    return *(this->find(key));
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

} // namespace document
} // namespace v_noabi
} // namespace bsoncxx

namespace bsoncxx {
namespace v_noabi {

///
/// Convert to the @ref bsoncxx::v_noabi equivalent of `v`.
///
inline v_noabi::document::view from_v1(v1::document::view const& v) {
    return {v};
}

///
/// Convert to the @ref bsoncxx::v1 equivalent of `v`.
///
inline v1::document::view to_v1(v_noabi::document::view const& v) {
    return v1::document::view{v};
}

} // namespace v_noabi
} // namespace bsoncxx

#include <bsoncxx/config/postlude.hpp>

///
/// @file
/// Provides @ref bsoncxx::v_noabi::document::view.
///
/// @par Includes
/// - @ref bsoncxx/v1/document/view.hpp
///
