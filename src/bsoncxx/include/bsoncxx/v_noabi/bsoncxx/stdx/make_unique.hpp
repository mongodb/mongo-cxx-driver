// Copyright 2014 MongoDB Inc.
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

#include <cstddef>
#include <memory>
#include <type_traits>
#include <utility>

#include <bsoncxx/config/prelude.hpp>

namespace bsoncxx {
inline namespace v_noabi {
namespace stdx {

namespace detail {

// Switch backend of make_unique by the type we are creating.
// It would be easier to 'if constexpr' on whether we are an array and whether to direct-init or
// value-init, but we don't have if-constexpr and we need it to guard against an uterance of a
// possibly-illegal 'new' expression.
template <typename T>
struct make_unique_impl {
    // For make_unique:
    template <typename... Args,
              // Guard on constructible-from:
              typename = decltype(new T(std::declval<Args>()...))>
    static std::unique_ptr<T> make(std::true_type /* direct-init */, Args&&... args) {
        return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
    }

    // For make_unique_for_overwrite:
    template <typename U = T,
              // Guard on whether T is value-initializable:
              // (Hide behind a deduced 'U' to defer the evaluation of
              // this default template argument until overload resolution)
              typename = decltype(new U)>
    static std::unique_ptr<T> make(std::false_type /* value-init */) {
        return std::unique_ptr<T>(new T);
    }
};

// For unbounded arrays:
template <typename Elem>
struct make_unique_impl<Elem[]> {
    template <typename ShouldDirectInit,
              // Guard on whether the new-expression will be legal:
              typename = decltype(new Elem[std::declval<std::size_t>()])>
    static std::unique_ptr<Elem[]> make(ShouldDirectInit, std::size_t count) {
        // These can share a function via a plain if, because both new expressions
        // must be semantically valid
        if (ShouldDirectInit()) {
            return std::unique_ptr<Elem[]>(new Elem[count]());
        } else {
            return std::unique_ptr<Elem[]>(new Elem[count]);
        }
    }
};

// Bounded arrays are disallowed:
template <typename Elem, std::size_t N>
struct make_unique_impl<Elem[N]> {};

// References are nonsense:
template <typename T>
struct make_unique_impl<T&> {};

// References are nonsense:
template <typename T>
struct make_unique_impl<T&&> {};

}  // namespace detail

/**
 * @brief Create a new std::unique_ptr that points-to the given type, direct-initialized based on
 * the given arguments.
 *
 * @tparam T Any non-array object type or any array of unknown bound.
 * @param args If T is a non-array object type, these are the constructor arguments used to
 * direct-initialize the object. If T is an array of unknown bound, then the sole argument must be a
 * single std::size_t that specifies the number of objects to allocate in the array.
 *
 * Requires:
 * - If T is an array of unknown bounds, then args... must be a single size_t and the element type
 * of T must be value-initializable.
 * - Otherwise, if T is a non-array object type, then T must be direct-initializable with arguments
 * `args...`
 * - Otherwise, this function is excluded from overload resolution.
 */
template <typename T,
          typename... Args,
          typename Impl = detail::make_unique_impl<T>,
          typename std::enable_if<!std::is_array<T>::value,
                                  decltype(Impl::make(std::true_type{}, std::declval<Args>()...),
                                           void())>::type* = nullptr>
std::unique_ptr<T> make_unique(Args&&... args) {
    return Impl::make(std::true_type{}, std::forward<Args>(args)...);
}

/**
 * @copydoc bsoncxx::v_noabi::stdx::make_unique
 */
template <
    typename T,
    typename Impl = detail::make_unique_impl<T>,
    typename std::enable_if<std::is_array<T>::value,
                            decltype(Impl::make(std::true_type{}, std::declval<std::size_t>()),
                                     void())>::type* = nullptr>
std::unique_ptr<T> make_unique(std::size_t count) {
    return Impl::make(std::true_type{}, count);
}

/**
 * @brief Create a new std::unique_ptr that points-to a default-initialized instance of the given
 * type.
 *
 * @tparam T A non-array object type or an array of unknown bound
 * @param args If T is an object type, then args... must no arguments are allowed.
 * If T is an array of unknown bound, then args... must be a single size_t specifying
 * the length of the array to allocate.
 *
 * Requires:
 * - T must be default-initializable
 * - If T is an array of unknown bounds, then args... must be a single size_t
 * - Otherwise, if T is a non-array object type, args... must be empty
 * - Otherwise, this function is excluded from overload resolution
 */
template <typename T,
          typename... Args,
          typename Impl = detail::make_unique_impl<T>,
          typename std::enable_if<!std::is_array<T>::value,
                                  decltype(Impl::make(std::false_type{}, std::declval<Args>()...),
                                           void())>::type* = nullptr>
std::unique_ptr<T> make_unique_for_overwrite(Args&&... args) {
    return Impl::make(std::false_type{}, std::forward<Args>(args)...);
}

/**
 * @copydoc bsoncxx::v_noabi::stdx::make_unique_for_overwrite
 */
template <
    typename T,
    typename Impl = detail::make_unique_impl<T>,
    typename std::enable_if<std::is_array<T>::value,
                            decltype(Impl::make(std::false_type{}, std::declval<std::size_t>()),
                                     void())>::type* = nullptr>
std::unique_ptr<T> make_unique_for_overwrite(std::size_t count) {
    return Impl::make(std::false_type{}, count);
}

}  // namespace stdx
}  // namespace v_noabi
}  // namespace bsoncxx

#include <bsoncxx/config/postlude.hpp>
