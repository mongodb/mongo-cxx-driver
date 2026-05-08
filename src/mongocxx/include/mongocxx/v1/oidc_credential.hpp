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

#include <mongocxx/v1/oidc_credential-fwd.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/detail/prelude.hpp>

#include <bsoncxx/v1/stdx/optional.hpp>
#include <bsoncxx/v1/stdx/string_view.hpp>

#include <mongocxx/v1/config/export.hpp>

#include <chrono>
#include <string>

namespace mongocxx {
namespace v1 {

///
/// The credential returned by an OIDC callback.
///
class oidc_credential {
    class impl;
    void* _impl;

   public:
    ///
    /// Destroy this object.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~oidc_credential();

    ///
    /// Move constructor.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() oidc_credential(oidc_credential&& other) noexcept;

    ///
    /// Move assignment.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(oidc_credential&) operator=(oidc_credential&& other) noexcept;

    ///
    /// Copy construction.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() oidc_credential(oidc_credential const& other);

    ///
    /// Copy assignment.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(oidc_credential&) operator=(oidc_credential const& other);

    ///
    /// Construct a credential with an access token.
    ///
    explicit MONGOCXX_ABI_EXPORT_CDECL() oidc_credential(std::string access_token);

    ///
    /// Construct a credential with an access token and expiry duration.
    /// The expiry duration will be evaluated relative to the system clock immediately after the callback returns.
    ///
    explicit MONGOCXX_ABI_EXPORT_CDECL() oidc_credential(
        std::string access_token,
        std::chrono::milliseconds expires_in);

    ///
    /// Return the access token.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::string_view) access_token() const;

    ///
    /// Return the optional expiry duration.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<std::chrono::milliseconds>) expires_in() const;

    class internal;
};

} // namespace v1
} // namespace mongocxx

#include <mongocxx/v1/detail/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v1::oidc_credential.
///
