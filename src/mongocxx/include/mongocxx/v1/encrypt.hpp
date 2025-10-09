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

#include <mongocxx/v1/encrypt-fwd.hpp>

//

#include <mongocxx/v1/detail/prelude.hpp>

#include <bsoncxx/v1/types/value-fwd.hpp>
#include <bsoncxx/v1/types/view-fwd.hpp>

#include <mongocxx/v1/range-fwd.hpp>

#include <bsoncxx/v1/stdx/optional.hpp>
#include <bsoncxx/v1/stdx/string_view.hpp>

#include <mongocxx/v1/config/export.hpp>

#include <cstdint>
#include <memory>
#include <string>

namespace mongocxx {
namespace v1 {

///
/// Options related to explicit encryption for In-Use Encryption.
///
/// Supported fields include:
/// - `algorithm`
/// - `contention_factor` ("contentionFactor")
/// - `key_alt_name` ("keyAltName")
/// - `key_id` ("keyId")
/// - `query_type` ("queryType")
/// - `range_opts` ("rangeOpts")
///
/// @see
/// - [Fields and Encryption Types (MongoDB Manual)](https://www.mongodb.com/docs/manual/core/csfle/fundamentals/encryption-algorithms/)
/// - [Encrypted Fields and Enabled Queries (MongoDB Manual)](https://mongodb.com/docs/manual/core/queryable-encryption/fundamentals/encrypt-and-query/)
/// - [Encryption Schemas](https://www.mongodb.com/docs/manual/core/csfle/fundamentals/create-schema/)
///
/// @attention This feature is experimental! It is not ready for use!
///
class encrypt {
    // This class implements `EncryptOpts`:
    //  - https://specifications.readthedocs.io/en/latest/client-side-encryption/client-side-encryption/

   private:
    class impl;
    std::unique_ptr<impl> _impl;

   public:
    ///
    /// An encryption algorithm.
    ///
    /// @see
    /// - [Fields and Encryption Types (MongoDB Manual)](https://www.mongodb.com/docs/manual/core/csfle/fundamentals/encryption-algorithms/)
    ///
    enum class encryption_algorithm : std::uint8_t {
        k_deterministic, ///< "AEAD_AES_256_CBC_HMAC_SHA_512-Deterministic"
        k_random,        ///< "AEAD_AES_256_CBC_HMAC_SHA_512-Random"
        k_indexed,       ///< "Indexed"
        k_unindexed,     ///< "Unindexed"
        k_range,         ///< "Range"
    };

    ///
    /// A query type.
    ///
    /// @see
    /// - [Supported Operations for Queryable Encryption (MongoDB Manual)](https://www.mongodb.com/docs/manual/core/queryable-encryption/reference/supported-operations/)
    ///
    enum class encryption_query_type : std::uint8_t {
        k_equality, ///< "equalty"
        k_range,    ///< "range"
    };

    ///
    /// Destroy this object.
    ///
    /// @warning Invalidates all associated views.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() ~encrypt();

    ///
    /// Move constructor.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() encrypt(encrypt&& other) noexcept;

    ///
    /// Move assignment.
    ///
    /// @par Postconditions:
    /// - `other` is in an assign-or-destroy-only state.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(encrypt&) operator=(encrypt&& other) noexcept;

    ///
    /// Copy construction.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() encrypt(encrypt const& other);

    ///
    /// Copy assignment.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(encrypt&) operator=(encrypt const& other);

    ///
    /// Default initialization.
    ///
    /// @par Postconditions:
    /// - All supported fields are "unset" or zero-initialized.
    ///
    MONGOCXX_ABI_EXPORT_CDECL() encrypt();

    ///
    /// Set the "keyId" field.
    ///
    /// @param v A @ref bsoncxx::v1::types::binary_subtype::k_uuid.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(encrypt&) key_id(bsoncxx::v1::types::value v);

    ///
    /// Return the current "keyId" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::types::view>) key_id() const;

    ///
    /// Set the "keyAltName" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(encrypt&) key_alt_name(std::string v);

    ///
    /// Return the "keyAltName" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<bsoncxx::v1::stdx::string_view>) key_alt_name() const;

    ///
    /// Set the "algorithm" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(encrypt&) algorithm(encryption_algorithm v);

    ///
    /// Return the current "algorithm" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<encryption_algorithm>) algorithm() const;

    ///
    /// Set the "contentionFactor" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(encrypt&) contention_factor(std::int64_t v);

    ///
    /// Return the current "contentionFactor" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<std::int64_t>) contention_factor() const;

    ///
    /// Set the "queryType" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(encrypt&) query_type(encryption_query_type v);

    ///
    /// Return the current "queryType" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<encryption_query_type>) query_type() const;

    ///
    /// Set the "rangeOpts" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(encrypt&) range_opts(v1::range v);

    ///
    /// Return the current "rangeOpts" field.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v1::stdx::optional<range>) range_opts() const;
};

} // namespace v1
} // namespace mongocxx

#include <mongocxx/v1/detail/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v1::encrypt.
///
