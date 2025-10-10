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

#include <mongocxx/v1/client_encryption-fwd.hpp>

//

#include <mongocxx/v1/detail/prelude.hpp>

#include <bsoncxx/v1/document/value-fwd.hpp>
#include <bsoncxx/v1/document/view-fwd.hpp>
#include <bsoncxx/v1/types/value-fwd.hpp>
#include <bsoncxx/v1/types/view-fwd.hpp>

#include <mongocxx/v1/client-fwd.hpp>
#include <mongocxx/v1/collection-fwd.hpp>
#include <mongocxx/v1/cursor-fwd.hpp>
#include <mongocxx/v1/data_key-fwd.hpp>
#include <mongocxx/v1/database-fwd.hpp>
#include <mongocxx/v1/delete_one_result-fwd.hpp>
#include <mongocxx/v1/encrypt-fwd.hpp>
#include <mongocxx/v1/rewrap_many_datakey_options-fwd.hpp>
#include <mongocxx/v1/rewrap_many_datakey_result-fwd.hpp>

#include <bsoncxx/v1/stdx/optional.hpp>
#include <bsoncxx/v1/stdx/string_view.hpp>

#include <mongocxx/v1/config/export.hpp>

#include <memory>
#include <string>
#include <utility>

namespace mongocxx {
namespace v1 {

///
/// Support for MongoDB In-Use Encryption.
///
/// Provides interfaces to support both Queryable Encryption and Client-Side Field Level Encryption (CSFLE).
///
/// @see
/// - [Client Side Encryption (MongoDB Specifications)](https://specifications.readthedocs.io/en/latest/client-side-encryption/client-side-encryption/)
/// - [In-Use Encryption (MongoDB Manual)](https://www.mongodb.com/docs/manual/core/csfle/)
///   - [Queryable Encryption](https://www.mongodb.com/docs/manual/core/queryable-encryption/)
///   - [CSFLE](https://www.mongodb.com/docs/manual/core/csfle/)
///
/// @attention This feature is experimental! It is not ready for use!
///
class client_encryption {
   private:
    class impl;
    std::unique_ptr<impl> _impl;

   public:
    class options;

    ///
    /// Destroy this object.
    ///
    /// @warning Invalidates all associated views.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~client_encryption();

    ///
    /// Move constructor.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() client_encryption(client_encryption&& other) noexcept;

    ///
    /// Move assignment.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(client_encryption&) operator=(client_encryption&& other) noexcept;

    ///
    /// This class is not copyable.
    ///
    client_encryption(client_encryption const& other);

    ///
    /// This class is not copyable.
    ///
    client_encryption& operator=(client_encryption const& other);

    ///
    /// Initialize with the given options.
    ///
    explicit MONGOCXX_ABI_EXPORT_CDECL() client_encryption(options const& opts);

    ///
    /// Create a new data key within the key vault collection.
    ///
    /// @returns The new key document ID as a @ref bsoncxx::v1::types::binary_subtype::k_uuid.
    ///
    /// @throws mongocxx::v1::server_error when a server-side error is encountered and a raw server error is available.
    /// @throws mongocxx::v1::exception for all other runtime errors.
    ///
    /// @{
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::types::value) create_data_key(
        bsoncxx::v1::stdx::string_view kms_provider,
        v1::data_key const& opts);

    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::types::value) create_data_key(bsoncxx::v1::stdx::string_view kms_provider);
    /// @}
    ///

