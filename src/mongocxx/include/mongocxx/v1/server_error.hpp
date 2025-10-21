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

#include <mongocxx/v1/server_error-fwd.hpp>

//

#include <mongocxx/v1/detail/prelude.hpp>

#include <bsoncxx/v1/document/view-fwd.hpp>

#include <bsoncxx/v1/stdx/string_view.hpp>

#include <mongocxx/v1/config/export.hpp>
#include <mongocxx/v1/exception.hpp>

#include <memory>
#include <system_error>

namespace mongocxx {
namespace v1 {

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
    std::unique_ptr<impl> _impl;

   public:
    ///
    /// Destroy this object.
    ///
    /// @warning Invalidates all associated views.
    ///
    ~server_error() override;

    ///
    /// Move constructor.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_CDECL server_error(server_error&& other) noexcept;

    ///
    /// Move assignment.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    server_error& MONGOCXX_ABI_CDECL operator=(server_error&& other) noexcept;

    ///
    /// Copy construction.
    ///
    MONGOCXX_ABI_CDECL server_error(server_error const& other);

    ///
    /// Copy assignment.
    ///
    server_error& MONGOCXX_ABI_CDECL operator=(server_error const& other);

    // Inherit constructors.
    using v1::exception::exception;

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

    ///
    /// Return true if the raw server error contains the specified error label.
    ///
    /// @important The set of error labels may vary depending on the operation and error.
    ///
    bool MONGOCXX_ABI_CDECL has_error_label(bsoncxx::v1::stdx::string_view label) const;
};

} // namespace v1
} // namespace mongocxx

#include <mongocxx/v1/detail/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v1::server_error.
///
