// Copyright 2015 MongoDB Inc.
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

#include <exception>
#include <initializer_list>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <utility>

#include "./operators.hpp"
#include "./type_traits.hpp"

// Try to detect std::optional. We'll reuse some components from there for compatibility
// with C++17 users.
#define BSONCXX_HAVE_STDLIB_OPTIONAL 0

#ifdef __has_include
#if __has_include(<version>)
#include <version>
#if defined(__cpp_lib_optional) && __cpp_lib_optional >= 201606L
#undef BSONCXX_HAVE_STDLIB_OPTIONAL
#define BSONCXX_HAVE_STDLIB_OPTIONAL 1
#include <optional>
#endif
#endif
#endif

#include <bsoncxx/config/prelude.hpp>

namespace bsoncxx {

namespace v_noabi {

namespace stdx {

/**
 * @brief Implementation of an std::optional-like class template
 *
 * Presents mostly the same interface as std::optional from C++17.
 *
 * @tparam T The type being made "optional"
 */
template <typename T>
class optional;

#if BSONCXX_HAVE_STDLIB_OPTIONAL
// If we have access to the stdlib component, re-use those so that users can use
// the same types and constants defined in <optional>
using std::bad_optional_access;
using std::in_place;
using std::in_place_t;
using std::nullopt;
using std::nullopt_t;
#else
/**
 * @brief Exception type thrown upon attempted access to a value-less optional<T>
 * via a throwing accessor API.
 */
class bad_optional_access : public std::exception {
   public:
    const char* what() const noexcept override {
        return "bad_optional_access()";
    }
};
/// Tag type to represent an empty optional value
struct nullopt_t {};
/// Tag constant to construct or compare with an empty optional value
static constexpr nullopt_t nullopt{};
/// Tag used to call the emplacement-constructor of optional<T>
static constexpr struct in_place_t {
} in_place;
#endif

namespace detail {

// Terminates the program when an illegal use of optional<T> is attempted
[[noreturn]] BSONCXX_API void BSONCXX_CALL terminate_disengaged_optional(const char* what) noexcept;
// Throws bad_optional_access for throwing optional<T> member functions
[[noreturn]] BSONCXX_API void BSONCXX_CALL throw_bad_optional();
// Base class of std::optional. Implementation detail, defined later
template <typename T>
struct optional_base_class;
// Pull bsoncxx::detail components
using namespace bsoncxx::detail;

// Base case: Things are not optionals.
template <typename T>
std::true_type not_an_optional_f(const T&);
// More-specialized if given an optional<T> or any class derived from a template
// specialization thereof.
template <typename T>
std::false_type not_an_optional_f(const optional<T>&);

// Utility trait to detect specializations of stdx::optional
template <typename T>
struct not_an_optional : decltype(not_an_optional_f(std::declval<T const&>())) {};

template <typename T, typename Ucvr, typename U>
struct enable_opt_conversion
    : conjunction<  //
          std::is_constructible<T, Ucvr>,
          disjunction<  //
              std::is_same<T, bool>,
              negation<conjunction<std::is_constructible<T, optional<U>&>,
                                   std::is_constructible<T, optional<U> const&>,
                                   std::is_constructible<T, optional<U>&&>,
                                   std::is_constructible<T, optional<U> const&&>,
                                   std::is_convertible<optional<U>&, T>,
                                   std::is_convertible<optional<U> const&, T>,
                                   std::is_convertible<optional<U>&&, T>,
                                   std::is_convertible<optional<U> const&&, T>>>>> {};

template <typename From, typename To>
struct enable_opt_value_conversion  //
    : conjunction<                  //
          std::is_constructible<To, From&&>,
          negation<is_alike<From, in_place_t>>,
          disjunction<negation<is_alike<To, bool>>,  //
                      not_an_optional<remove_cvref_t<From>>>> {};

}  // namespace detail

template <typename T>
class optional : detail::equality_operators,
                 detail::ordering_operators,
                 public detail::optional_base_class<T>::type {
   public:
    /// The type of value held within this optional
    using value_type = T;
    /// An lvalue-reference-to-mutable T
    using reference = detail::add_lvalue_reference_t<T>;
    /// An lvalue-reference-to-const T
    using const_reference = detail::add_lvalue_reference_t<detail::add_const_t<T>>;
    /// An rvalue-reference-to-mutable T
    using rvalue_reference = detail::add_rvalue_reference_t<T>;
    /// An rvalue-reference-to-const T
    using const_rvalue_reference = detail::add_rvalue_reference_t<detail::add_const_t<T>>;
    /// A pointer-to-mutable T
    using pointer = detail::add_pointer_t<T>;
    /// A pointer-to-const T
    using const_pointer = detail::add_pointer_t<const T>;

    // Constructors [1]
    optional() = default;
    constexpr optional(nullopt_t) noexcept {}

    // Ctor [2] and [3] are provided by base classes
    optional(const optional&) = default;
    optional(optional&&) = default;
    // Same with assignments
    optional& operator=(const optional&) = default;
    optional& operator=(optional&&) = default;
    ~optional() = default;

    // In-place constructor
    template <typename... Args>
    bsoncxx_cxx14_constexpr explicit optional(in_place_t, Args&&... args) noexcept(
        noexcept(T(BSONCXX_FWD(args)...))) {
        this->emplace(BSONCXX_FWD(args)...);
    }

    // Explicit converting constructor. Only available if implicit conversion is
    // not possible.
    template <typename U = T,
              detail::requires_t<int,
                                 detail::enable_opt_value_conversion<U&&, T>,
                                 detail::negation<std::is_convertible<U&&, T>>> = 0>
    bsoncxx_cxx14_constexpr explicit optional(U&& arg) noexcept(
        std::is_nothrow_constructible<T, U&&>::value)
        : optional(in_place, BSONCXX_FWD(arg)) {}

    // Implicit converting constructor. Only available if implicit conversion is
    // possible.
    template <typename U = T,
              detail::requires_t<int,
                                 detail::enable_opt_value_conversion<U&&, T>,
                                 std::is_convertible<U&&, T>> = 0>
    bsoncxx_cxx14_constexpr optional(U&& arg) noexcept(std::is_nothrow_constructible<T, U&&>::value)
        : optional(in_place, BSONCXX_FWD(arg)) {}

    template <typename U,
              detail::requires_t<int,
                                 detail::enable_opt_conversion<T, const U&, U>,
                                 detail::negation<std::is_convertible<U const&, T>>> = 0>
    bsoncxx_cxx14_constexpr explicit optional(optional<U> const& other) noexcept(
        std::is_nothrow_constructible<T, detail::add_lvalue_reference_t<const U>>::value) {
        if (other.has_value()) {
            this->emplace(*other);
        }
    }

    template <typename U,
              detail::requires_t<int,
                                 detail::enable_opt_conversion<T, const U&, U>,
                                 std::is_convertible<U const&, T>> = 0>
    bsoncxx_cxx14_constexpr optional(optional<U> const& other) noexcept(
        std::is_nothrow_constructible<T, detail::add_lvalue_reference_t<const U>>::value) {
        if (other.has_value()) {
            this->emplace(*other);
        }
    }

    template <typename U,
              detail::requires_t<int,
                                 detail::enable_opt_conversion<T, U&&, U>,
                                 detail::negation<std::is_convertible<U&&, T>>> = 0>
    bsoncxx_cxx14_constexpr explicit optional(optional<U>&& other) noexcept(
        std::is_nothrow_constructible<T, detail::add_lvalue_reference_t<U&&>>::value) {
        if (other.has_value()) {
            this->emplace(*BSONCXX_FWD(*other));
        }
    }

    template <typename U,
              detail::requires_t<int,
                                 detail::enable_opt_conversion<T, U&&, U>,
                                 std::is_convertible<U&&, T>> = 0>
    bsoncxx_cxx14_constexpr optional(optional<U>&& other) noexcept(
        std::is_nothrow_constructible<T, detail::add_lvalue_reference_t<U&&>>::value) {
        if (other.has_value()) {
            this->emplace(*BSONCXX_FWD(*other));
        }
    }

    constexpr bool has_value() const noexcept {
        return this->_has_value;
    }
    constexpr explicit operator bool() const noexcept {
        return this->has_value();
    }

    // Unchecked dereference operators
    bsoncxx_cxx14_constexpr reference operator*() & noexcept {
        _assert_has_value("operator*() &");
        return this->_storage.value;
    }
    bsoncxx_cxx14_constexpr const_reference operator*() const& noexcept {
        _assert_has_value("operator*() const&");
        return this->_storage.value;
    }
    bsoncxx_cxx14_constexpr rvalue_reference operator*() && noexcept {
        _assert_has_value("operator*() &");
        return static_cast<rvalue_reference>(**this);
    }
    bsoncxx_cxx14_constexpr const_rvalue_reference operator*() const&& noexcept {
        _assert_has_value("operator*() const&");
        return static_cast<const_rvalue_reference>(**this);
    }

    // (Unchecked) member-access operators
    bsoncxx_cxx14_constexpr pointer operator->() noexcept {
        _assert_has_value("operator->()");
        return std::addressof(**this);
    }
    bsoncxx_cxx14_constexpr const_pointer operator->() const noexcept {
        _assert_has_value("operator->() const");
        return std::addressof(**this);
    }

    // Checked accessors
    bsoncxx_cxx14_constexpr reference value() & {
        _throw_if_empty();
        return **this;
    }
    bsoncxx_cxx14_constexpr const_reference value() const& {
        _throw_if_empty();
        return **this;
    }
    bsoncxx_cxx14_constexpr rvalue_reference value() && {
        _throw_if_empty();
        return static_cast<rvalue_reference>(**this);
    }
    bsoncxx_cxx14_constexpr const_rvalue_reference value() const&& {
        _throw_if_empty();
        return static_cast<const_rvalue_reference>(**this);
    }

    // Checked value-or-alternative
    template <typename U>
    bsoncxx_cxx14_constexpr value_type value_or(U&& dflt) const& {
        if (has_value()) {
            return **this;
        } else {
            return static_cast<value_type>(BSONCXX_FWD(dflt));
        }
    }

    template <typename U>
    bsoncxx_cxx14_constexpr value_type value_or(U&& dflt) && {
        if (has_value()) {
            return *std::move(*this);
        } else {
            return static_cast<value_type>(BSONCXX_FWD(dflt));
        }
    }

   private:
    bsoncxx_cxx14_constexpr void _assert_has_value(const char* msg) const noexcept {
        if (!this->has_value()) {
            detail::terminate_disengaged_optional(msg);
        }
    }

    bsoncxx_cxx14_constexpr void _throw_if_empty() const {
        if (!this->has_value()) {
            detail::throw_bad_optional();
        }
    }
};

/**
 * @brief Construct an optional by decay-copying the given value into a new
 * optional<decay_t<T>>
 *
 * @param value The value being made into an optional
 */
template <typename T>
bsoncxx_cxx14_constexpr optional<detail::decay_t<T>> make_optional(T&& value) noexcept(
    std::is_nothrow_constructible<detail::decay_t<T>, T&&>::value) {
    return optional<detail::decay_t<T>>(BSONCXX_FWD(value));
}

/**
 * @brief Emplace-construct a new optional of the given type with the given
 * constructor arguments
 *
 * @tparam T The type to be constructed
 * @param args Constructor arguments
 */
template <typename T, typename... Args>
bsoncxx_cxx14_constexpr optional<T> make_optional(Args&&... args) noexcept(
    std::is_nothrow_constructible<T, Args&&...>::value) {
    return optional<T>(in_place, BSONCXX_FWD(args)...);
}

/**
 * @brief Emplace-construct a new optional of the given type with the given
 * arguments (accepts an init-list as the first argument)
 */
template <typename T, typename U, typename... Args>
bsoncxx_cxx14_constexpr optional<T>
make_optional(std::initializer_list<U> il, Args&&... args) noexcept(
    std::is_nothrow_constructible<T, std::initializer_list<U>, Args&&...>::value) {
    return optional<T>(in_place, il, BSONCXX_FWD(args)...);
}

namespace detail {

/**
 * @brief Union template that defines the storage for an optional's data.
 */
template <typename T>
union storage_for {
    // Placeholder member for disengaged optional
    char nothing;
    // Member that holds the actual value
    T value;

