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

#include <bsoncxx/document/value-fwd.hpp> // IWYU pragma: export

//

#include <bsoncxx/v1/detail/type_traits.hpp>
#include <bsoncxx/v1/document/value.hpp> // IWYU pragma: export
#include <bsoncxx/v1/stdx/string_view.hpp>

#include <cstdint>
#include <cstdlib>
#include <memory>
#include <type_traits>
#include <utility>

#include <bsoncxx/array/view.hpp>
#include <bsoncxx/document/element.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/stdx/type_traits.hpp> // IWYU pragma: keep: backward compatibity, to be removed.

#include <bsoncxx/config/prelude.hpp>

namespace bsoncxx {
namespace v_noabi {
namespace document {

///
/// A read-only BSON document that owns its underlying buffer.
///
/// When a document::value goes out of scope, the underlying buffer is freed. Generally this class
/// should be used sparingly; document::view should be used instead wherever possible.
///
class value {
   private:
    v1::document::value _value;
    std::size_t _length;

   public:
    ///
    /// The type of the deleter used to free the underlying BSON binary data.
    ///
    using deleter_type = void(BSONCXX_ABI_CDECL*)(std::uint8_t*);

    ///
    /// The type of the unique pointer used to manage the underlying BSON binary data.
    ///
    using unique_ptr_type = std::unique_ptr<std::uint8_t[], deleter_type>;

    /// @copydoc v_noabi::document::view::const_iterator
    using const_iterator = v_noabi::document::view::const_iterator;

    /// @copydoc v_noabi::document::view::iterator
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
    ///   A pointer to a buffer containing a valid BSON document.
    /// @param length
    ///   The length of the document.
    /// @param deleter
    ///   A user provided deleter.
    ///
    value(std::uint8_t* data, std::size_t length, deleter_type deleter)
        : _value{data, std::move(deleter)}, _length{length} {}

    ///
    /// Constructs a value from a std::unique_ptr to a buffer. The ownership
    /// of the buffer is transferred to the constructed value.
    ///
    /// @warning For backward compatibility, `length` is NOT validated. When `length` is inconsistent with the embedded
    /// length as indicated by the BSON bytes, the BSON bytes may be parsed as "invalid" despite the BSON bytes
    /// themselves being valid.
    ///
    /// @param ptr
    ///   A pointer to a buffer containing a valid BSON document.
    /// @param length
    ///   The length of the document.
    ///
    value(unique_ptr_type ptr, std::size_t length) : _value{std::move(ptr)}, _length{length} {}

    ///
    /// Constructs a value from a view of a document. The data referenced
    /// by the document::view will be copied into a new buffer managed by the
    /// constructed value.
    ///
    /// @param view
    ///   A view of another document to copy.
    ///
    explicit BSONCXX_ABI_EXPORT_CDECL() value(v_noabi::document::view view);

    ///
    /// Constructor used for serialization of user objects. This uses argument-dependent lookup
    /// to find the function declaration
    /// `void to_bson(T& t, bsoncxx::v_noabi::document::value doc)`.
    ///
    /// @par Constraints:
    /// - `T` is not @ref bsoncxx::v_noabi::array::view.
    ///
    /// @param t
    ///   A user-defined object to serialize into a BSON object.
    ///
    template <typename T, detail::requires_not_t<int, std::is_same<T, array::view>> = 0>
    explicit value(T const& t) : value({}) {
        to_bson(t, *this);
    }

