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

#include <string>

#include <mongocxx/client_encryption-fwd.hpp>
#include <mongocxx/options/encrypt-fwd.hpp>

#include <bsoncxx/stdx/optional.hpp>
#include <bsoncxx/types.hpp>
#include <bsoncxx/types/bson_value/view_or_value.hpp>

#include <mongocxx/options/range.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
namespace v_noabi {
namespace options {

///
/// Used by @ref mongocxx::v_noabi::client_encryption.
///
class encrypt {
   public:
    ///
    /// Sets the key to use for this encryption operation. A key id can be used instead
    /// of a key alt name.
    ///
    /// If a non-owning bson_value::view is passed in as the key_id, the object that owns
    /// key_id's memory must outlive this object.
    ///
    /// @param key_id
    ///   The id of the key to use for encryption, as a bson_value containing a
    ///   UUID (BSON binary subtype 4).
    ///
    /// @return
    ///   A reference to this object to facilitate method chaining.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/core/security-client-side-encryption/
    ///
    MONGOCXX_ABI_EXPORT_CDECL(encrypt&)
    key_id(bsoncxx::v_noabi::types::bson_value::view_or_value key_id);

    ///
    /// Gets the key_id.
    ///
    /// @return
    ///   An optional owning bson_value containing the key_id.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<
                              bsoncxx::v_noabi::types::bson_value::view_or_value> const&)
    key_id() const;

    ///
    /// Sets a name by which to lookup a key from the key vault collection to use
    /// for this encryption operation. A key alt name can be used instead of a key id.
    ///
    /// @param name
    ///   The name of the key to use for encryption.
    ///
    /// @return
    ///   A reference to this obejct to facilitate method chaining.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/reference/method/getClientEncryption/
    ///
    MONGOCXX_ABI_EXPORT_CDECL(encrypt&) key_alt_name(std::string name);

    ///
    /// Gets the current key alt name.
    ///
    /// @return
    ///   An optional key name.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<std::string> const&)
    key_alt_name() const;

    ///
    /// Determines which AEAD_AES_256_CBC algorithm to use with HMAC_SHA_512 when
    /// encrypting data.
    ///
    enum class encryption_algorithm : std::uint8_t {
        ///
        /// Use deterministic encryption.
        ///
        k_deterministic,

        ///
        /// Use randomized encryption.
        ///
        k_random,

        ///
        /// Use indexed encryption.
        ///
        k_indexed,

        ///
        /// Use unindexed encryption.
        ///
        k_unindexed,

        ///
        /// Use range encryption.
        ///
        k_range,
    };

    ///
    /// queryType only applies when algorithm is "indexed" or "range".
    /// It is an error to set queryType when algorithm is not "indexed" or "range".
    ///
    enum class encryption_query_type : std::uint8_t {
        /// @brief Use query type "equality".
        k_equality,

        /// @brief Use query type "range".
        k_range,
    };

    ///
    /// Sets the algorithm to use for encryption.
    ///
    /// Indexed and Unindexed are used for Queryable Encryption.
    ///
    /// @param algorithm
    ///   An algorithm, either deterministic, random, indexed, or unindexed to use for encryption.
    ///
    /// @note To insert or query with an indexed encrypted payload, use a mongocxx::v_noabi::client
    /// configured with mongocxx::v_noabi::options::auto_encryption.
    /// mongocxx::v_noabi::options::auto_encryption::bypass_query_analysis may be true.
    /// mongocxx::v_noabi::options::auto_encryption::bypass_auto_encryption must be false.
    ///
    /// @see
    /// - https://www.mongodb.com/docs/manual/core/security-client-side-encryption/#encryption-algorithms
    ///
    MONGOCXX_ABI_EXPORT_CDECL(encrypt&) algorithm(encryption_algorithm algorithm);

    ///
    /// Gets the current algorithm.
    ///
    /// Indexed and Unindexed are used for Queryable Encryption.
    ///
    /// @return
    ///   An optional algorithm.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<encryption_algorithm> const&)
    algorithm() const;

    ///
    /// Sets the contention factor to use for encryption.
    /// contentionFactor only applies when algorithm is "Indexed" or "Range".
    /// It is an error to set contentionFactor when algorithm is not "Indexed".
    ///
    /// @param contention_factor
    ///   An integer specifiying the desired contention factor.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(encrypt&) contention_factor(int64_t contention_factor);

    ///
    /// Gets the current contention factor.
    ///
    /// @return
    ///   An optional contention factor.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<int64_t> const&)
    contention_factor() const;

    ///
    /// Sets the query type to use for encryption.
    ///
    /// @param query_type
    /// One of the following: - equality
    /// query_type only applies when algorithm is "Indexed" or "Range".
    /// It is an error to set query_type when algorithm is not "Indexed" or "Range".
    ///
    MONGOCXX_ABI_EXPORT_CDECL(encrypt&) query_type(encryption_query_type query_type);

    ///
    /// Gets the current query type.
    ///
    /// @return
    ///   A query type.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<encryption_query_type> const&)
    query_type() const;

    ///
    /// Sets the range options to use for encryption.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(encrypt&) range_opts(options::range opts);

    ///
    /// Gets the current range options.
    ///
    /// @return
    ///   An optional range options.
    ///
    MONGOCXX_ABI_EXPORT_CDECL(bsoncxx::v_noabi::stdx::optional<options::range> const&)
    range_opts() const;

   private:
    friend ::mongocxx::v_noabi::client_encryption;

    void* convert() const;

    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::types::bson_value::view_or_value> _key_id;
    bsoncxx::v_noabi::stdx::optional<std::string> _key_alt_name;
    bsoncxx::v_noabi::stdx::optional<encryption_algorithm> _algorithm;
    bsoncxx::v_noabi::stdx::optional<int64_t> _contention_factor;
    bsoncxx::v_noabi::stdx::optional<encryption_query_type> _query_type;
    bsoncxx::v_noabi::stdx::optional<options::range> _range_opts;
};

} // namespace options
} // namespace v_noabi
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::options::encrypt.
///