    // Default-construct activates the placeholder
    storage_for() noexcept : nothing(0) {}

    // Empty special members allow the union to be used in semiregular contexts,
    // but it is the responsibility of the using class to implement them properly
    ~storage_for() {}
    storage_for(const storage_for&) noexcept {}
    storage_for& operator=(const storage_for&) noexcept {
        return *this;
    }
};

// Whether a type is copyable, moveable, or immobile
enum copymove_classification {
    copyable,
    movable,
    immobile,
};

/// Classify the constructibility of the given type
template <typename T,
          bool CanCopy = std::is_copy_constructible<T>::value,
          bool CanMove = std::is_move_constructible<T>::value>
constexpr copymove_classification classify_construct() {
    return CanCopy ? copyable : CanMove ? movable : immobile;
}

/// Classify the assignability of the given type
template <typename T,
          bool CanCopy = std::is_copy_assignable<T>::value,
          bool CanMove = std::is_move_assignable<T>::value>
constexpr copymove_classification classify_assignment() {
    return CanCopy ? copyable : CanMove ? movable : immobile;
}

/**
 * @brief Common base class for optional storage implementation
 *
 * @tparam T
 */
template <typename T>
class optional_common_base;

/// Define the special member constructors for optional<T>
template <typename T, copymove_classification = classify_construct<T>()>
struct optional_construct_base;

/// Define the special member assignment operators for optional<T>
template <typename T, copymove_classification = classify_assignment<T>()>
struct optional_assign_base;

template <typename T>
struct optional_assign_base<T, copyable> : optional_construct_base<T> {};

template <typename T>
struct optional_assign_base<T, movable> : optional_construct_base<T> {
    // Constructors defer to base
    optional_assign_base() = default;
    optional_assign_base(optional_assign_base const&) = default;
    optional_assign_base(optional_assign_base&&) = default;
    ~optional_assign_base() = default;

