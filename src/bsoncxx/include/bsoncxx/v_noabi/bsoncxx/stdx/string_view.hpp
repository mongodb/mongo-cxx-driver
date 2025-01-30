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

#include <bsoncxx/v1/stdx/string_view.hpp>

namespace bsoncxx {
namespace v_noabi {
namespace stdx {

using v1::stdx::basic_string_view;
using v1::stdx::string_view;

} // namespace stdx
} // namespace v_noabi
} // namespace bsoncxx

namespace bsoncxx {
namespace stdx {

#if defined(BSONCXX_API_OVERRIDE_DEFAULT_ABI)

using v1::stdx::basic_string_view;
using v1::stdx::string_view;

#else

using v_noabi::stdx::basic_string_view;
using v_noabi::stdx::string_view;

#endif // defined(BSONCXX_API_OVERRIDE_DEFAULT_ABI)

} // namespace stdx
} // namespace bsoncxx

///
/// @file
/// Provides `std::string_view`-related polyfills for library API usage.
///
/// @par Includes
/// - @ref bsoncxx/v1/stdx/string_view.hpp
///
/// @see
/// - [Choosing a C++17 Polyfill](https://www.mongodb.com/docs/languages/cpp/cpp-driver/current/polyfill-selection/)
///
