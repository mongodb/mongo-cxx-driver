// Copyright 2020 MongoDB Inc.
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

#include <bsoncxx/config/prelude.hpp>

#if defined(BSONCXX_POLY_USE_MNMLSTC)

#if defined(MONGO_CXX_DRIVER_COMPILING) || defined(BSONCXX_POLY_USE_SYSTEM_MNMLSTC)
#include <core/any.hpp>
#else
#include <bsoncxx/third_party/mnmlstc/core/any.hpp>
#endif

namespace bsoncxx {
BSONCXX_INLINE_NAMESPACE_BEGIN
namespace stdx {

using ::core::any;
using ::core::any_cast;
using ::core::bad_any_cast;

}  // namespace stdx
BSONCXX_INLINE_NAMESPACE_END
}  // namespace bsoncxx

#elif defined(BSONCXX_POLY_USE_BOOST)
#include <boost/any.hpp>

namespace bsoncxx {
BSONCXX_INLINE_NAMESPACE_BEGIN
namespace stdx {

using ::boost::any;
using ::boost::any_cast;
using ::boost::bad_any_cast;

}  // namespace stdx
BSONCXX_INLINE_NAMESPACE_END
}  // namespace bsoncxx

#elif defined(BSONCXX_POLY_USE_STD_EXPERIMENTAL)

#include <experimental/any>

namespace bsoncxx {
BSONCXX_INLINE_NAMESPACE_BEGIN
namespace stdx {

using ::std::experimental::any;
using ::std::experimental::any_cast;
using ::std::experimental::bad_any_cast;

}  // namespace stdx
BSONCXX_INLINE_NAMESPACE_END
}  // namespace bsoncxx

#elif defined(BSONCXX_POLY_USE_STD)

#include <any>

namespace bsoncxx {
BSONCXX_INLINE_NAMESPACE_BEGIN
namespace stdx {

using ::std::any;
using ::std::any_cast;
using ::std::bad_any_cast;

}  // namespace stdx
BSONCXX_INLINE_NAMESPACE_END
}  // namespace bsoncxx

#else
#error "Cannot find a valid polyfill for any"
#endif

#include <bsoncxx/config/postlude.hpp>
