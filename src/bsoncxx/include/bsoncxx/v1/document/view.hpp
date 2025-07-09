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

#include <bsoncxx/v1/document/view-fwd.hpp>

//

#include <bsoncxx/v1/detail/prelude.hpp>

#include <bsoncxx/v1/config/export.hpp>
#include <bsoncxx/v1/element/view.hpp>
#include <bsoncxx/v1/stdx/string_view.hpp>

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iterator>
#include <system_error>

namespace bsoncxx {
namespace v1 {
namespace error {
namespace category {

///
/// Declares error categories for error codes declared in @ref bsoncxx::v1::error::document.
///
namespace document {

///
/// The error category for @ref bsoncxx::v1::error::document::view.
///
/// @attention This feature is experimental! It is not ready for use!
///
BSONCXX_ABI_EXPORT_CDECL(std::error_category const&) view();

} // namespace document
} // namespace category
} // namespace error
} // namespace v1
} // namespace bsoncxx

namespace bsoncxx {
namespace v1 {
namespace error {

///
/// Declares error codes returned by @ref bsoncxx::v1::document interfaces.
///
namespace document {

///
/// Errors codes which may be returned by @ref bsoncxx::v1::document::view.
///
/// @attention This feature is experimental! It is not ready for use!
///
enum class view {
    zero,         ///< Zero.
    invalid_data, ///< Data is invalid.
};

///
/// Support implicit conversion to `std::error_code`.
///
/// @attention This feature is experimental! It is not ready for use!
///
inline std::error_code make_error_code(view v) {
    return {static_cast<int>(v), v1::error::category::document::view()};
}

} // namespace document
} // namespace error
} // namespace v1
} // namespace bsoncxx

namespace std {

template <>
struct is_error_code_enum<bsoncxx::v1::error::document::view> : true_type {};

} // namespace std

namespace bsoncxx {
namespace v1 {
namespace document {

///
/// A non-owning, read-only BSON document.
///
/// An "invalid" view, as indicated by @ref operator bool() const, does not satisfy the minimum requirements of a valid
/// BSON document, which are that:
///
/// - @ref data() is not null, and
/// - @ref size() is not less than `5` (the minimum size of a BSON document).
///
/// The BSON bytes being represented is only validated as minimally required to satisfy a requested operation. When an
/// operation is not satisfiable due to invalid data, the operation will throw an @ref bsoncxx::v1::exception with @ref
/// bsoncxx::v1::error::document::view::invalid_data.
///
/// @attention This feature is experimental! It is not ready for use!
///
class view {
   public:
    class const_iterator;

    ///
    /// Equivalent to @ref const_iterator.
    ///
    using iterator = const_iterator;

   private:
    enum : std::size_t { _empty_length = 5u };

    std::uint8_t const* _data;
    std::size_t _length;

   public:
    ///
    /// Initialize as an empty view.
    ///
    /// @par Postconditions:
    /// - `this->data() != nullptr`
    /// - `this->size() == 5`
    /// - `this->empty() == true`
    ///
    BSONCXX_ABI_EXPORT_CDECL() view();

    ///
    /// Initialize with the given BSON bytes.
    ///
    /// @par Preconditions:
    /// - `length` must be less than or equal to the size of the storage region pointed to by `data`.
    ///
    view(std::uint8_t const* data, std::size_t length) : _data(data), _length(length) {}

    ///
    /// Return a pointer to the BSON bytes being represented.
    ///
    std::uint8_t const* data() const {
        return _data;
    }

    ///
    /// Return the length of the BSON bytes being represented.
    ///
    /// @note This returns the length as specified during initialization, not the length of the BSON bytes as
    /// indicated by the BSON bytes itself.
    ///
    std::size_t size() const {
        return _length;
    }

    /// @copydoc size() const
    std::size_t length() const {
        return _length;
    }

    ///
    /// Return true when the BSON bytes represents an empty view.
    ///
    /// @note This does not return true when this view is invalid.
    ///
    bool empty() const {
        return this->operator bool() && _data[4] == 0u;
    }

    ///
    /// Return true when this view is valid.
    ///
    /// @note This does not validate the BSON bytes being represented.
    ///
    explicit operator bool() const {
        return _data && _length >= _empty_length;
    }

    ///
    /// Return a const iterator to the beginning of the range of BSON elements within this view.
    ///
    /// If this view is invalid, returns an end iterator.
    ///
    /// @exception bsoncxx::v1::exception with @ref bsoncxx::v1::error::document::view::invalid_data if this operation
    /// failed due to invalid BSON bytes.
    ///
    BSONCXX_ABI_EXPORT_CDECL(const_iterator) cbegin() const;

    ///
    /// Return a const iterator to the end of the range of BSON elements within this view.
    ///
    const_iterator cend() const;

    /// @copydoc cbegin() const
    const_iterator begin() const;

