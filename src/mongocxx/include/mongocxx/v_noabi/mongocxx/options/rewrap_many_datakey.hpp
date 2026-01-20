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

#include <bsoncxx/v1/document/value.hpp>

#include <mongocxx/options/rewrap_many_datakey-fwd.hpp> // IWYU pragma: export

///

#include <mongocxx/v1/rewrap_many_datakey_options.hpp> // IWYU pragma: export

#include <string>
#include <utility>

#include <mongocxx/client_encryption-fwd.hpp> // IWYU pragma: keep: backward compatibility, to be removed.

#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/document/view_or_value.hpp>
#include <bsoncxx/stdx/optional.hpp>
#include <bsoncxx/string/view_or_value.hpp>
#include <bsoncxx/types.hpp>                          // IWYU pragma: keep: backward compatibility, to be removed.
#include <bsoncxx/types/bson_value/view_or_value.hpp> // IWYU pragma: keep: backward compatibility, to be removed.

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
namespace v_noabi {
namespace options {

///
/// Used by @ref mongocxx::v_noabi::client_encryption::rewrap_many_datakey.
///
class rewrap_many_datakey {
   public:
    ///
    /// Default initialization.
    ///
    rewrap_many_datakey() = default;

    ///
    /// Construct with the @ref mongocxx::v1 equivalent.
    ///
    /* explicit(false) */ MONGOCXX_ABI_EXPORT_CDECL() rewrap_many_datakey(v1::rewrap_many_datakey_options opts);

    ///
    /// Convert to the @ref mongocxx::v1 equivalent.
    ///
    explicit operator v1::rewrap_many_datakey_options() const {
        v1::rewrap_many_datakey_options ret;

        ret.provider(std::string{_provider.view()});

        if (_master_key) {
            ret.master_key(bsoncxx::v1::document::value{bsoncxx::v_noabi::to_v1(_master_key->view())});
        }

        return ret;
    }

    ///
    /// Set the optional KMS provider use to encrypt the data keys. Do not set to use the current
    /// KMS provider(s).
    ///
    /// A KMS provider (AWS KMS, Azure Key Vault, GCP KMS, the local provider,
    /// or KMIP) is used to decrypt data keys after fetching from the MongoDB
    /// Key Vault, and encrypt newly created data keys.
    ///
    /// @param provider String name of the provider.
    ///
    /// @return
    ///   An optional document containing the TLS options.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/core/csfle/reference/kms-providers/#std-label-csfle-reference-kms-providers
    ///
    rewrap_many_datakey& provider(bsoncxx::v_noabi::string::view_or_value provider) {
        _provider = std::move(provider);
        return *this;
    }

    ///
    /// Get the KMS provider
    ///
    /// A KMS provider (AWS KMS, Azure Key Vault, GCP KMS, the local provider,
    /// or KMIP) is used to decrypt data keys after fetching from the MongoDB
    /// Key Vault, and encrypt newly created data keys.
    ///
    /// @return
    ///   An optional string name of the provider.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/core/csfle/reference/kms-providers/#std-label-csfle-reference-kms-providers
    ///
    bsoncxx::v_noabi::string::view_or_value provider() const {
        return _provider;
    }

    ///
    /// Set the masterKey option.
    ///
    /// The masterKey document MUST have the fields corresponding to the given
    /// provider as specified in masterKey. masterKey MUST NOT be given if it is
    /// not applicable for the given provider.
    ///
    /// @param master_key A document of the master key.
    ///
    /// @return
    ///   A reference to this object to facilitate method chaining.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/core/csfle/reference/kms-providers/#std-label-csfle-reference-kms-providers-create-and-store
    ///
    rewrap_many_datakey& master_key(bsoncxx::v_noabi::document::view_or_value master_key) {
        _master_key = std::move(master_key);
        return *this;
    }

    ///
    /// Get the masterKey option.
    ///
    /// The masterKey document MUST have the fields corresponding to the given
    /// provider as specified in masterKey. masterKey MUST NOT be given if it is
    /// not applicable for the given provider.
    ///
    /// @return
    ///   A reference to this object to facilitate method chaining.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/core/csfle/reference/kms-providers/#std-label-csfle-reference-kms-providers-create-and-store
    ///
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view_or_value> const& master_key() const {
        return _master_key;
    }

   private:
    bsoncxx::v_noabi::string::view_or_value _provider;
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view_or_value> _master_key;
};

} // namespace options
} // namespace v_noabi
} // namespace mongocxx

namespace mongocxx {
namespace v_noabi {

///
/// Convert to the @ref mongocxx::v_noabi equivalent of `v`.
///
inline v_noabi::options::rewrap_many_datakey from_v1(v1::rewrap_many_datakey_options v) {
    return {std::move(v)};
}

///
/// Convert to the @ref mongocxx::v1 equivalent of `v`.
///
inline v1::rewrap_many_datakey_options to_v1(v_noabi::options::rewrap_many_datakey const& v) {
    return v1::rewrap_many_datakey_options{v};
}

} // namespace v_noabi
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::options::rewrap_many_datakey.
///
/// @par Includes
/// - @ref mongocxx/v1/rewrap_many_datakey_options.hpp
///