    // No copy
    bsoncxx_cxx14_constexpr optional_assign_base& operator=(const optional_assign_base&) = delete;
    // Allow move-assign:
    bsoncxx_cxx14_constexpr optional_assign_base& operator=(optional_assign_base&& other) = default;
};

template <typename T>
struct optional_assign_base<T, immobile> : optional_construct_base<T> {
    optional_assign_base() = default;
    optional_assign_base(optional_assign_base const&) = default;
    optional_assign_base(optional_assign_base&&) = default;
    ~optional_assign_base() = default;

    // No assignment at all
    optional_assign_base& operator=(const optional_assign_base&) = delete;
    optional_assign_base& operator=(optional_assign_base&&) = delete;
};

template <typename T>
struct optional_construct_base<T, copyable> : optional_common_base<T> {};

template <typename T>
struct optional_construct_base<T, movable> : optional_common_base<T> {
    optional_construct_base() = default;

    optional_construct_base(const optional_construct_base&) = delete;
    optional_construct_base(optional_construct_base&& other) = default;
    optional_construct_base& operator=(const optional_construct_base&) = default;
    optional_construct_base& operator=(optional_construct_base&&) = default;
};

template <typename T>
struct optional_construct_base<T, immobile> : optional_common_base<T> {
    optional_construct_base() = default;
    optional_construct_base(const optional_construct_base&) = delete;
    optional_construct_base& operator=(const optional_construct_base&) = default;
    optional_construct_base& operator=(optional_construct_base&&) = default;
};

// Optional's ADL-only operators live here:
struct operators_base {
    template <typename T, typename U>
    friend bsoncxx_cxx14_constexpr auto tag_invoke(equal_to,
                                                   optional<T> const& left,
                                                   optional<U> const& right) noexcept
        -> requires_t<bool, is_equality_comparable<T, U>> {
        if (left.has_value() != right.has_value()) {
            return false;
        }
        return !left.has_value() || *left == *right;
    }

