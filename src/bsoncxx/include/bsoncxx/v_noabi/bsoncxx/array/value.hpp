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

#include <bsoncxx/array/value-fwd.hpp> // IWYU pragma: export

//

#include <bsoncxx/v1/array/value.hpp> // IWYU pragma: export

#include <cstdint>
#include <cstdlib>
#include <memory>
#include <utility>

#include <bsoncxx/array/view.hpp>
#include <bsoncxx/document/element.hpp>
#include <bsoncxx/document/value.hpp>

#include <bsoncxx/config/prelude.hpp>

namespace bsoncxx {
namespace v_noabi {
namespace array {

///
/// A read-only BSON array that owns its underlying buffer.
///
/// When a array::value goes out of scope, the underlying buffer is freed. Generally this class
/// should be used sparingly; array::view should be used instead wherever possible.
///
class value {
   private:
    v1::array::value _value;
    std::size_t _length;

   public:
    /// @copydoc v_noabi::document::value::deleter_type
    using deleter_type = void(BSONCXX_ABI_CDECL*)(std::uint8_t*);

    /// @copydoc v_noabi::document::value::unique_ptr_type
    using unique_ptr_type = std::unique_ptr<uint8_t[], deleter_type>;

    /// @copydoc v_noabi::document::value::const_iterator
    using const_iterator = v_noabi::array::view::const_iterator;

    /// @copydoc v_noabi::document::value::iterator
    using iterator = const_iterator;

    ~value() = default;

    value(value&&) = default;
    value& operator=(value&&) = default;

    value(value const& other) : value{other.view()} {}

    value& operator=(value const& other) {
        *this = value{other.view()};
        return *this;
    }

    ///
    /// Constructs a value from a buffer.
    ///
    /// This constructor transfers ownership of the buffer to the resulting
    /// value. A user-provided deleter is used to destroy the buffer.
    ///
    /// @warning For backward compatibility, `length` is NOT validated. When `length` is inconsistent with the embedded
    /// length as indicated by the BSON bytes, the BSON bytes may be parsed as "invalid" despite the BSON bytes
    /// themselves being valid.
    ///
    /// @param data
    ///   A pointer to a buffer containing a valid BSON array.
    /// @param length
    ///   The length of the document.
    /// @param deleter
    ///   A user provided deleter.
    ///
    value(std::uint8_t* data, std::size_t length, deleter_type deleter)
        : _value{data, std::move(deleter)}, _length{length} {}

    ///
    /// Constructs a value from a std::unique_ptr to a buffer. The ownership
    /// of the buffer is transferred to the resulting value.
    ///
    /// @warning For backward compatibility, `length` is NOT validated. When `length` is inconsistent with the embedded
    /// length as indicated by the BSON bytes, the BSON bytes may be parsed as "invalid" despite the BSON bytes
    /// themselves being valid.
    ///
    /// @param ptr
    ///   A pointer to a buffer containing a valid BSON array.
    /// @param length
    ///   The length of the document.
    ///
    value(unique_ptr_type ptr, std::size_t length) : _value{std::move(ptr)}, _length{length} {}

    ///
    /// Constructs a value from a view of an array. The data referenced
    /// by the array::view will be copied into a new buffer managed by the
    /// constructed value.
    ///
    /// @param view
    ///   A view of another array to copy.
    ///
    explicit BSONCXX_ABI_EXPORT_CDECL() value(v_noabi::array::view view);

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
    explicit operator v1::array::value() const& {
        return _value;
    }

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
    explicit operator v1::array::value() && {
        _length = 0u;
        return std::move(_value);
    }

    ///
    /// @returns A const_iterator to the first element of the array.
    ///
    const_iterator cbegin() const {
        return this->view().cbegin();
    }

    ///
    /// @returns A const_iterator to the past-the-end element of the array.
    ///
    const_iterator cend() const {
        return this->view().cend();
    }

    ///
    /// @returns A const_iterator to the first element of the array.
    ///
    const_iterator begin() const {
        return this->view().begin();
    }

    ///
    /// @returns A const_iterator to the past-the-end element of the array.
    ///
    const_iterator end() const {
        return this->view().end();
    }

    /// @copydoc bsoncxx::v_noabi::array::view::find(std::uint32_t i) const
    const_iterator find(std::uint32_t i) const {
        return const_iterator{*_value.view().find(i)};
    }

    /// @copydoc bsoncxx::v_noabi::array::view::operator[](std::uint32_t i) const
    v_noabi::document::element operator[](std::uint32_t i) const {
        return _value.operator[](i);
    }

    /// @copydoc bsoncxx::v_noabi::array::view::data() const
    std::uint8_t const* data() const {
        return _value.data();
    }

    /// @copydoc bsoncxx::v_noabi::array::view::size() const
    std::size_t size() const {
        return _length; // Do NOT use _value.size().
    }

    /// @copydoc bsoncxx::v_noabi::array::view::length() const
    std::size_t length() const {
        return _length; // Do NOT use _value.length().
    }

    ///
    /// Return true when `this->length() == 5`.
    ///
    /// @warning For backward compatibility, this function does NOT check if the underlying BSON bytes represent a valid
    /// empty document.
    ///
    bool empty() const {
        return _length == 5; // Do NOT use _value.empty().
    }

    ///
    /// Get a view over the array owned by this value.
    ///
    v_noabi::array::view view() const noexcept {
        return {_value.data(), _length};
    }

    ///
    /// Conversion operator that provides a view given a value.
    ///
    /// @return A view over the value.
    ///
    /* explicit(false) */ operator v_noabi::array::view() const noexcept {
        return this->view();
    }

    ///
    /// Transfer ownership of the underlying buffer to the caller.
    ///
    /// @warning
    ///   After calling release() it is illegal to call any methods
    ///   on this class, unless it is subsequently moved into.
    ///
    /// @return A std::unique_ptr with ownership of the buffer.
    ///
    unique_ptr_type release() {
        auto ptr = _value.release();

        // Invariant: the underlying deleter type MUST be `deleter_type`.
        auto const deleter_ptr = ptr.get_deleter().target<deleter_type>();

        // Invariant: `ptr` implies `deleter_ptr`, but not the reverse.
        return {ptr.release(), deleter_ptr ? *deleter_ptr : nullptr};
    }

    ///
    /// Replace the formerly-owned buffer with the new view.
    /// This will make a copy of the passed-in view.
    ///
    void reset(v_noabi::array::view view) {
        _value.reset(to_v1(view));
    }
};

} // namespace array
} // namespace v_noabi
} // namespace bsoncxx

namespace bsoncxx {
namespace v_noabi {

///
/// Convert from the @ref bsoncxx::v1 equivalent of `v`.
///
inline v_noabi::array::value from_v1(v1::array::value const& v) {
    return v_noabi::array::value{from_v1(v.view())};
}

///
/// Convert from the @ref bsoncxx::v1 equivalent of `v`.
///
/// @throws bsoncxx::v_noabi::exception with bsoncxx::v_noabi::error_code::k_invalid_deleter_type if the type of the
/// deleter for `value` is not @ref bsoncxx::v_noabi::array::value::deleter_type.
///
BSONCXX_ABI_EXPORT_CDECL(v_noabi::array::value) from_v1(v1::array::value&& v);

///
/// Convert to the @ref bsoncxx::v1 equivalent of `v`.
///
inline v1::array::value to_v1(v_noabi::array::value v) {
    return v1::array::value{std::move(v)};
}

} // namespace v_noabi
} // namespace bsoncxx

#include <bsoncxx/config/postlude.hpp>

///
/// @file
/// Provides @ref bsoncxx::v_noabi::array::value.
///
