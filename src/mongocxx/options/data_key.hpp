// Copyright 2020 MongoDB Inc.
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

#include <string>
#include <vector>

#include <bsoncxx/document/view_or_value.hpp>
#include <bsoncxx/stdx/optional.hpp>
#include <mongocxx/stdx.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

class client_encryption;

namespace options {

///
/// Class representing options for data key generation for encryption.
///
class MONGOCXX_API data_key {
   public:
    ///
    /// Sets a KMS-specific key used to encrypt the new data key. If the
    /// kmsProvider is "aws" it is required and has the following fields:
    ///
    ///   - region: String // Required.
    ///
    ///   - key: String // Required. The Amazon Resource Name (ARN) to the
    ///     AWS customer master key (CMK).
    ///
    ///   - endpoint: String // Optional. An alternate host identifier to send
    ///     KMS requests to. May include port number.
    ///
    /// @param master_key
    ///   The document representing the master key.
    ///
    /// @return
    ///   A reference to this object.
    ///
    /// @see https://docs.mongodb.com/manual/core/security-client-side-encryption-key-management/
    ///
    data_key& master_key(bsoncxx::document::view_or_value master_key);

    ///
    /// Gets the master key.
    ///
    /// @return
    ///   An optional document containing the master key.
    ///
    const stdx::optional<bsoncxx::document::view_or_value>& master_key() const;

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
    /// @see https://docs.mongodb.com/manual/reference/method/getClientEncryption/
    ///
    data_key& key_alt_names(std::vector<std::string> key_alt_names);

    ///
    /// Gets the alternate names for the data key.
    ///
    /// @return
    ///   The alternate names for the data key.
    ///
    const std::vector<std::string>& key_alt_names() const;

   private:
    friend class mongocxx::client_encryption;
    MONGOCXX_PRIVATE void* convert() const;

    stdx::optional<bsoncxx::document::view_or_value> _master_key;
    std::vector<std::string> _key_alt_names;
};

}  // namespace options

MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