    template <typename T, typename U>
    friend constexpr auto tag_invoke(equal_to, optional<T> const& left, U const& right) noexcept
        -> requires_t<bool, not_an_optional<U>, is_equality_comparable<T, U>> {
        return left.has_value() && *left == right;
    }

    template <typename T>
    friend constexpr bool tag_invoke(equal_to, optional<T> const& opt, nullopt_t) noexcept {
        return !opt.has_value();
    }

    template <typename T, typename U>
    bsoncxx_cxx14_constexpr friend auto tag_invoke(compare_three_way compare,
                                                   optional<T> const& left,
                                                   optional<U> const& right)
        -> requires_t<strong_ordering, is_totally_ordered_with<T, U>> {
        if (left.has_value()) {
            if (right.has_value()) {
                return compare(*left, *right);
            } else {
                // non-null is greater than any null
                return strong_ordering::greater;
            }
        } else {
            if (right.has_value()) {
                // Null is less than any non-null
                return strong_ordering::less;
            } else {
                // Both are null
                return strong_ordering::equal;
            }
        }
    }

    template <typename T, typename U>
    bsoncxx_cxx14_constexpr friend auto tag_invoke(compare_three_way compare,
                                                   optional<T> const& left,
                                                   U const& right)
        -> requires_t<strong_ordering, not_an_optional<U>, is_totally_ordered_with<T, U>> {
        if (left.has_value()) {
            return compare(*left, right);
        }
        // null optional is less-than any non-null value
        return strong_ordering::less;
    }

