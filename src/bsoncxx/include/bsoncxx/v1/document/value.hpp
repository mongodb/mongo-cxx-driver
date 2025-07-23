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

#include <bsoncxx/v1/document/value-fwd.hpp>

//

#include <bsoncxx/v1/detail/prelude.hpp>

#include <bsoncxx/v1/config/export.hpp>
#include <bsoncxx/v1/detail/type_traits.hpp>
#include <bsoncxx/v1/document/view.hpp>
#include <bsoncxx/v1/element/view.hpp>

#include <cstddef>
#include <cstring>
#include <functional>
#include <memory>
#include <type_traits>
#include <utility>

namespace bsoncxx {
namespace v1 {
namespace document {

///
/// A BSON document.
///
/// @attention This feature is experimental! It is not ready for use!
///
class value {
   public:
    ///
    /// The type of the deleter used to free the underlying BSON bytes.
    ///
    /// A deleter `D` that is wrapped by @ref deleter_type must:
    ///
    /// - be `std::is_nothrow_destructible<D>`,
    /// - be `std::is_nothrow_move_constructible<D>`,
    /// - not throw an exception when invoked with a `std::uint8_t*`.
    ///
    /// These requirements are the same as those for a `std::unique_ptr<T, D>` deleter `D`, but do not include nothrow
    /// move assignability due to being wrapped by @ref deleter_type.
    ///
    /// @important Despite being a `std::function<T>`, @ref deleter_type is assumed to behave like a C++23
    /// `std::move_only_function<T>`. That is, it is assumed to be nothrow destructible, nothrow move constructible, and
    /// nothrow move assignable.
    ///
    using deleter_type = std::function<void BSONCXX_ABI_CDECL(std::uint8_t*)>;

    ///
    /// The deleter used to free copied BSON bytes when a user-provided deleter is not specified.
    ///
    using default_deleter_type = std::default_delete<std::uint8_t[]>;

    ///
    /// The type of a pointer to @ref noop_deleter.
    ///
    using noop_deleter_type = void(BSONCXX_ABI_CDECL*)(std::uint8_t*);

    ///
    /// The deleter used to avoid freeing preallocated storage representing an empty BSON document.
    ///
    static BSONCXX_ABI_EXPORT_CDECL(void) noop_deleter(std::uint8_t*);

    ///
    /// The type of the unique pointer used to manage the underlying BSON bytes.
    ///
    using unique_ptr_type = std::unique_ptr<std::uint8_t[], deleter_type>;

   private:
    unique_ptr_type _data;

    template <typename T>
    using is_value = detail::is_alike<T, value>;

    template <typename T>
    using to_bson_expr = decltype(to_bson(std::declval<T const&>(), std::declval<value&>()));

    template <typename T>
    struct has_to_bson : detail::conjunction<detail::negation<is_value<T>>, detail::is_detected<to_bson_expr, T>> {};

    template <typename T>
    using from_bson_expr = decltype(from_bson(std::declval<T&>(), std::declval<v1::document::view>()));

    template <typename T>
    struct has_from_bson : detail::conjunction<detail::negation<is_value<T>>, detail::is_detected<from_bson_expr, T>> {
    };

    template <typename T>
    struct is_valid_deleter
        : detail::disjunction<
              // std::function<T> may not be nothrow move constructible across all pre-C++20 implementations.
              // Deliberately allow an exemption that assumes it is de facto nothrow move constructible.
              std::is_same<T, deleter_type>,
              // Same requirements for D in std::unique_ptr<T, D> but without (nothrow) move assignability which is not
              // required for T in std::function<T>.
              detail::conjunction<
                  std::is_convertible<T, deleter_type>,
                  std::is_nothrow_destructible<T>,
                  std::is_nothrow_move_constructible<T>>> {};

   public:
    /// @copydoc v1::document::view::const_iterator
    using const_iterator = v1::document::view::const_iterator;

    /// @copydoc v1::document::view::iterator
    using iterator = const_iterator;

    ///
    /// Destroy this object.
    ///
    ~value() = default;

    ///
    /// Move construction.
    ///
    /// @par Postconditions:
    /// - `other` is equivalent to a default-initialized value.
    ///
    value(value&& other) noexcept = default;

    ///
    /// Move assignment.
    ///
    /// @par Postconditions:
    /// - `other` is equivalent to a default-initialized value.
    ///
    value& operator=(value&& other) noexcept = default;

    ///
    /// Copy construction.
    ///
    /// The copied value is allocated using `operator new[]` and the deleter is set to @ref
    /// default_deleter_type.
    ///
    value(value const& other) : value{other.view()} {}

    ///
    /// Copy assignment.
    ///
    /// The copied value is allocated using `operator new[]` and the deleter is set to @ref
    /// default_deleter_type.
    ///
    value& operator=(value const& other) {
        *this = value{other.view()};
        return *this;
    }

