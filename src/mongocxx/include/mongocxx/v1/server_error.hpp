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

#include <mongocxx/v1/server_error-fwd.hpp> // IWYU pragma: export

//

#include <string>
#include <system_error>

namespace mongocxx {
namespace v1 {

///
/// A MongoDB server error.
///
/// @important This class may contain both a client error code _and_ a server error code:
/// - Use `this->code()` to obtain the primary error code (which may equal `this->server_code()`).
/// - Use `this->server_code()` to obtain the server error code (which may equal `zero`).
/// Use @ref mongocxx::v1::source_errc to determine the origin of `this->code()`.
///
/// @par Inherits:
/// - @ref mongocxx::v1::exception
///
class server_error {};

} // namespace v1
} // namespace mongocxx

///
/// @file
/// Provides @ref mongocxx::v1::server_error.
///
