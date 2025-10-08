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

#include <bsoncxx/v1/detail/prelude.hpp>

#include <bsoncxx/v1/config/config.hpp>

#if defined(BSONCXX_POLY_USE_STD)

#include <optional>

namespace bsoncxx {
namespace v1 {
namespace stdx {

using std::in_place;
using std::in_place_t;
using std::make_optional;
using std::nullopt;
using std::nullopt_t;
using std::optional;

} // namespace stdx
} // namespace v1
} // namespace bsoncxx

#elif defined(BSONCXX_POLY_USE_IMPLS)

#include <bsoncxx/v1/detail/compare.hpp>
#include <bsoncxx/v1/detail/macros.hpp>
#include <bsoncxx/v1/detail/type_traits.hpp>

#include <cstddef>
#include <cstdio>
#include <exception>
#include <initializer_list>
#include <memory>
#include <new>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace bsoncxx {

namespace v1 {

namespace stdx {

template <typename T>
class optional;

BSONCXX_PRIVATE_WARNINGS_PUSH();
BSONCXX_PRIVATE_WARNINGS_DISABLE(Clang("-Wweak-vtables"));
// Exception type thrown upon attempted access to a value-less optional<T> via a throwing accessor
// API.
class bad_optional_access : public std::exception {
   public:
    char const* what() const noexcept override {
        return "bad_optional_access()";
    }
};
BSONCXX_PRIVATE_WARNINGS_POP();

// Tag type to represent an empty optional value.
struct nullopt_t {
    explicit constexpr nullopt_t(std::nullptr_t) noexcept {}
};

// Tag constant to construct or compare with an empty optional value.
static constexpr nullopt_t nullopt{nullptr};

// Tag used to call the emplacement-constructor of optional<T>.
static constexpr struct in_place_t {
} in_place;

namespace detail {

// Terminates the program when an illegal use of optional<T> is attempted.
[[noreturn]] inline void terminate_disengaged_optional(char const* what) noexcept {
    (void)std::fprintf(stderr, "%s: Invalid attempted use of disengaged optional<T>\n", what);
    std::terminate();
}

// Throws bad_optional_access for throwing optional<T> member functions.
[[noreturn]] inline void throw_bad_optional() {
    throw bad_optional_access();
}

// Base class of std::optional. Implementation detail, defined later.
template <typename T>
struct optional_base_class;

// Base case: Things are not optionals.
template <typename T>
std::true_type not_an_optional_f(T const&);
// More-specialized if given an optional<T> or any class derived from a template
// specialization thereof.
template <typename T>
std::false_type not_an_optional_f(optional<T> const&);

// Utility trait to detect specializations of stdx::optional.
template <typename T>
struct not_an_optional : decltype(not_an_optional_f(std::declval<T const&>())){};

template <typename T, typename Ucvr, typename U>
struct enable_opt_conversion : bsoncxx::detail::conjunction<
                                   std::is_constructible<T, Ucvr>,
                                   bsoncxx::detail::disjunction<
                                       std::is_same<T, bool>,
                                       bsoncxx::detail::negation<bsoncxx::detail::conjunction<
                                           std::is_constructible<T, optional<U>&>,
                                           std::is_constructible<T, optional<U> const&>,
                                           std::is_constructible<T, optional<U>&&>,
                                           std::is_constructible<T, optional<U> const&&>,
                                           std::is_convertible<optional<U>&, T>,
                                           std::is_convertible<optional<U> const&, T>,
                                           std::is_convertible<optional<U>&&, T>,
                                           std::is_convertible<optional<U> const&&, T>>>>> {};

template <typename From, typename To>
struct enable_opt_value_conversion : bsoncxx::detail::conjunction<
                                         std::is_constructible<To, From&&>,
                                         bsoncxx::detail::negation<bsoncxx::detail::is_alike<From, in_place_t>>,
                                         bsoncxx::detail::negation<bsoncxx::detail::is_alike<From, optional<To>>>,
                                         bsoncxx::detail::disjunction<
                                             bsoncxx::detail::negation<bsoncxx::detail::is_alike<To, bool>>,
                                             detail::not_an_optional<bsoncxx::detail::remove_cvref_t<From>>>> {};

} // namespace detail

template <typename T>
class optional : bsoncxx::detail::equality_operators,
                 bsoncxx::detail::ordering_operators,
                 public detail::optional_base_class<T>::type {
   public:
    using value_type = T;
    using reference = bsoncxx::detail::add_lvalue_reference_t<T>;
    using const_reference = bsoncxx::detail::add_lvalue_reference_t<bsoncxx::detail::add_const_t<T>>;
    using rvalue_reference = bsoncxx::detail::add_rvalue_reference_t<T>;
    using const_rvalue_reference = bsoncxx::detail::add_rvalue_reference_t<bsoncxx::detail::add_const_t<T>>;
    using pointer = bsoncxx::detail::add_pointer_t<T>;
    using const_pointer = bsoncxx::detail::add_pointer_t<T const>;

    // Constructors [1].

    optional() = default;
    constexpr optional(nullopt_t) noexcept {}

    // Ctor [2] and [3] are provided by base classes.

    optional(optional const&) = default;
    optional(optional&&) = default;

    // Same with assignments

    optional& operator=(optional const&) = default;
    optional& operator=(optional&&) = default;
    ~optional() = default;

    // In-place constructors

    template <typename... Args>
    BSONCXX_PRIVATE_CONSTEXPR_CXX14 explicit optional(in_place_t, Args&&... args) noexcept(
        noexcept(T(BSONCXX_PRIVATE_FWD(args)...))) {
        this->emplace(BSONCXX_PRIVATE_FWD(args)...);
    }

    template <typename U, typename... Args>
    BSONCXX_PRIVATE_CONSTEXPR_CXX14 explicit optional(in_place_t, std::initializer_list<U> il, Args&&... args) noexcept(
        noexcept(T(il, BSONCXX_PRIVATE_FWD(args)...))) {
        this->emplace(il, BSONCXX_PRIVATE_FWD(args)...);
    }

    // Explicit converting constructor. Only available if implicit conversion is
    // not possible.
    template <
        typename U = T,
        bsoncxx::detail::requires_t<
            int,
            detail::enable_opt_value_conversion<U&&, T>,
            bsoncxx::detail::negation<std::is_convertible<U&&, T>>> = 0>
    BSONCXX_PRIVATE_CONSTEXPR_CXX14 explicit optional(U&& arg) noexcept(std::is_nothrow_constructible<T, U&&>::value)
        : optional(in_place, BSONCXX_PRIVATE_FWD(arg)) {}

    // Implicit converting constructor. Only available if implicit conversion is
    // possible.
    template <
        typename U = T,
        bsoncxx::detail::requires_t<int, detail::enable_opt_value_conversion<U&&, T>, std::is_convertible<U&&, T>> = 0>
    BSONCXX_PRIVATE_CONSTEXPR_CXX14 optional(U&& arg) noexcept(std::is_nothrow_constructible<T, U&&>::value)
        : optional(in_place, BSONCXX_PRIVATE_FWD(arg)) {}

    template <
        typename U,
        bsoncxx::detail::requires_t<
            int,
            detail::enable_opt_conversion<T, U const&, U>,
            bsoncxx::detail::negation<std::is_convertible<U const&, T>>> = 0>
    BSONCXX_PRIVATE_CONSTEXPR_CXX14 explicit optional(optional<U> const& other) noexcept(
        std::is_nothrow_constructible<T, bsoncxx::detail::add_lvalue_reference_t<U const>>::value) {
        if (other.has_value()) {
            this->emplace(*other);
        }
    }

    template <
        typename U,
        bsoncxx::detail::requires_t<
            int,
            detail::enable_opt_conversion<T, U const&, U>,
            std::is_convertible<U const&, T>> = 0>
    BSONCXX_PRIVATE_CONSTEXPR_CXX14 optional(optional<U> const& other) noexcept(
        std::is_nothrow_constructible<T, bsoncxx::detail::add_lvalue_reference_t<U const>>::value) {
        if (other.has_value()) {
            this->emplace(*other);
        }
    }

    template <
        typename U,
        bsoncxx::detail::requires_t<
            int,
            detail::enable_opt_conversion<T, U&&, U>,
            bsoncxx::detail::negation<std::is_convertible<U&&, T>>> = 0>
    BSONCXX_PRIVATE_CONSTEXPR_CXX14 explicit optional(optional<U>&& other) noexcept(
        std::is_nothrow_constructible<T, bsoncxx::detail::add_lvalue_reference_t<U&&>>::value) {
        if (other.has_value()) {
            this->emplace(*BSONCXX_PRIVATE_FWD(other));
        }
    }

    template <
        typename U,
        bsoncxx::detail::requires_t<int, detail::enable_opt_conversion<T, U&&, U>, std::is_convertible<U&&, T>> = 0>
    BSONCXX_PRIVATE_CONSTEXPR_CXX14 optional(optional<U>&& other) noexcept(
        std::is_nothrow_constructible<T, bsoncxx::detail::add_lvalue_reference_t<U&&>>::value) {
        if (other.has_value()) {
            this->emplace(*BSONCXX_PRIVATE_FWD(other));
        }
    }

    constexpr bool has_value() const noexcept {
        return this->_has_value;
    }
    constexpr explicit operator bool() const noexcept {
        return this->has_value();
    }

    // Unchecked dereference operators.

    BSONCXX_PRIVATE_CONSTEXPR_CXX14 reference operator*() & noexcept {
        _assert_has_value("operator*() &");
        return this->_storage.value;
    }
    BSONCXX_PRIVATE_CONSTEXPR_CXX14 const_reference operator*() const& noexcept {
        _assert_has_value("operator*() const&");
        return this->_storage.value;
    }
    BSONCXX_PRIVATE_CONSTEXPR_CXX14 rvalue_reference operator*() && noexcept {
        _assert_has_value("operator*() &&");
        return static_cast<rvalue_reference>(**this);
    }
    BSONCXX_PRIVATE_CONSTEXPR_CXX14 const_rvalue_reference operator*() const&& noexcept {
        _assert_has_value("operator*() const&&");
        return static_cast<const_rvalue_reference>(**this);
    }

    // (Unchecked) member-access operators.

    BSONCXX_PRIVATE_CONSTEXPR_CXX14 pointer operator->() noexcept {
        _assert_has_value("operator->()");
        return std::addressof(**this);
    }
    BSONCXX_PRIVATE_CONSTEXPR_CXX14 const_pointer operator->() const noexcept {
        _assert_has_value("operator->() const");
        return std::addressof(**this);
    }

    // Checked accessors.

    BSONCXX_PRIVATE_CONSTEXPR_CXX14 reference value() & {
        _throw_if_empty();
        return **this;
    }
    BSONCXX_PRIVATE_CONSTEXPR_CXX14 const_reference value() const& {
        _throw_if_empty();
        return **this;
    }
    BSONCXX_PRIVATE_CONSTEXPR_CXX14 rvalue_reference value() && {
        _throw_if_empty();
        return static_cast<rvalue_reference>(**this);
    }
    BSONCXX_PRIVATE_CONSTEXPR_CXX14 const_rvalue_reference value() const&& {
        _throw_if_empty();
        return static_cast<const_rvalue_reference>(**this);
    }

    // Checked value-or-alternative.

    template <typename U>
    BSONCXX_PRIVATE_CONSTEXPR_CXX14 value_type value_or(U&& dflt) const& {
        if (has_value()) {
            return **this;
        } else {
            return static_cast<value_type>(BSONCXX_PRIVATE_FWD(dflt));
        }
    }

    template <typename U>
    BSONCXX_PRIVATE_CONSTEXPR_CXX14 value_type value_or(U&& dflt) && {
        if (has_value()) {
            return *std::move(*this);
        } else {
            return static_cast<value_type>(BSONCXX_PRIVATE_FWD(dflt));
        }
    }

   private:
    BSONCXX_PRIVATE_CONSTEXPR_CXX14 void _assert_has_value(char const* msg) const noexcept {
        if (!this->has_value()) {
            detail::terminate_disengaged_optional(msg);
        }
    }

    BSONCXX_PRIVATE_CONSTEXPR_CXX14 void _throw_if_empty() const {
        if (!this->has_value()) {
            detail::throw_bad_optional();
        }
    }
};

// Construct an optional by decay-copying the given value into a new optional<decay_t<T>>.
//
// @param value The value being made into an optional.
template <typename T>
BSONCXX_PRIVATE_CONSTEXPR_CXX14 optional<bsoncxx::detail::decay_t<T>> make_optional(T&& value) noexcept(
    std::is_nothrow_constructible<bsoncxx::detail::decay_t<T>, T&&>::value) {
    return optional<bsoncxx::detail::decay_t<T>>(BSONCXX_PRIVATE_FWD(value));
}

// Emplace-construct a new optional of the given type with the given constructor arguments.
//
// @tparam T The type to be constructed
// @param args Constructor arguments
template <typename T, typename... Args>
BSONCXX_PRIVATE_CONSTEXPR_CXX14 optional<T> make_optional(Args&&... args) noexcept(
    std::is_nothrow_constructible<T, Args&&...>::value) {
    return optional<T>(in_place, BSONCXX_PRIVATE_FWD(args)...);
}

// Emplace-construct a new optional of the given type with the given arguments (accepts an init-list
// as the first argument).
template <typename T, typename U, typename... Args>
BSONCXX_PRIVATE_CONSTEXPR_CXX14 optional<T> make_optional(std::initializer_list<U> il, Args&&... args) noexcept(
    std::is_nothrow_constructible<T, std::initializer_list<U>, Args&&...>::value) {
    return optional<T>(in_place, il, BSONCXX_PRIVATE_FWD(args)...);
}

namespace detail {

// Union template that defines the storage for an optional's data.
template <typename T, bool = std::is_trivially_destructible<T>::value>
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
    storage_for(storage_for const&) = delete;
    storage_for& operator=(storage_for const&) = delete;
};

template <typename T>
union storage_for<T, true /* Is trivially destructible */> {
    char nothing;
    T value;
    storage_for() noexcept : nothing(0) {}
    storage_for(storage_for const&) = delete;
    storage_for& operator=(storage_for const&) = delete;
};

// Whether a type is copyable, moveable, or immobile.
enum copymove_classification {
    copyable,
    movable,
    immobile,
};

// Classify the constructibility of the given type.
template <
    typename T,
    bool CanCopy = std::is_copy_constructible<T>::value,
    bool CanMove = std::is_move_constructible<T>::value>
constexpr copymove_classification classify_construct() {
    return CanCopy ? copyable : CanMove ? movable : immobile;
}

// Classify the assignability of the given type.
template <
    typename T,
    bool CanCopy = std::is_copy_assignable<T>::value,
    bool CanMove = std::is_move_assignable<T>::value>
constexpr copymove_classification classify_assignment() {
    return CanCopy ? copyable : CanMove ? movable : immobile;
}

// Common base class for optional storage implementation
//
// @tparam T
template <typename T>
class optional_common_base;

// Define the special member constructors for optional<T>.
template <typename T, copymove_classification = classify_construct<T>()>
struct optional_construct_base;

// Define the special member assignment operators for optional<T>.
template <typename T, copymove_classification = classify_assignment<T>()>
struct optional_assign_base;

template <bool TrivialDestruct>
struct optional_destruct_helper;

template <typename T>
using optional_destruct_base =
    typename optional_destruct_helper<std::is_trivially_destructible<T>::value>::template base<T>;

template <typename T>
struct optional_assign_base<T, copyable> : optional_construct_base<T> {};

template <typename T>
struct optional_assign_base<T, movable> : optional_construct_base<T> {
    // Constructors defer to base.