    ///
    /// Initialize as an empty document (or array).
    ///
    /// The value is equivalent to a default-initialized @ref bsoncxx::v1::document::view and the deleter is set to @ref
    /// bsoncxx::v1::document::value::noop_deleter.
    ///
    /// @warning Modifying the pointed-to data after default construction is undefined behavior.
    ///
    value() : _data{const_cast<std::uint8_t*>(v1::document::view{}.data()), &noop_deleter} {}

    ///
    /// Initialize as owning `data` which will be freed with `deleter`.
    ///
    /// @par Constraints:
    /// - `Deleter` must satisfy the requirements described by @ref deleter_type.
    ///
    /// @par Preconditions:
    /// - If `data` is not null, the size of the storage region pointed to by `data` must be greater than or equal to 5.
    /// - The embedded length must be less than or equal to the size of the storage region pointed to by `data`.
    /// - `deleter` must be capable of freeing the storage region pointed to by `data`.
    ///
    template <typename Deleter, detail::enable_if_t<is_valid_deleter<Deleter>::value>* = nullptr>
    value(std::uint8_t* data, Deleter deleter) : _data{data, std::move(deleter)} {}

    ///
    /// Equivalent to @ref value(std::uint8_t* data, Deleter deleter), but validates the embedded length against
    /// `length`.
    ///
    /// @par Preconditions:
    /// - If `data` is not null, the size of the storage region pointed to by `data` must be greater than or equal to
    ///   `length`.
    ///
    /// @exception bsoncxx::v1::exception with @ref bsoncxx::v1::document::view::errc::invalid_length if `length` is
    /// less than `5` or less than `this->size()`.
    ///
    template <typename Deleter, detail::enable_if_t<is_valid_deleter<Deleter>::value>* = nullptr>
    value(std::uint8_t* data, std::size_t length, Deleter deleter) : value{data, std::move(deleter)} {
        (void)v1::document::view{data, length}; // May throw.
    }

    ///
    /// Initialize as owning `data` which will be freed with @ref default_deleter_type.
    ///
    /// @par Preconditions:
    /// - If `data` is not null, the size of the storage region pointed to by `data` must be greater than or equal to 5.
    /// - The embedded length must be less than or equal to the size of the storage region pointed to by `data`.
    ///
    explicit value(std::uint8_t* data) : value{data, default_deleter_type{}} {}

    ///
    /// Equivalent to @ref value(std::uint8_t* data), but validates the embedded length against `length`.
    ///
    /// @par Preconditions:
    /// - If `data` is not null, the size of the storage region pointed to by `data` must be greater than or equal to
    ///   `length`.
    ///
    /// @exception bsoncxx::v1::exception with @ref bsoncxx::v1::document::view::errc::invalid_length if `length` is
    /// less than `5` or less than `this->size()`.
    ///
    explicit value(std::uint8_t* data, std::size_t length) : value{data} {
        (void)v1::document::view{data, length}; // May throw.
    }

    ///
    /// Initialize as owning `ptr`.
    ///
    /// @par Preconditions:
    /// - If `data` is not null, the size of the storage region pointed to by `data` must be greater than or equal to 5.
    /// - The embedded length must be less than or equal to the size of the storage region pointed to by `data`.
    ///
    explicit value(unique_ptr_type ptr) : _data{std::move(ptr)} {}

    ///
    /// Equivalent to @ref value(unique_ptr_type ptr), but validates the embedded length against `length`.
    ///
    /// @par Preconditions:
    /// - If `ptr` is not null, the size of the storage region pointed to by `data` must be greater than or equal to
    ///   `length`.
    ///
    /// @exception bsoncxx::v1::exception with @ref bsoncxx::v1::document::view::errc::invalid_length if `length` is
    /// less than `5` or less than `this->size()`.
    ///
    explicit value(unique_ptr_type ptr, std::size_t length) : value{std::move(ptr)} {
        (void)v1::document::view{_data.get(), length}; // May throw.
    }

    ///
    /// Initialize with a copy of the BSON bytes referenced by `view`.
    ///
    /// If `view` is equivalent to a default-initialized @ref bsoncxx::v1::document::view, this value is
    /// equivalent to `value()`.
    ///
    /// If `view` is invalid, this value is equivalent to `value{nullptr}`.
    ///
    /// Otherwise, the copied value is allocated using `operator new[]` and the deleter is set to @ref
    /// default_deleter_type.
    ///
    explicit value(v1::document::view view) {
        if (!view) {
            return;
        }

        v1::document::view const empty;

        if (view.data() == empty.data()) {
            _data = unique_ptr_type{const_cast<std::uint8_t*>(empty.data()), &noop_deleter};
        } else {
            _data = unique_ptr_type{new std::uint8_t[view.size()], default_deleter_type{}};
            std::memcpy(_data.get(), view.data(), view.size());
        }
    }

    ///
    /// Equivalent to `to_bson(v, *this);` after default-initialization.
    ///
    /// @par Constraints:
    /// - `T` is not @ref bsoncxx::v1::document::value.
    /// - `to_bson(v, *this)` is a valid and unambiguous overload found by ADL.
    ///
    template <typename T, detail::enable_if_t<has_to_bson<T>::value>* = nullptr>
    explicit value(T const& v) : value{} {
        to_bson(v, *this); // ADL.
    }