    ///
    /// Assignment used for serialization of user objects. This uses argument-dependent lookup
    /// to find the function declaration
    /// `void to_bson(T& t, bsoncxx::v_noabi::document::value doc)`.
    ///
    /// @note `T` is not constrained!
    ///
    /// @param t
    ///   A user-defined object to serialize into a BSON object.
    ///
    template <typename T>
    value& operator=(T const& t) {
        *this = value{t};
        return *this;
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
    explicit operator v1::document::value() const& {
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
    explicit operator v1::document::value() && {
        _length = 0u;
        return std::move(_value);
    }

    ///
    /// @returns A const_iterator to the first element of the document.
    ///
    const_iterator cbegin() const {
        return this->view().cbegin();
    }

    ///
    /// @returns A const_iterator to the past-the-end element of the document.
    ///
    const_iterator cend() const {
        return this->view().cend();
    }

    ///
    /// @returns A const_iterator to the first element of the document.
    ///
    const_iterator begin() const {
        return this->view().begin();
    }

    ///
    /// @returns A const_iterator to the past-the-end element of the document.
    ///
    const_iterator end() const {
        return this->view().end();
    }

    /// @copydoc bsoncxx::v_noabi::document::view::find(bsoncxx::v1::stdx::string_view key) const
    const_iterator find(v1::stdx::string_view key) const {
        return const_iterator{*_value.find(key)};
    }

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
    v_noabi::document::element operator[](v1::stdx::string_view key) const {
        return _value.operator[](key);
    }

    ///
    /// Access the raw bytes of the underlying document.
    ///
    /// @return A pointer to the value's buffer.
    ///
    std::uint8_t const* data() const {
        return _value.data();
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
        return _length; // Do NOT use _value.size().
    }

    /// @copydoc size() const
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
    /// Get a view over the document owned by this value.
    ///
    v_noabi::document::view view() const noexcept {
        return {_value.data(), _length}; // Do NOT use _value.view().
    }

    ///
    /// Conversion operator that provides a view given a value.
    ///
    /// @return A view over the value.
    ///
    /* explicit(false) */ operator v_noabi::document::view() const noexcept {
        return this->view();
    }

    ///
    /// Constructs an object of type T from this document object. This method uses
    /// argument-dependent lookup to find the function declaration
    /// `void from_bson(T& t, bsoncxx::v_noabi::document::view const& doc)`.
    ///
    /// @note Type T must be default-constructible. Otherwise, use `void get(T& t)`.
    ///
    template <typename T>
    T get() {
        T temp{};
        from_bson(temp, this->view());
        return temp;
    }

    ///
    /// Constructs an object of type T from this document object. This method uses
    /// argument-dependent lookup to find the function declaration
    /// `void from_bson(T& t, bsoncxx::v_noabi::document::view const& doc)`.
    ///
    /// @param t
    ///   The object to construct. The contents of the document object will be deserialized
    ///   into t.
    ///
    template <typename T>
    void get(T& t) {
        from_bson(t, this->view());
    }

    ///
    /// Transfer ownership of the underlying buffer to the caller.
    ///
    /// @warning
    ///   After calling release() it is illegal to call any methods
    ///   on this class, unless it is subsequently moved into.
    ///
    /// @return A std::unique_ptr with ownership of the buffer. If the pointer is null, the deleter may also be null.
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
    void reset(v_noabi::document::view view) {
        _value.reset(to_v1(view));
    }
};

///
/// Compares two document values for (in)-equality.
///
/// @{

/// @relatesalso bsoncxx::v_noabi::document::value
inline bool operator==(value const& lhs, value const& rhs) {
    return (lhs.view() == rhs.view());
}

/// @relatesalso bsoncxx::v_noabi::document::value
inline bool operator!=(value const& lhs, value const& rhs) {
    return !(lhs == rhs);
}

/// @}
///

} // namespace document
} // namespace v_noabi
} // namespace bsoncxx

namespace bsoncxx {
namespace v_noabi {

///
/// Convert from the @ref bsoncxx::v1 equivalent of `v`.
///
inline v_noabi::document::value from_v1(v1::document::value const& v) {
    return v_noabi::document::value{from_v1(v.view())};
}

///
/// Convert from the @ref bsoncxx::v1 equivalent of `v`.
///
BSONCXX_ABI_EXPORT_CDECL(v_noabi::document::value) from_v1(v1::document::value&& v);

///
/// Convert to the @ref bsoncxx::v1 equivalent of `v`.
///
inline v1::document::value to_v1(v_noabi::document::value v) {
    return v1::document::value{std::move(v)};
}

} // namespace v_noabi
} // namespace bsoncxx

namespace bsoncxx {
namespace document {

using v_noabi::document::operator==;
using v_noabi::document::operator!=;

} // namespace document
} // namespace bsoncxx

#include <bsoncxx/config/postlude.hpp>

///
/// @file
/// Provides @ref bsoncxx::v_noabi::document::value.
///
/// @par Includes
/// - @ref bsoncxx/v1/document/value.hpp
///