    optional_assign_base() = default;
    optional_assign_base(optional_assign_base const&) = default;
    optional_assign_base(optional_assign_base&&) = default;
    ~optional_assign_base() = default;

    // Disallow copies.

    BSONCXX_PRIVATE_CONSTEXPR_CXX14 optional_assign_base& operator=(optional_assign_base const&) = delete;

    // Allow move-assignment.

    BSONCXX_PRIVATE_CONSTEXPR_CXX14 optional_assign_base& operator=(optional_assign_base&&) = default;
};

template <typename T>
struct optional_assign_base<T, immobile> : optional_construct_base<T> {
    optional_assign_base() = default;
    optional_assign_base(optional_assign_base const&) = default;
    optional_assign_base(optional_assign_base&&) = default;
    ~optional_assign_base() = default;

    // No assignment at all
    optional_assign_base& operator=(optional_assign_base const&) = delete;
    optional_assign_base& operator=(optional_assign_base&&) = delete;
};

template <typename T>
struct optional_construct_base<T, copyable> : optional_destruct_base<T> {};

template <typename T>
struct optional_construct_base<T, movable> : optional_destruct_base<T> {
    optional_construct_base() = default;

    optional_construct_base(optional_construct_base const&) = delete;
    optional_construct_base(optional_construct_base&&) = default;
    optional_construct_base& operator=(optional_construct_base const&) = default;
    optional_construct_base& operator=(optional_construct_base&&) = default;
};

template <typename T>
struct optional_construct_base<T, immobile> : optional_destruct_base<T> {
    optional_construct_base() = default;
    optional_construct_base(optional_construct_base const&) = delete;
    optional_construct_base& operator=(optional_construct_base const&) = default;
    optional_construct_base& operator=(optional_construct_base&&) = default;
};

template <>
struct optional_destruct_helper<false /* Non-trivial */> {
    template <typename T>
    struct base : optional_common_base<T> {
        // Special members defer to base.

