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

#include <bsoncxx/v1/exception-fwd.hpp>

//

#include <bsoncxx/v1/detail/prelude.hpp>

#include <system_error>

namespace bsoncxx {
namespace v1 {
namespace error {

///
/// Enumeration identifying the source of a @ref bsoncxx::v1 error.
///
/// @attention This feature is experimental! It is not ready for use!
///
enum class source {};

///
/// Enumeration identifying the type (cause) of a @ref bsoncxx::v1 error.
///
/// @attention This feature is experimental! It is not ready for use!
///
enum class type {};

} // namespace error
} // namespace v1
} // namespace bsoncxx

namespace bsoncxx {
namespace v1 {

///
/// Base class for all exceptions thrown by @ref bsoncxx::v1.
///
/// @attention This feature is experimental! It is not ready for use!
///
class exception {};

} // namespace v1
} // namespace bsoncxx

#include <bsoncxx/v1/detail/postlude.hpp>

///
/// @file
/// Provides bsoncxx error-handling utilities.
///
