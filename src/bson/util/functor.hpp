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

#include "driver/config/prelude.hpp"

#include <type_traits>

namespace bson {
namespace util {
namespace functor {

template <typename, typename>
struct build_free_function;

template <typename F, typename R, typename... Args>
struct build_free_function<F, R(Args...)> {
    typedef R (*type)(Args...);
};

template <typename, typename>
struct build_class_function;

template <typename C, typename R, typename... Args>
struct build_class_function<C, R(Args...)> {
    typedef R (C::*type)(Args...);
};

template <typename>
struct strip_cv_from_class_function;

template <typename C, typename R, typename... Args>
struct strip_cv_from_class_function<R (C::*)(Args...)> {
    typedef R (C::*type)(Args...);
};

template <typename C, typename R, typename... Args>
struct strip_cv_from_class_function<R (C::*)(Args...) const> {
    typedef R (C::*type)(Args...);
};

template <typename C, typename R, typename... Args>
struct strip_cv_from_class_function<R (C::*)(Args...) volatile> {
    typedef R (C::*type)(Args...);
};

template <typename C, typename S>
struct is_class_method_with_signature {
    typedef int yes;
    typedef char no;

    // T stands for SFINAE
    template <typename T>
    static typename std::enable_if<std::is_convertible<typename build_class_function<C, S>::type,
                                                       typename strip_cv_from_class_function<
                                                           decltype(&T::operator())>::type>::value,
                                   yes>::type
        sfinae(void *);

    template <typename>
    static no sfinae(...);

    static bool constexpr value = sizeof(sfinae<C>(nullptr)) == sizeof(yes);
};

template <typename F, typename S>
struct is_function_with_signature
    : std::is_convertible<F, typename build_free_function<F, S>::type> {};

template <typename C, typename S, bool>
struct is_functor_impl : is_class_method_with_signature<C, S> {};

template <typename F, typename S>
struct is_functor_impl<F, S, false> : is_function_with_signature<F, S> {};

}  // functor

template <typename C, typename S>
struct is_functor : functor::is_functor_impl<C, S, std::is_class<C>::value> {};

}  // util
}  // bson

#include "driver/config/postlude.hpp"