        base() = default;
        base(base const&) = default;
        base(base&&) = default;
        base& operator=(base const&) = default;
        base& operator=(base&&) = default;

        ~base() {
            // Here we destroy the contained object during destruction.
            this->reset();
        }
    };
};

template <>
struct optional_destruct_helper<true /* Trivial */> {
    // Just fall-through to the common base, which has no special destructor.

    template <typename T>
    using base = optional_common_base<T>;
};

// Optional's ADL-only operators are defined here.
struct optional_operators_base {
    template <typename T, typename U>
    friend BSONCXX_PRIVATE_CONSTEXPR_CXX14 auto
    tag_invoke(bsoncxx::detail::equal_to, optional<T> const& left, optional<U> const& right) noexcept
        -> bsoncxx::detail::requires_t<bool, bsoncxx::detail::is_equality_comparable<T, U>> {
        if (left.has_value() != right.has_value()) {
            return false;
        }

        BSONCXX_PRIVATE_WARNINGS_PUSH();
        BSONCXX_PRIVATE_WARNINGS_DISABLE(GNU("-Wfloat-equal"));
        return !left.has_value() || *left == *right;
        BSONCXX_PRIVATE_WARNINGS_POP();
    }

    template <typename T, typename U>
    friend constexpr auto tag_invoke(bsoncxx::detail::equal_to, optional<T> const& left, U const& right) noexcept
        -> bsoncxx::detail::requires_t<bool, not_an_optional<U>, bsoncxx::detail::is_equality_comparable<T, U>> {
        BSONCXX_PRIVATE_WARNINGS_PUSH();
        BSONCXX_PRIVATE_WARNINGS_DISABLE(GNU("-Wfloat-equal"));
        return left.has_value() && *left == right;
        BSONCXX_PRIVATE_WARNINGS_POP();
    }

