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
#include <core/string.hpp>
#else
#include <bsoncxx/third_party/mnmlstc/core/string.hpp>
#endif

namespace bsoncxx {
namespace stdx {

using ::core::string_view;

}  // namespace stdx
}  // namespace bsoncxx

#elif defined(BSONCXX_POLY_USE_BOOST)

#include <boost/utility/string_ref.hpp>

namespace bsoncxx {
namespace stdx {

using string_view = ::boost::string_ref;

}  // namespace stdx
}  // namespace bsoncxx

#elif defined(BSONCXX_POLY_USE_STD_EXPERIMENTAL)

#include <experimental/string_view>

namespace bsoncxx {
namespace stdx {

using ::std::experimental::string_view;

}  // namespace stdx
}  // namespace bsoncxx

#else
#error "Cannot find a valid polyfill for string_view"
#endif

#include <bsoncxx/config/postlude.hpp>