    template <typename T>
    constexpr friend strong_ordering tag_invoke(compare_three_way,
                                                optional<T> const& left,
                                                nullopt_t) {
        return left.has_value() ? strong_ordering::greater : strong_ordering::equal;
    }
};

// Common base class of all optionals
template <typename T>
class optional_common_base : operators_base {
    using storage_type = detail::storage_for<T>;

   public:
    optional_common_base() = default;
    ~optional_common_base() {
        this->reset();
    }

    optional_common_base(const optional_common_base& other) noexcept(
        std::is_nothrow_copy_constructible<T>::value) {
        if (other._has_value) {
            this->emplace(other._storage.value);
        }
    }

    optional_common_base(optional_common_base&& other) noexcept(
        std::is_nothrow_move_constructible<T>::value) {
        if (other._has_value) {
            this->_emplace_construct_anew(std::move(other)._storage.value);
        }
    }

    optional_common_base& operator=(const optional_common_base& other) noexcept(
        std::is_nothrow_copy_assignable<T>::value) {
        this->_assign(BSONCXX_FWD(other));
        return *this;
    }

    optional_common_base& operator=(optional_common_base&& other) noexcept(
        std::is_nothrow_move_assignable<T>::value) {
        this->_assign(BSONCXX_FWD(other));
        return *this;
    }

    /**
     * @internal
     * @brief If the optional is holding a value, destroy that value and set ourselves null
     */
    void reset() noexcept {
        if (this->_has_value) {
            this->_storage.value.~T();
        }
        this->_has_value = false;
    }

    /**
     * @internal
     * @brief If the optional is holding a value, destroy that value. Construct
     * a new value in-place using the given arguments.
     */
    template <typename... Args>
    void emplace(Args&&... args) noexcept(noexcept((T(BSONCXX_FWD(args)...)))) {
        this->reset();
        this->_emplace_construct_anew(BSONCXX_FWD(args)...);
    }

   private:
    friend optional<T>;
    storage_type _storage;
    bool _has_value = false;

    /**
     * @internal
     * @brief In-place construct a new value from the given arguments. Assumes
     * that the optional does not have a live value.
     */
    template <typename... Args>
    void _emplace_construct_anew(Args&&... args) noexcept(
        std::is_nothrow_constructible<T, Args&&...>::value) {
        new (std::addressof(this->_storage.value)) T(static_cast<Args&&>(args)...);
        this->_has_value = true;
    }

    /**
     * @internal
     * @brief Perform the semantics of the assignment operator.
     */
    template <typename U>
    void _assign(U&& other_storage) {
        if (other_storage._has_value) {
            // We are receiving a value
            if (this->_has_value) {
                // We already have a value. Invoke the underlying assignment.
                this->_storage.value = std::forward<U>(other_storage)._storage.value;
            } else {
                // We don't have a value. Use the constructor.
                this->_emplace_construct_anew(std::forward<U>(other_storage)._storage.value);
            }
        } else {
            // We are receiving nullopt. Destroy our value, if present:
            this->reset();
        }
    }
};

template <typename T>
struct optional_base_class {
    using type = optional_assign_base<T>;
};

}  // namespace detail

}  // namespace stdx

}  // namespace v_noabi

}  // namespace bsoncxx

#undef BSONCXX_HAVE_STDLIB_OPTIONAL

#include <bsoncxx/config/postlude.hpp>

namespace bsoncxx {
namespace stdx {

using ::bsoncxx::v_noabi::stdx::in_place;
using ::bsoncxx::v_noabi::stdx::in_place_t;
using ::bsoncxx::v_noabi::stdx::make_optional;
using ::bsoncxx::v_noabi::stdx::nullopt;
using ::bsoncxx::v_noabi::stdx::nullopt_t;
using ::bsoncxx::v_noabi::stdx::optional;

}  // namespace stdx
}  // namespace bsoncxx