    template <typename T>
    friend constexpr bool tag_invoke(bsoncxx::detail::equal_to, optional<T> const& opt, nullopt_t) noexcept {
        return !opt.has_value();
    }

    template <typename T, typename U>
    BSONCXX_PRIVATE_CONSTEXPR_CXX14 friend auto
    tag_invoke(bsoncxx::detail::compare_three_way compare, optional<T> const& left, optional<U> const& right)
        -> bsoncxx::detail::requires_t<
            bsoncxx::detail::strong_ordering,
            bsoncxx::detail::is_totally_ordered_with<T, U>> {
        if (left.has_value()) {
            if (right.has_value()) {
                return compare(*left, *right);
            } else {
                // Non-null is greater than any null.
                return bsoncxx::detail::strong_ordering::greater;
            }
        } else {
            if (right.has_value()) {
                // Null is less than any non-null.
                return bsoncxx::detail::strong_ordering::less;
            } else {
                // Both are null.
                return bsoncxx::detail::strong_ordering::equal;
            }
        }
    }

    template <typename T, typename U>
    BSONCXX_PRIVATE_CONSTEXPR_CXX14 friend auto
    tag_invoke(bsoncxx::detail::compare_three_way compare, optional<T> const& left, U const& right)
        -> bsoncxx::detail::requires_t<
            bsoncxx::detail::strong_ordering,
            not_an_optional<U>,
            bsoncxx::detail::is_totally_ordered_with<T, U>> {
        if (left.has_value()) {
            return compare(*left, right);
        }
        // Null optional is less-than any non-null value.
        return bsoncxx::detail::strong_ordering::less;
    }

