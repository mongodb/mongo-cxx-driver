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

#include <mongocxx/options/data_key-fwd.hpp> // IWYU pragma: export

//

#include <bsoncxx/v1/document/value.hpp>

#include <mongocxx/v1/data_key_options.hpp> // IWYU pragma: export

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

#include <mongocxx/client_encryption-fwd.hpp> // IWYU pragma: keep: backward compatibility, to be removed.

#include <bsoncxx/document/view.hpp>
#include <bsoncxx/document/view_or_value.hpp>
#include <bsoncxx/stdx/optional.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
namespace v_noabi {
namespace options {

///
/// Used by @ref mongocxx::v_noabi::client_encryption::create_data_key.
///
class data_key {
   public:
    ///
    /// Default initialization.
    ///
    data_key() = default;

    ///
    /// Construct with the @ref mongocxx::v1 equivalent.
    ///
    /* explicit(false) */ MONGOCXX_ABI_EXPORT_CDECL() data_key(v1::data_key_options key);

    ///
    /// Convert to the @ref mongocxx::v1 equivalent.
    ///
    explicit operator v1::data_key_options() const {
        v1::data_key_options ret;

        if (_master_key) {
            ret.master_key(bsoncxx::v1::document::value{bsoncxx::v_noabi::to_v1(_master_key->view())});
        }

        ret.key_alt_names(_key_alt_names);

        if (_key_material) {
            ret.key_material(*_key_material);
        }

        return ret;
    }

    ///
    /// Sets a KMS-specific key used to encrypt the new data key.
    ///
    /// If the KMS provider is "aws" the masterKey is required and has the following fields:
    ///
    /// ```
    /// {
    ///    region: String,
    ///    key: String, // The Amazon Resource Name (ARN) to the AWS customer master key (CMK).
    ///    endpoint: Optional<String> // An alternate host identifier to send KMS requests to. May
    ///                               // include port number. Defaults to
    ///                               // "kms.<region>.amazonaws.com"
    /// }
    /// ```
    ///
    /// If the KMS provider is "azure" the masterKey is required and has the following fields:
    ///
    /// ```
    /// {
    ///    keyVaultEndpoint: String, // Host with optional port. Example: "example.vault.azure.net".
    ///    keyName: String,
    ///    keyVersion: Optional<String> // A specific version of the named key, defaults to using
    ///                                 // the key's primary version.
    /// }
    /// ```
    ///
    /// If the KMS provider is "gcp" the masterKey is required and has the following fields:
    ///
    /// ```
    /// {
    ///    projectId: String,
    ///    location: String,
    ///    keyRing: String,
    ///    keyName: String,
    ///    keyVersion: Optional<String>, // A specific version of the named key, defaults to using
    ///                                  // the key's primary version.
    ///    endpoint: Optional<String> // Host with optional port. Defaults to
    ///                               // "cloudkms.googleapis.com".
    /// }
    /// ```
    ///
    /// If the KMS provider is "kmip" the masterKey is required and has the following fields:
    ///
    /// ```
    /// {
    ///     keyId: Optional<String>, // keyId is the KMIP Unique Identifier to a 96 byte KMIP Secret
    ///                              // Data managed object.If keyId is omitted, the driver creates
    ///                              // a random 96 byte KMIP Secret Data managed object.
    ///     endpoint: Optional<String>, // Host with optional port.
    ///     delegated: Optional<Boolean> // If true, this key should be decrypted by the KMIP
    ///                                  // server.
    /// }
    /// ```
    ///
    /// If the KMS provider is "local" the masterKey is not applicable.
    ///
    /// @param master_key
    ///   The document representing the master key.
    ///
    /// @return
    ///   A reference to this object.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/core/security-client-side-encryption-key-management/
    ///
    data_key& master_key(bsoncxx::v_noabi::document::view_or_value master_key) {
        _master_key = std::move(master_key);
        return *this;
    }

    ///
    /// Gets the master key.
    ///
    /// @return
    ///   An optional document containing the master key.
    ///
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view_or_value> const& master_key() const {
        return _master_key;
    }

    ///
    /// Sets an optional list of string alternate names used to reference the key.
    /// If a key is created with alternate names, then encryption may refer to the
    /// key by the unique alternate name instead of by _id.
    ///
    /// @param key_alt_names
    ///   The alternate names for the key.
    ///
    /// @return
    ///   A reference to this object.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/method/getClientEncryption/
    ///
    data_key& key_alt_names(std::vector<std::string> key_alt_names) {
        _key_alt_names = std::move(key_alt_names);
        return *this;
    }

    ///
    /// Gets the alternate names for the data key.
    ///
    /// @return
    ///   The alternate names for the data key.
    ///
    std::vector<std::string> const& key_alt_names() const {
        return _key_alt_names;
    }

    ///
    /// Represents binary data used to represent key material.
    ///
    using key_material_type = std::vector<std::uint8_t>;

    ///
    /// Sets the binary data for the key material
    ///
    /// An optional BinData of 96 bytes to use as custom key material for the
    /// data key being created. If keyMaterial is given, the custom key material
    /// is used for encrypting and decrypting data.
    ///
    /// Otherwise, the key material for the new data key is generated from a
    /// cryptographically secure random device.
    ///
    /// @param key_material
    ///   The binary data for the keyMaterial
    ///
    /// @return
    ///   A reference to this object.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/v6.0/reference/method/KeyVault.createKey/
    ///
    data_key& key_material(key_material_type key_material) {
        _key_material = std::move(key_material);
        return *this;
    }

    ///
    /// Gets the keyMaterial as binary data
    ///
    /// @return
    ///   The binary data for the key material
    ///
    /// @see
    /// - https://www.mongodb.com/docs/v6.0/reference/method/KeyVault.createKey/
    ///
    bsoncxx::v_noabi::stdx::optional<key_material_type> const& key_material() {
        return _key_material;
    }

    class internal;

   private:
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view_or_value> _master_key;
    std::vector<std::string> _key_alt_names;
    bsoncxx::v_noabi::stdx::optional<key_material_type> _key_material;
};

} // namespace options
} // namespace v_noabi
} // namespace mongocxx

namespace mongocxx {
namespace v_noabi {

///
/// Convert to the @ref mongocxx::v_noabi equivalent of `v`.
///
inline v_noabi::options::data_key from_v1(v1::data_key_options v) {
    return {std::move(v)};
}

/// Convert to the @ref mongocxx::v1 equivalent of `v`.
///
inline v1::data_key_options to_v1(v_noabi::options::data_key const& v) {
    return v1::data_key_options{v};
}

} // namespace v_noabi
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::options::data_key.
///
/// @par Includes
/// - @ref mongocxx/v1/data_key_options.hpp
///
