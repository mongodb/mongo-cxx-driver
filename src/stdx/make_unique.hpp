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

#include <memory>

namespace mongo {
namespace stdx {

#if __cplusplus >= 201402L

using make_unique = ::std::make_unique;

#else

#include <type_traits>
#include <utility>
    //Below code taken from N3588

    template<class T> struct _Never_true : std::false_type { };

    template<class T> struct _Unique_if {
        typedef std::unique_ptr<T> _Single;
    };

    template<class T> struct _Unique_if<T[]> {
        typedef std::unique_ptr<T[]> _Runtime;
    };

    template<class T, size_t N> struct _Unique_if<T[N]> {
        static_assert(_Never_true<T>::value, "make_unique forbids T[N]. Please use T[].");
    };

    template<class T, class... Args> typename _Unique_if<T>::_Single make_unique(Args&&... args) {
        return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
    }

    template<class T> typename _Unique_if<T>::_Single make_unique_default_init() {
        return std::unique_ptr<T>(new T);
    }

    template<class T> typename _Unique_if<T>::_Runtime make_unique(size_t n) {
        typedef typename std::remove_extent<T>::type U;
        return std::unique_ptr<T>(new U[n]());
    }

    template<class T> typename _Unique_if<T>::_Runtime make_unique_default_init(size_t n) {
        typedef typename std::remove_extent<T>::type U;
        return std::unique_ptr<T>(new U[n]);
    }

    template<class T, class... Args> typename _Unique_if<T>::_Runtime make_unique_value_init(size_t n, Args&&... args) {
        typedef typename std::remove_extent<T>::type U;
        return std::unique_ptr<T>(new U[n]{ std::forward<Args>(args)... });
    }

    template<class T, class... Args> typename _Unique_if<T>::_Runtime make_unique_auto_size(Args&&... args) {
        typedef typename std::remove_extent<T>::type U;
        return std::unique_ptr<T>(new U[sizeof...(Args)]{ std::forward<Args>(args)... });
    }

#endif

}  // namespace stdx
}  // namespace mongo