    template <typename T>
    constexpr friend bsoncxx::detail::strong_ordering
    tag_invoke(bsoncxx::detail::compare_three_way compare, optional<T> const& left, nullopt_t) {
        return compare(left.has_value(), false);
    }
};

// An ADL-visible swap() should only be available for swappable objects
template <typename T, bool IsSwappable = bsoncxx::detail::is_swappable<T>::value>
struct optional_swap_mixin {};

template <typename T>
struct optional_swap_mixin<T, true> {
    BSONCXX_PRIVATE_CONSTEXPR_CXX14 friend void swap(optional<T>& left, optional<T>& right) noexcept(
        std::is_nothrow_move_constructible<T>::value && bsoncxx::detail::is_nothrow_swappable<T>::value) {
        left.swap(right);
    }
};

// Common base class of all optionals.
template <typename T>
class optional_common_base : optional_operators_base, optional_swap_mixin<T> {
    using storage_type = detail::storage_for<bsoncxx::detail::remove_const_t<T>>;

   public:
    optional_common_base() = default;
    ~optional_common_base() = default;

    optional_common_base(optional_common_base const& other) noexcept(std::is_nothrow_copy_constructible<T>::value) {
        if (other._has_value) {
            this->emplace(other._storage.value);
        }
    }

    optional_common_base(optional_common_base&& other) noexcept(std::is_nothrow_move_constructible<T>::value) {
        if (other._has_value) {
            this->_emplace_construct_anew(std::move(other)._storage.value);
        }
    }

