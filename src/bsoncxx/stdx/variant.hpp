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
#include <core/variant.hpp>
#else
#include <bsoncxx/third_party/mnmlstc/core/variant.hpp>
#endif

namespace bsoncxx {
BSONCXX_INLINE_NAMESPACE_BEGIN
namespace stdx {

using ::core::bad_variant_get;
using ::core::variant;

}  // namespace stdx
BSONCXX_INLINE_NAMESPACE_END
}  // namespace bsoncxx

#elif defined(BSONCXX_POLY_USE_BOOST)
#include <boost/variant.hpp>
#include <boost/variant/get.hpp>

namespace bsoncxx {
BSONCXX_INLINE_NAMESPACE_BEGIN
namespace stdx {

using ::boost::variant;
using bad_variant_get = ::boost::bad_get;

}  // namespace stdx
BSONCXX_INLINE_NAMESPACE_END
}  // namespace bsoncxx

#elif defined(BSONCXX_POLY_USE_STD_EXPERIMENTAL)

#include <experimental/variant>

namespace bsoncxx {
BSONCXX_INLINE_NAMESPACE_BEGIN
namespace stdx {

using ::std::experimental::bad_variant_get;
using ::std::experimental::variant;

}  // namespace stdx
BSONCXX_INLINE_NAMESPACE_END
}  // namespace bsoncxx

#elif defined(BSONCXX_POLY_USE_STD)

#include <variant>

namespace bsoncxx {
BSONCXX_INLINE_NAMESPACE_BEGIN
namespace stdx {

using ::std::bad_variant_get;
using ::std::variant;

}  // namespace stdx
BSONCXX_INLINE_NAMESPACE_END
}  // namespace bsoncxx

#else
#error "Cannot find a valid polyfill for variant"
#endif

#include <bsoncxx/config/postlude.hpp>
