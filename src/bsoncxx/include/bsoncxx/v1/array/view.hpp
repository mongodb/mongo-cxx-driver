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

#include <bsoncxx/v1/array/view-fwd.hpp>

//

#include <bsoncxx/v1/detail/prelude.hpp>

#include <bsoncxx/v1/config/export.hpp>
#include <bsoncxx/v1/document/view.hpp>
#include <bsoncxx/v1/element/view.hpp>

#include <cstddef>
#include <cstdint>
#include <iterator>

namespace bsoncxx {
namespace v1 {
namespace array {

///
/// A non-owning, read-only BSON array.
///
/// An "invalid" view, as indicated by @ref operator bool() const, does not satisfy the minimum requirements of a valid
/// BSON document, which are that:
///
/// - @ref data() is not null, and
/// - @ref size() is not less than `5` (the minimum size of a BSON document).
///
/// The BSON binary data being represented is only validated as minimally required to satisfy a requested operation.
/// When an operation is not satisfiable due to invalid data, the operation will throw an @ref bsoncxx::v1::exception
/// with @ref bsoncxx::v1::error::document::view::invalid_data.
///
/// @attention This feature is experimental! It is not ready for use!
///
class view {
   public:
    ///
    /// Equivalent to @ref const_iterator.
    ///
    using const_iterator = v1::document::view::const_iterator;

    ///
    /// Equivalent to @ref const_iterator.
    ///
    using iterator = const_iterator;

   private:
    v1::document::view _view;

   public:
    /// @copydoc v1::document::view::view()
    view() = default;

    /// @copydoc v1::document::view::view(std::uint8_t const* data, std::size_t length)
    view(std::uint8_t const* data, std::size_t length) : _view{data, length} {}

    /// @copydoc v1::document::view::data() const
    std::uint8_t const* data() const {
        return _view.data();
    }

    /// @copydoc v1::document::view::size() const
    std::size_t size() const {
        return _view.size();
    }

    /// @copydoc v1::document::view::length() const
    std::size_t length() const {
        return _view.length();
    }

    /// @copydoc v1::document::view::empty() const
    bool empty() const {
        return _view.empty();
    }

    /// @copydoc v1::document::view::operator bool() const
    explicit operator bool() const {
        return _view.operator bool();
    }

    /// @copydoc v1::document::view::cbegin() const
    BSONCXX_ABI_EXPORT_CDECL(const_iterator) cbegin() const;

    /// @copydoc v1::document::view::cend() const
    const_iterator cend() const {
        return {};
    }

    /// @copydoc v1::document::view::cbegin() const
    const_iterator begin() const {
        return this->cbegin();
    }

    /// @copydoc v1::document::view::cend() const
    const_iterator end() const {
        return this->cend();
    }

    ///
    /// Return a const iterator to the element within the represented BSON array at index `i`.
    ///
    /// If this view is invalid, returns an end iterator.
    ///
    /// @exception bsoncxx::v1::exception with @ref bsoncxx::v1::error::document::view::invalid_data if this operation
    /// failed due to invalid BSON binary data.
    ///
    BSONCXX_ABI_EXPORT_CDECL(const_iterator) find(std::uint32_t i) const;

    /// @copydoc find(std::uint32_t i) const
    v1::element::view operator[](std::uint32_t i) const {
        return *(this->find(i));
    }

    ///
    /// Implicitly convert to a @ref bsoncxx::v1::document::view.
    ///
    /* explicit(false) */ operator v1::document::view() const {
        return _view;
    }

    /// @copydoc v1::document::view::operator==(v1::document::view const& lhs, v1::document::view const& rhs)
    friend bool operator==(view const& lhs, view const& rhs) {
        return lhs._view == rhs._view;
    }

    /// @copydoc v1::document::view::operator!=(v1::document::view const& lhs, v1::document::view const& rhs)
    friend bool operator!=(view const& lhs, view const& rhs) {
        return !(lhs == rhs);
    }
};

} // namespace array
} // namespace v1
} // namespace bsoncxx

#include <bsoncxx/v1/detail/postlude.hpp>

///
/// @file
/// Provides @ref bsoncxx::v1::array::view.
///
/// @par Includes
/// - @ref bsoncxx/v1/document/view.hpp
/// - @ref bsoncxx/v1/element/view.hpp
///
