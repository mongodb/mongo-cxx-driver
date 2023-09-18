// Copyright 2023 MongoDB Inc.
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

#include <bsoncxx/stdx/optional.hpp>
#include <bsoncxx/string/view_or_value.hpp>
#include <bsoncxx/types.hpp>
#include <bsoncxx/types/bson_value/view_or_value.hpp>
#include <mongocxx/stdx.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

class client_encryption;

namespace options {

class MONGOCXX_API rewrap_many_datakey {
   public:
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
    /// https://www.mongodb.com/docs/manual/core/csfle/reference/kms-providers/#std-label-csfle-reference-kms-providers
    ///
    rewrap_many_datakey& provider(bsoncxx::string::view_or_value provider);

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
    /// https://www.mongodb.com/docs/manual/core/csfle/reference/kms-providers/#std-label-csfle-reference-kms-providers
    ///
    bsoncxx::string::view_or_value provider() const;

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
    /// https://www.mongodb.com/docs/manual/core/csfle/reference/kms-providers/#std-label-csfle-reference-kms-providers-create-and-store
    ///
    rewrap_many_datakey& master_key(bsoncxx::document::view_or_value master_key);

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
    /// https://www.mongodb.com/docs/manual/core/csfle/reference/kms-providers/#std-label-csfle-reference-kms-providers-create-and-store
    ///
    const stdx::optional<bsoncxx::document::view_or_value>& master_key() const;

   private:
    friend class mongocxx::client_encryption;
    bsoncxx::string::view_or_value _provider;
    stdx::optional<bsoncxx::document::view_or_value> _master_key;
};

}  // namespace options
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx

#include <mongocxx/config/postlude.hpp>