    optional_common_base& operator=(optional_common_base const& other) noexcept(
        std::is_nothrow_copy_assignable<T>::value) {
        this->_assign(BSONCXX_PRIVATE_FWD(other));
        return *this;
    }

    optional_common_base& operator=(optional_common_base&& other) noexcept(std::is_nothrow_move_assignable<T>::value) {
        this->_assign(BSONCXX_PRIVATE_FWD(other));
        return *this;
    }

    // If the optional is holding a value, destroy that value and set ourselves null.
    void reset() noexcept {
        if (this->_has_value) {
            this->_storage.value.~T();
        }
        this->_has_value = false;
    }

    // If the optional is holding a value, destroy that value. Construct a new value in-place using
    // the given arguments.
    template <typename... Args>
    T& emplace(Args&&... args) {
        this->reset();
        this->_emplace_construct_anew(BSONCXX_PRIVATE_FWD(args)...);
        return this->_storage.value;
    }

    // If the optional is holding a value, destroy that value. Construct a new value in-place using
    // the given arguments.
    template <typename U, typename... Args>
    T& emplace(std::initializer_list<U> il, Args&&... args) {
        this->reset();
        this->_emplace_construct_anew(il, BSONCXX_PRIVATE_FWD(args)...);
        return this->_storage.value;
    }