    ///
    /// Equivalent to `this->reset(view)`.
    ///
    value& operator=(v1::document::view view) {
        this->reset(view);
        return *this;
    }

    ///
    /// Equivalent to `*this = value{v}`.
    ///
    /// @par Constraints:
    /// - `T` is not @ref bsoncxx::v1::document::value.
    /// - `to_bson(v, *this)` is a valid and unambiguous overload found by ADL.
    ///
    template <typename T, detail::enable_if_t<has_to_bson<T>::value>* = nullptr>
    value& operator=(T const& v) {
        *this = value{v};
        return *this;
    }

    ///
    /// Equivalent to `from_bson(v, this->view())`.
    ///
    /// @par Constraints:
    /// - `T` is not @ref bsoncxx::v1::document::value.
    /// - `from_bson(v, this->view())` is a valid and unambiguous overload found by ADL.
    ///
    template <typename T, detail::enable_if_t<has_from_bson<T>::value>* = nullptr>
    void get(T& v) const {
        from_bson(v, this->view()); // ADL.
    }

    ///
    /// Equivalent to `T value; this->get(value); return value;`.
    ///
    /// @par Constraints:
    /// - `T` is not @ref bsoncxx::v1::document::value.
    /// - `T` is default-constructible.
    /// - `from_bson(v, this->view())` is a valid and unambiguous overload found by ADL.
    ///
    template <
        typename T,
        detail::enable_if_t<detail::conjunction<std::is_default_constructible<T>, has_from_bson<T>>::value>* = nullptr>
    T get() const {
        T res;
        this->get(res);
        return res;
    }

    ///
    /// Return the current deleter.
    ///
    deleter_type const& get_deleter() const {
        return _data.get_deleter();
    }

    ///
    /// Release ownership of the underlying BSON bytes.
    ///
    unique_ptr_type release() {
        return std::move(_data);
    }

    ///
    /// Replace the underlying BSON bytes with `v`.
    ///
    void reset(value v) {
        *this = std::move(v);
    }

    ///
    /// Replace the underlying BSON bytes with a copy of `v`.
    ///
    /// If `v` is invalid, reset to a default-initialized value.
    ///
    /// The copied value is allocated using `operator new[]` and the deleter is set to @ref
    /// default_deleter_type.
    ///
    void reset(v1::document::view v) {
        *this = value{v};
    }

    ///
    /// Return a view of the BSON bytes as a document.
    ///
    v1::document::view view() const {
        return v1::document::view{_data.get()};
    }

    ///
    /// Implicitly convert to `this->view()`.
    ///
    /* explicit(false) */ operator v1::document::view() const {
        return this->view();
    }

    /// @copydoc v1::document::view::cbegin() const
    const_iterator cbegin() const {
        return this->view().cbegin();
    }

    /// @copydoc v1::document::view::cend() const
    const_iterator cend() const {
        return this->view().cend();
    }

    /// @copydoc v1::document::view::begin() const
    const_iterator begin() const {
        return this->view().begin();
    }

    /// @copydoc v1::document::view::end() const
    const_iterator end() const {
        return this->view().end();
    }

    /// @copydoc v1::document::view::find(v1::stdx::string_view key) const
    const_iterator find(v1::stdx::string_view key) const {
        return this->view().find(key);
    }

    /// @copydoc v1::document::view::operator[](v1::stdx::string_view key) const
    v1::element::view operator[](v1::stdx::string_view key) const {
        return this->view()[key];
    }

    /// @copydoc v1::document::view::data() const
    std::uint8_t const* data() const {
        return _data.get();
    }

    /// @copydoc v1::document::view::size() const
    std::size_t size() const {
        return this->view().size();
    }

    /// @copydoc v1::document::view::length() const
    std::size_t length() const {
        return this->view().length();
    }

    /// @copydoc v1::document::view::empty() const
    bool empty() const {
        return this->view().empty();
    }

    /// @copydoc v1::document::view::operator bool() const
    explicit operator bool() const {
        return this->view().operator bool();
    }

    /// @copydoc v1::document::view::operator==(v1::document::view lhs, v1::document::view rhs)
    friend bool operator==(value const& lhs, value const& rhs) {
        return lhs.view() == rhs.view();
    }

    /// @copydoc v1::document::view::operator!=(v1::document::view lhs, v1::document::view rhs)
    friend bool operator!=(value const& lhs, value const& rhs) {
        return !(lhs == rhs);
    }
};

} // namespace document
} // namespace v1
} // namespace bsoncxx

#include <bsoncxx/v1/detail/postlude.hpp>

///
/// @file
/// Provides @ref bsoncxx::v1::document::value.
///
/// @par Includes
/// - @ref bsoncxx/v1/document/view.hpp
/// - @ref bsoncxx/v1/element/view.hpp
///
