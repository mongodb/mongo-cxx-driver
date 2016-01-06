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

#include <bsoncxx/config/prelude.hpp>

#if defined(BSONCXX_POLY_USE_MNMLSTC)

#if defined(MONGO_CXX_DRIVER_COMPILING) || defined(BSONCXX_POLY_USE_SYSTEM_MNMLSTC)
#include <core/optional.hpp>
#else
#include <bsoncxx/third_party/mnmlstc/core/optional.hpp>
#endif

namespace bsoncxx {
namespace stdx {

using ::core::optional;
using ::core::nullopt;
using ::core::make_optional;

}  // namespace stdx
}  // namespace bsoncxx

#elif defined(BSONCXX_POLY_USE_BOOST)

#include <boost/none.hpp>
#include <boost/optional/optional.hpp>

namespace bsoncxx {
namespace stdx {

using ::boost::optional;
using nullopt_t = ::boost::none_t;

// TODO(MSVC): This would be better expressed as constexpr, but VS2015U1 can't do it.
const nullopt_t nullopt{::boost::none};
using ::boost::make_optional;

}  // namespace stdx
}  // namespace bsoncxx

#elif defined(BSONCXX_POLY_USE_STD_EXPERIMENTAL)

#include <experimental/optional>

namespace bsoncxx {
namespace stdx {

using ::std::experimental::optional;
using ::std::experimental::nullopt;
using ::std::experimental::make_optional;

}  // namespace stdx
}  // namespace bsoncxx

#else
#error "Cannot find a valid polyfill for optional"
#endif

#include <bsoncxx/config/postlude.hpp>
