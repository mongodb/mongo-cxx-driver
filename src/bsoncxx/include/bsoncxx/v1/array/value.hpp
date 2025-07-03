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

#include <bsoncxx/v1/array/value-fwd.hpp>

//

#include <bsoncxx/v1/detail/prelude.hpp>

#include <bsoncxx/v1/array/view.hpp>
#include <bsoncxx/v1/config/export.hpp>
#include <bsoncxx/v1/document/value.hpp>

#include <cstdint>
#include <type_traits>
#include <utility>

namespace bsoncxx {
namespace v1 {
namespace array {

///
/// A BSON array.
///
/// @attention This feature is experimental! It is not ready for use!
///
class value {
   private:
    v1::document::value _value;

    template <typename T>
    struct is_valid_deleter : std::is_constructible<v1::document::value, std::uint8_t*, std::size_t, T> {};

   public:
    /// @copydoc v1::document::value::deleter_type
    using deleter_type = v1::document::value::deleter_type;

    /// @copydoc v1::document::value::default_deleter_type
    using default_deleter_type = v1::document::value::default_deleter_type;

    /// @copydoc v1::document::value::unique_ptr_type
    using unique_ptr_type = v1::document::value::unique_ptr_type;

    /// @copydoc v1::document::view::const_iterator
    using const_iterator = v1::document::view::const_iterator;

    /// @copydoc v1::document::view::iterator
    using iterator = const_iterator;

    /// @copydoc v1::document::value::~value()
    ~value() = default;

    /// @copydoc v1::document::value::value(v1::document::value&& other) noexcept
    value(value&& other) noexcept : _value{std::move(other._value)} {}

    /// @copydoc v1::document::value::operator=(v1::document::value&& other) noexcept
    value& operator=(value&& other) noexcept {
        _value = std::move(other._value);
        return *this;
    }

    /// @copydoc v1::document::value::value(v1::document::value const& other)
    value(value const& other) : _value(other._value) {}

    /// @copydoc v1::document::value::operator=(v1::document::value const& other)
    value& operator=(value const& other) {
        _value = other._value;
        return *this;
    }

    /// @copydoc v1::document::value::value()
    value() = default;

    /// @copydoc v1::document::value::value(std::uint8_t* data, std::size_t length, Deleter deleter)
    template <typename Deleter, detail::enable_if_t<is_valid_deleter<Deleter>::value>* = nullptr>
    value(std::uint8_t* data, std::size_t length, Deleter deleter) : _value{data, length, std::move(deleter)} {}

    /// @copydoc v1::document::value::value(std::uint8_t* data, std::size_t length)
    value(std::uint8_t* data, std::size_t length) : _value{data, length} {}

    /// @copydoc v1::document::value::value(v1::document::value::unique_ptr_type ptr, std::size_t length)
    value(unique_ptr_type ptr, std::size_t length) : _value{std::move(ptr), length} {}

    /// @copydoc v1::document::value::value(v1::document::view view)
    explicit value(v1::array::view view) : _value{view} {}

    /// @copydoc v1::document::value::get_deleter() const
    deleter_type const& get_deleter() const {
        return _value.get_deleter();
    }

    /// @copydoc v1::document::value::release()
    unique_ptr_type release() {
        return _value.release();
    }

    /// @copydoc v1::document::value::reset(v1::document::value v)
    void reset(value v) {
        _value = std::move(v._value);
    }

    /// @copydoc v1::document::value::reset(v1::document::view v)
    void reset(v1::array::view v) {
        *this = value{v};
    }

    ///
    /// Return a view of the BSON bytes as an array.
    ///
    v1::array::view view() const {
        return {_value.data(), _value.size()};
    }

    ///
    /// Implicitly convert to `this->view()`.
    ///
    /* explicit(false) */ operator v1::array::view() const {
        return this->view();
    }

    /// @copydoc v1::array::view::cbegin() const
    v1::array::view::const_iterator cbegin() const {
        return this->view().cbegin();
    }

    /// @copydoc v1::array::view::cend() const
    v1::array::view::const_iterator cend() const {
        return this->view().cend();
    }

    /// @copydoc v1::array::view::begin() const
    v1::array::view::const_iterator begin() const {
        return this->view().begin();
    }

    /// @copydoc v1::array::view::end() const
    v1::array::view::const_iterator end() const {
        return this->view().end();
    }

    /// @copydoc v1::array::view::find(std::uint32_t i) const
    v1::array::view::const_iterator find(std::uint32_t i) const {
        return this->view().find(i);
    }

    /// @copydoc v1::array::view::operator[](std::uint32_t i) const
    v1::element::view operator[](std::uint32_t i) const {
        return this->view()[i];
    }

    /// @copydoc v1::array::view::data() const
    std::uint8_t const* data() const {
        return this->view().data();
    }

    /// @copydoc v1::array::view::size() const
    std::size_t size() const {
        return this->view().size();
    }

    /// @copydoc v1::array::view::length() const
    std::size_t length() const {
        return this->view().length();
    }

    /// @copydoc v1::array::view::empty() const
    bool empty() const {
        return this->view().empty();
    }

    /// @copydoc v1::array::view::operator bool() const
    explicit operator bool() const {
        return this->view().operator bool();
    }

    /// @copydoc v1::array::view::operator==(v1::array::view lhs, v1::array::view rhs)
    friend bool operator==(value const& lhs, value const& rhs) {
        return lhs.view() == rhs.view();
    }

    /// @copydoc v1::array::view::operator!=(v1::array::view lhs, v1::array::view rhs)
    friend bool operator!=(value const& lhs, value const& rhs) {
        return !(lhs == rhs);
    }
};

} // namespace array
} // namespace v1
} // namespace bsoncxx

#include <bsoncxx/v1/detail/postlude.hpp>

///
/// @file
/// Provides @ref bsoncxx::v1::array::value.
///
/// @par Includes
/// - @ref bsoncxx/v1/array/view.hpp
/// - @ref bsoncxx/v1/document/value.hpp
///
