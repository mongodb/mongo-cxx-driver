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

#include <mongocxx/v1/detail/prelude.hpp>

#include <bsoncxx/v1/document/view-fwd.hpp>

#include <bsoncxx/v1/detail/macros.hpp>

#include <mongocxx/v1/config/export.hpp>
#include <mongocxx/v1/exception.hpp> // IWYU pragma: export

#include <memory>
#include <system_error>

namespace mongocxx {
namespace v1 {

BSONCXX_PRIVATE_WARNINGS_PUSH();
BSONCXX_PRIVATE_WARNINGS_DISABLE(MSVC(4251));
BSONCXX_PRIVATE_WARNINGS_DISABLE(MSVC(4275));

///
/// A MongoDB server error.
///
/// @important `this->code()` always returns the raw server error code. Use `this->client_code()` to query the
/// client-side error code.
///
/// @par Inherits:
/// - @ref mongocxx::v1::exception
///
class server_error : public v1::exception {
   private:
    class impl;
    std::shared_ptr<impl> _impl;

   public:
    ///
    /// The client error code.
    ///
    /// @returns Default-initialized when no client error code is available.
    ///
    std::error_code MONGOCXX_ABI_CDECL client_code() const;

    ///
    /// The raw server error.
    ///
    /// @important The contents of the resulting BSON document may vary depending on the operation and error.
    ///
    bsoncxx::v1::document::view MONGOCXX_ABI_CDECL raw() const;

    class internal;

   private:
    MONGOCXX_ABI_NO_EXPORT /* explicit(false) */
    server_error(int code, char const* message, std::unique_ptr<impl> impl);

    MONGOCXX_ABI_NO_EXPORT void key_function() const override;
};

BSONCXX_PRIVATE_WARNINGS_POP();

} // namespace v1
} // namespace mongocxx

#include <mongocxx/v1/detail/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v1::server_error.
///
/// @par Includes
/// - @ref mongocxx/v1/exception.hpp
///