    // Special swap for optional values that removes need for a temporary.
    BSONCXX_PRIVATE_CONSTEXPR_CXX14 void swap(optional_common_base& other) noexcept(
        std::is_nothrow_move_constructible<T>::value && bsoncxx::detail::is_nothrow_swappable<T>::value) {
        if (other._has_value) {
            if (this->_has_value) {
                using std::swap;
                // Defer to the underlying swap.
                swap(this->_storage.value, other._storage.value);
            } else {
                // "steal" the other's value.
                this->emplace(std::move(other._storage.value));
                other.reset();
            }
        } else if (this->_has_value) {
            other.emplace(std::move(this->_storage.value));
            this->reset();
        } else {
            // Neither optional has a value, so do nothing.
        }
    }

   private:
    friend optional<T>;
    storage_type _storage;
    bool _has_value = false;

    // In-place construct a new value from the given arguments. Assumes that the optional does not
    // have a live value.
    template <typename... Args>
    void _emplace_construct_anew(Args&&... args) noexcept(std::is_nothrow_constructible<T, Args&&...>::value) {
        new (std::addressof(this->_storage.value)) T(BSONCXX_PRIVATE_FWD(args)...);
        this->_has_value = true;
    }

    // Perform the semantics of the assignment operator.
    template <typename U>
    void _assign(U&& other_storage) {
        if (other_storage._has_value) {
            // We are receiving a value.
            if (this->_has_value) {
                // We already have a value. Invoke the underlying assignment.
                this->_storage.value = BSONCXX_PRIVATE_FWD(other_storage)._storage.value;
            } else {
                // We don't have a value. Use the constructor.
                this->_emplace_construct_anew(BSONCXX_PRIVATE_FWD(other_storage)._storage.value);
            }
        } else {
            // We are receiving nullopt. Destroy our value, if present.
            this->reset();
        }
    }
};

template <typename T>
struct optional_base_class {
    using type = optional_assign_base<T>;
};

template <
    typename T,
    bool CanHash = std::is_default_constructible<std::hash<bsoncxx::detail::remove_const_t<T>>>::value>
struct optional_hash;

// Hash is "disabled" if the underlying type is not hashable (disabled = cannot construct the hash
// invocable).
template <typename T>
struct optional_hash<T, false> {
    optional_hash() = delete;
    optional_hash(optional_hash const&) = delete;
};

template <typename T>
struct optional_hash<T, true> {
    using Td = bsoncxx::detail::remove_const_t<T>;
    constexpr std::size_t operator()(optional<T> const& opt) const
        noexcept(noexcept(std::hash<Td>()(std::declval<Td const&>()))) {
        return opt.has_value() ? std::hash<Td>()(*opt) : std::hash<void*>()(nullptr);
    }
};

} // namespace detail

} // namespace stdx

} // namespace v1

} // namespace bsoncxx

namespace std {

template <typename T>
struct hash<bsoncxx::v1::stdx::optional<T>> : bsoncxx::v1::stdx::detail::optional_hash<T> {};

} // namespace std

#else
#error "Cannot find a valid polyfill for optional"
#endif

#include <bsoncxx/v1/detail/postlude.hpp>

///
/// @file
/// Provides `std::optional`-related polyfills for library API usage.
///
/// @note The API and ABI compatibility of this polyfill is determined by polyfill build
/// configuration variables and the `BSONCXX_POLY_USE_*` macros provided by @ref
/// bsoncxx/v1/config/config.hpp.
///
/// @see
/// - [Choosing a C++17 Polyfill](https://www.mongodb.com/docs/languages/cpp/cpp-driver/current/polyfill-selection/)
///

#if defined(BSONCXX_PRIVATE_DOXYGEN_PREPROCESSOR)

namespace bsoncxx {
namespace v1 {
namespace stdx {

///
/// A polyfill for `std::optional<T>`.
///
/// @note The API and ABI compatibility of this polyfill is determined by polyfill build
/// configuration variables and the `BSONCXX_POLY_USE_*` macros provided by @ref
/// bsoncxx/v1/config/config.hpp.
///
template <typename T>
class optional {};

} // namespace stdx
} // namespace v1
} // namespace bsoncxx

#endif // defined(BSONCXX_PRIVATE_DOXYGEN_PREPROCESSOR)