    /// @copydoc cend() const
    const_iterator end() const;

    ///
    /// Return a const iterator to the element within the represented BSON document whose key compares equal to `key`.
    ///
    /// If this view is invalid or the requested field is not found, returns an end iterator.
    ///
    /// @par Complexity
    /// Linear.
    ///
    /// @exception bsoncxx::v1::exception with @ref bsoncxx::v1::error::document::view::invalid_data if this operation
    /// failed due to invalid BSON bytes.
    ///
    BSONCXX_ABI_EXPORT_CDECL(const_iterator) find(v1::stdx::string_view key) const;

    ///
    /// Return the first element within the represented BSON document whose key compares equal to `key`.
    ///
    /// If this view is invalid or the requested field is not found, returns an invalid element.
    ///
    /// @par Complexity
    /// Linear.
    ///
    /// @exception bsoncxx::v1::exception with @ref bsoncxx::v1::error::document::view::invalid_data if this operation
    /// failed due to invalid BSON bytes.
    ///
    v1::element::view operator[](v1::stdx::string_view key) const;

    ///
    /// Compare equal when the BSON bytes represented by `lhs` and `rhs` compare equal.
    ///
    /// An invalid view only compares equal to another invalid view. The underlying BSON bytes (if any) is ignored
    /// for an invalid document.
    ///
    /// @{
    friend bool operator==(view lhs, view rhs) {
        if (!lhs != !rhs) {
            return false;
        }

        return !lhs || (lhs.length() == rhs.length() && std::memcmp(lhs.data(), rhs.data(), lhs.length()) == 0);
    }

    friend bool operator!=(view lhs, view rhs) {
        return !(lhs == rhs);
    }
    /// @}
    ///
};

///
/// A const iterator over the elements of a view.
///
/// @note This iterator almost satisfies Cpp17ForwardIterator, but `std::iterator_traits<T>::reference` is defined as
/// `value_type`, similar to `std::vector<bool>::iterator` and `std::istreambuf_iterator<T>`. Therefore, this iterator
/// only fully satisfies Cpp17InputIterator.
///
/// @attention This feature is experimental! It is not ready for use!
///
class view::const_iterator {
   private:
    v1::element::view _element;

   public:
    ///
    /// Provide `std::iterator_traits<T>::iterator_category`.
    ///
    using iterator_category = std::input_iterator_tag;

    ///
    /// Provide `std::iterator_traits<T>::value_type`.
    ///
    using value_type = v1::element::view;

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
    /// Initialize as an end iterator.
    ///
    const_iterator() = default;

    ///
    /// Return the current element.
    ///
    /// If this is an end iterator, returns an invalid element.
    ///
    reference operator*() const {
        return _element;
    }

    ///
    /// Access the current element.
    ///
    /// If this is an end iterator, accesses an invalid element.
    ///
    pointer operator->() const {
        return &_element;
    }

    ///
    /// Pre-increment this iterator.
    ///
    /// If this is an end iterator, it remains an end iterator.
    ///
    BSONCXX_ABI_EXPORT_CDECL(const_iterator&) operator++();

    ///
    /// Post-increment this iterator.
    ///
    /// If this is an end iterator, it remains an end iterator.
    ///
    const_iterator operator++(int) {
        const_iterator tmp = *this;
        this->operator++();
        return tmp;
    }

    ///
    /// Compare equal when `lhs` and `rhs` point to the same element within the same range of BSON bytes.
    ///
    /// An end iterator only compares equal to another end iterator. The underlying BSON bytes (if any) is ignored
    /// for an end iterator.
    ///
    friend bool operator==(const_iterator const& lhs, const_iterator const& rhs) {
        if (!lhs._element != !rhs._element) {
            return false;
        }

        return !lhs._element ||
               (lhs._element.raw() == rhs._element.raw() && lhs._element.offset() == rhs._element.offset());
    }

    ///
    /// Equivalent to `!(lhs == rhs)`.
    ///
    friend bool operator!=(const_iterator const& lhs, const_iterator const& rhs) {
        return !(lhs == rhs);
    }

    class internal;

   private:
    explicit BSONCXX_ABI_EXPORT_CDECL() const_iterator(v1::element::view element);
};

inline view::const_iterator view::cend() const {
    return {};
}

inline view::const_iterator view::begin() const {
    return this->cbegin();
}

inline view::const_iterator view::end() const {
    return this->cend();
}

inline v1::element::view view::operator[](v1::stdx::string_view key) const {
    return *(this->find(key));
}

} // namespace document
} // namespace v1
} // namespace bsoncxx

#include <bsoncxx/v1/detail/postlude.hpp>

///
/// @file
/// Provides @ref bsoncxx::v1::document::view.
///
/// @par Includes
/// - @ref bsoncxx/v1/element/view.hpp
///