    ///
    /// Create a collection with encrypted fields.
    ///
    /// @param db The database within which to create the encrypted collection.
    /// @param name The name of the new encrypted collection.
    /// @param opts "createCollection" options.
    /// @param coll_opts Set to the options used to create the encrypted collection, including the "encryptedFields"
    /// field.
    /// @param kms_provider The KMS provider to use for this operation.
    /// @param master_key The master key to use when creating a new data key.
    ///
    /// @throws mongocxx::v1::server_error when a server-side error is encountered and a raw server error is available.
    /// @throws mongocxx::v1::exception for all other runtime errors.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(v1::collection) create_encrypted_collection(
        v1::database& db,
        bsoncxx::v1::stdx::string_view name,
        bsoncxx::v1::document::view opts,
        bsoncxx::v1::document::value& coll_opts,
        bsoncxx::v1::stdx::string_view kms_provider,
        bsoncxx::v1::document::view master_key);

    ///
    /// Create a collection with encrypted fields.
    ///
    /// @param db The database within which to create the encrypted collection.
    /// @param name The name of the new encrypted collection.
    /// @param opts "createCollection" options.
    /// @param coll_opts Set to the options used to create the encrypted collection, including the "encryptedFields"
    /// field.
    /// @param kms_provider The KMS provider to use for this operation.
    ///
    /// @throws mongocxx::v1::server_error when a server-side error is encountered and a raw server error is available.
    /// @throws mongocxx::v1::exception for all other runtime errors.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(v1::collection) create_encrypted_collection(
        v1::database& db,
        bsoncxx::v1::stdx::string_view name,
        bsoncxx::v1::document::view opts,
        bsoncxx::v1::document::value& coll_opts,
        bsoncxx::v1::stdx::string_view kms_provider);

    ///
    /// Encrypt the given value.
    ///
    /// @returns The encrypted value as a @ref bsoncxx::v1::types::binary_subtype::k_encrypted.
    ///
    /// @throws mongocxx::v1::server_error when a server-side error is encountered and a raw server error is available.
    /// @throws mongocxx::v1::exception for all other runtime errors.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::types::value) encrypt(
        bsoncxx::v1::types::view value,
        v1::encrypt const& opts);

    ///
    /// Encrypt the given Match Expression or Aggregate Expression.
    ///
    /// @note This operation is only supported when the "queryType" field is "range" and the "algorithm" field is
    /// "Range".
    ///
    /// @returns The encrypted expression.
    ///
    /// @throws mongocxx::v1::server_error when a server-side error is encountered and a raw server error is available.
    /// @throws mongocxx::v1::exception for all other runtime errors.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::document::value) encrypt_expression(
        bsoncxx::v1::document::view expr,
        v1::encrypt const& opts);

    ///
    /// Decrypt the given value.
    ///
    /// @throws mongocxx::v1::server_error when a server-side error is encountered and a raw server error is available.
    /// @throws mongocxx::v1::exception for all other runtime errors.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::types::value) decrypt(bsoncxx::v1::types::view value);

    ///
    /// Decrypt and (re-)encrypt multiple data keys.
    ///
    /// When the "masterKey" field is set, the data keys are are (re-)encrypted using the new "masterKey". Otherwise,
    /// each data key is (re-)encrypted using their current "masterKey".
    ///
    /// All rewrapped data keys are updated in the key vault collection using a single bulk write operation. No bulk
    /// write operation is executed when no data keys are found that match the given `filter`.
    ///
    /// @throws mongocxx::v1::server_error when a server-side error is encountered and a raw server error is available.
    /// @throws mongocxx::v1::exception for all other runtime errors.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(v1::rewrap_many_datakey_result) rewrap_many_datakey(
        bsoncxx::v1::document::view filter,
        v1::rewrap_many_datakey_options const& opts);

    ///
    /// Delete the specified data key.
    ///
    /// @throws mongocxx::v1::server_error when a server-side error is encountered and a raw server error is available.
    /// @throws mongocxx::v1::exception for all other runtime errors.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(v1::delete_one_result) delete_key(bsoncxx::v1::types::value id);

    ///
    /// Return the requested data key.
    ///
    /// @returns The raw server response.
    ///
    /// @throws mongocxx::v1::server_error when a server-side error is encountered and a raw server error is available.
    /// @throws mongocxx::v1::exception for all other runtime errors.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::document::value) get_key(bsoncxx::v1::types::view id);

    ///
    /// Return all data keys within the associated key vault collection.
    ///
    /// @throws mongocxx::v1::server_error when a server-side error is encountered and a raw server error is available.
    /// @throws mongocxx::v1::exception for all other runtime errors.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(v1::cursor) get_keys();

    ///
    /// Add a new keyAltName to the specified data key.
    ///
    /// @returns The data key before the addition of the new keyAltName.
    ///
    /// @throws mongocxx::v1::server_error when a server-side error is encountered and a raw server error is available.
    /// @throws mongocxx::v1::exception for all other runtime errors.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::document::value) add_key_alt_name(
        bsoncxx::v1::types::view id,
        bsoncxx::v1::stdx::string_view key_alt_name);

    ///
    /// Remove the specified data key.
    ///
    /// @returns The data key before its removal.
    ///
    /// @throws mongocxx::v1::server_error when a server-side error is encountered and a raw server error is available.
    /// @throws mongocxx::v1::exception for all other runtime errors.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::document::value) remove_key_alt_name(
        bsoncxx::v1::types::view id,
        bsoncxx::v1::stdx::string_view key_alt_name);

    ///
    /// Return the specified data key.
    ///
    /// @throws mongocxx::v1::server_error when a server-side error is encountered and a raw server error is available.
    /// @throws mongocxx::v1::exception for all other runtime errors.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::document::value) get_key_by_alt_name(
        bsoncxx::v1::stdx::string_view key_alt_name);
};

///
/// Options for @ref mongocxx::v1::client_encryption.
///
/// Supported fields include:
/// - `key_vault_client` ("keyVaultClient")
/// - `key_vault_namespace` ("keyVaultNamespace")
/// - `kms_providers` ("kmsProviders")
/// - `tls_opts` ("tlsOptions")
///
/// @attention This feature is experimental! It is not ready for use!
///
class client_encryption::options {
   private:
    class impl;
    std::unique_ptr<impl> _impl;

   public:
    ///
    /// Destroy this object.
    ///
    /// @warning Invalidates all associated views.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~options();

    ///
    /// Move constructor.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() options(options&& other) noexcept;

    ///
    /// Move assignment.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(options&) operator=(options&& other) noexcept;

    ///
    /// Copy construction.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() options(options const& other);

    ///
    /// Copy assignment.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(options&) operator=(options const& other);

    ///
    /// Default initialization.
    ///
    /// @par Postconditions:
    /// - All supported fields are "unset" or zero-initialized.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() options();

    ///
    /// Set the "keyVaultClient" field.
    ///
    /// @important The associated client MUST outlive any client or pool configured to use this option.
    ///
    /// @param v Equivalent to "unset" when null.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(options&) key_vault_client(mongocxx::v1::client* v);

    ///
    /// Return the current "keyVaultClient" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<mongocxx::v1::client*>) key_vault_client() const;

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
    MONGOCXX_ABI_EXPORT_CDECL(options&) key_vault_namespace(ns_pair v);

    ///
    /// Return the current "keyVaultNamespace" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<ns_pair>) key_vault_namespace() const;

    ///
    /// Set the "kmsProviders" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(options&) kms_providers(bsoncxx::v1::document::value v);

    ///
    /// Return the current "kmsProviders" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view>) kms_providers() const;

    ///
    /// Set the "tlsOptions" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(options&) tls_opts(bsoncxx::v1::document::value v);

    ///
    /// Return the current "tlsOptions" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view>) tls_opts() const;
};

} // namespace v1
} // namespace mongocxx

#include <mongocxx/v1/detail/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v1::client_encryption.
///
