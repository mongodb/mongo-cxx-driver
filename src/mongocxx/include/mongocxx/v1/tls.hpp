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

#include <mongocxx/v1/tls-fwd.hpp>

//

#include <mongocxx/v1/detail/prelude.hpp>

#include <bsoncxx/v1/stdx/optional.hpp>
#include <bsoncxx/v1/stdx/string_view.hpp>

#include <mongocxx/v1/config/export.hpp>

#include <string>

namespace mongocxx {
namespace v1 {

///
/// Options related to TLS configuration.
///
/// Supported fields include:
/// - `allow_invalid_certificates` ("weak_cert_validation")
/// - `ca_dir`
/// - `ca_file`
/// - `crl_file`
/// - `pem_file`
/// - `pem_password` ("pem_pwd")
///
/// @see
/// - [`mongoc_ssl_opt_t` (mongoc)](https://mongoc.org/libmongoc/current/mongoc_ssl_opt_t.html)
/// - [TLS/SSL (Transport Encryption) (MongoDB Manual)](https://www.mongodb.com/docs/manual/core/security-transport-encryption/)
///
class tls {
   private:
    class impl;
    void* _impl;

   public:
    ///
    /// Destroy this object.
    ///
    /// @warning Invalidates all associated views.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~tls();

    ///
    /// Move constructor.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() tls(tls&& other) noexcept;

    ///
    /// Move assignment.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(tls&) operator=(tls&& other) noexcept;

    ///
    /// Copy construction.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() tls(tls const& other);

    ///
    /// Copy assignment.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(tls&) operator=(tls const& other);

    ///
    /// Default initialization.
    ///
    /// @par Postconditions:
    /// - All supported fields are "unset" or zero-initialized.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() tls();

    ///
    /// Set the "pem_file" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(tls&) pem_file(std::string v);

    ///
    /// Return the current "pem_file" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::stdx::string_view>) pem_file() const;

    ///
    /// Set the "pem_pwd" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(tls&) pem_password(std::string v);

    ///
    /// Return the current "pem_pwd" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::stdx::string_view>) pem_password() const;

    ///
    /// Set the "ca_file" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(tls&) ca_file(std::string v);

    ///
    /// Retur the current "ca_file" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::stdx::string_view>) ca_file() const;

    ///
    /// Set the "ca_dir" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(tls&) ca_dir(std::string v);

    ///
    /// Return the current "ca_dir" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::stdx::string_view>) ca_dir() const;

    ///
    /// Set the "crl_file" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(tls&) crl_file(std::string v);

    ///
    /// Return the current "crl_file" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::stdx::string_view>) crl_file() const;

    ///
    /// Set the "weak_cert_validation" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(tls&) allow_invalid_certificates(bool v);

    ///
    /// Return the current "weak_cert_validation" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bool>) allow_invalid_certificates() const;
};

} // namespace v1
} // namespace mongocxx

#include <mongocxx/v1/detail/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v1::tls.
///
