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

#include <mongocxx/v1/auto_encryption-fwd.hpp>

//

#include <mongocxx/v1/detail/prelude.hpp>

#include <bsoncxx/v1/document/value-fwd.hpp>
#include <bsoncxx/v1/document/view-fwd.hpp>

#include <mongocxx/v1/client-fwd.hpp>
#include <mongocxx/v1/pool-fwd.hpp>

#include <bsoncxx/v1/stdx/optional.hpp>

#include <mongocxx/v1/config/export.hpp>

#include <memory>
#include <string>
#include <utility>

namespace mongocxx {
namespace v1 {

///
/// Options related In-Use Encryption configuration.
///
/// Supported fields include:
/// - `bypass_auto_encryption` ("bypassAutoEncryption")
/// - `bypass_query_analysis` ("bypassQueryAnalysis")
/// - `encrypted_fields_map` ("encryptedFieldsMap")
/// - `extra_options` ("extraOptions")
/// - `key_vault_client` ("keyVaultClient")
/// - `key_vault_namespace` ("keyVaultNamespace")
/// - `key_vault_pool` ("keyVaultPool")
/// - `kms_providers` ("kmsProviders")
/// - `schema_map` ("schemaMap")
/// - `tls_options` ("tlsOptions")
///
/// @see
/// - [MongoClient Options for Queryable Encryption (MongoDB Manual)](https://www.mongodb.com/docs/manual/core/queryable-encryption/reference/qe-options-clients/)
/// - [MongoClient Options for CSFLE](https://www.mongodb.com/docs/manual/core/csfle/reference/csfle-options-clients/)
///
class auto_encryption {
   private:
    class impl;
    std::unique_ptr<impl> _impl;

   public:
    ///
    /// Destroy this object.
    ///
    /// @warning Invalidates all associated views.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~auto_encryption();

    ///
    /// Move constructor.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() auto_encryption(auto_encryption&& other) noexcept;

    ///
    /// Move assignment.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(auto_encryption&) operator=(auto_encryption&& other) noexcept;

    ///
    /// Copy construction.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() auto_encryption(auto_encryption const& other);

    ///
    /// Copy assignment.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(auto_encryption&) operator=(auto_encryption const& other);

    ///
    /// Default initialization.
    ///
    /// @par Postconditions:
    /// - All supported fields are "unset" or zero-initialized.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() auto_encryption();

    ///
    /// Set the "keyVaultClient" field.
    ///
    /// @important The associated client MUST outlive any client or pool configured to use this option.
    ///
    /// @note The "keyVaultClient" and "keyVaultPool" fields are mutually exclusive.
    ///
    /// @param v Equivalent to "unset" when null.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(auto_encryption&) key_vault_client(v1::client* v);

    ///
    /// Return the current "keyVaultClient" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(v1::client*) key_vault_client() const;

    ///
    /// Set the "keyVaultPool" field.
    ///
    /// Equivalent to "keyVaultClient", but acquires a client from the given pool on-demand.
    ///
    /// @important The associated pool MUST outlive any client or pool configured to use this option.
    ///
    /// @note The "keyVaultClient" and "keyVaultPool" fields are mutually exclusive.
    ///
    /// @param v Equivalent to "unset" when null.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(auto_encryption&) key_vault_pool(v1::pool* v);

    ///
    /// Return the current "keyVaultPool" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(v1::pool*) key_vault_pool() const;

    ///
    /// The name of a database and a collection.
    ///
    /// ```cpp
    /// ns_pair{"db", "coll"} // db.coll
    /// ```
    ///
    using ns_pair = std::pair<std::string, std::string>;

    ///
    /// Set the "keyVaultNamespace" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(auto_encryption&) key_vault_namespace(ns_pair v);

    ///
    /// Return the current "keyVaultNamespace" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<ns_pair>) key_vault_namespace() const;

    ///
    /// Set the "kmsProviders" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(auto_encryption&) kms_providers(bsoncxx::v1::document::value v);

    ///
    /// Return the current "kmsProviders" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view>) kms_providers() const;

    ///
    /// Set the "tlsOptions" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(auto_encryption&) tls_opts(bsoncxx::v1::document::value v);

    ///
    /// Return the current "tlsOptions" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view>) tls_opts() const;

    ///
    /// Set the "schemaMap" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(auto_encryption&) schema_map(bsoncxx::v1::document::value v);

    ///
    /// Return the "schemaMap" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view>) schema_map() const;

    ///
    /// Set the "encryptedFieldsMap" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(auto_encryption&) encrypted_fields_map(bsoncxx::v1::document::value v);

    ///
    /// Return the current "encryptedFieldsMap" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view>) encrypted_fields_map() const;

    ///
    /// Set the "bypassAutoEncryption" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(auto_encryption&) bypass_auto_encryption(bool v);

    ///
    /// Return the current "bypassAutoEncryption" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bool) bypass_auto_encryption() const;

    ///
    /// Set the "bypassQueryAnalysis" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(auto_encryption&) bypass_query_analysis(bool v);

    ///
    /// Return the current "bypassQueryAnalysis" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bool) bypass_query_analysis() const;

    ///
    /// Set the "extraOptions" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(auto_encryption&) extra_options(bsoncxx::v1::document::value v);

    ///
    /// Return the current "extraOptions" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view>) extra_options() const;
};

} // namespace v1
} // namespace mongocxx

#include <mongocxx/v1/detail/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v1::auto_encryption.
///
