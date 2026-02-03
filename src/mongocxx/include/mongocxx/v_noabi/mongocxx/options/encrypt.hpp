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

#include <mongocxx/options/encrypt-fwd.hpp> // IWYU pragma: export

//

#include <bsoncxx/v1/types/value.hpp>

#include <mongocxx/v1/encrypt_options.hpp> // IWYU pragma: export

#include <cstdint>
#include <string>
#include <utility>

#include <mongocxx/client_encryption-fwd.hpp> // IWYU pragma: keep: backward compatibility, to be removed.

#include <bsoncxx/stdx/optional.hpp>
#include <bsoncxx/types.hpp> // IWYU pragma: keep: backward compatibility, to be removed.
#include <bsoncxx/types/bson_value/view.hpp>
#include <bsoncxx/types/bson_value/view_or_value.hpp>

#include <mongocxx/options/range.hpp>
#include <mongocxx/options/text.hpp>

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
    /// Default initialization.
    ///
    encrypt() = default;

    ///
    /// Construct with the @ref mongocxx::v1 equivalent.
    ///
    /* explicit(false) */ MONGOCXX_ABI_EXPORT_CDECL() encrypt(v1::encrypt_options opts);

    ///
    /// Convert to the @ref mongocxx::v1 equivalent.
    ///
    explicit operator v1::encrypt_options() const {
        using bsoncxx::v_noabi::to_v1;
        using mongocxx::v_noabi::to_v1;

        v1::encrypt_options ret;

        if (_key_id) {
            ret.key_id(bsoncxx::v1::types::value{to_v1(*_key_id)});
        }

        if (_key_alt_name) {
            ret.key_alt_name(*_key_alt_name);
        }

        if (_algorithm) {
            ret.algorithm(*_algorithm);
        }

        if (_contention_factor) {
            ret.contention_factor(*_contention_factor);
        }

        if (_query_type) {
            ret.query_type(*_query_type);
        }

        if (_range_opts) {
            ret.range_opts(to_v1(*_range_opts));
        }

        if (_text_opts) {
            ret.text_opts(*_text_opts);
        }

        return ret;
    }

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
    encrypt& key_id(bsoncxx::v_noabi::types::bson_value::view_or_value key_id) {
        _key_id = std::move(key_id);
        return *this;
    }

    ///
    /// Gets the key_id.
    ///
    /// @return
    ///   An optional owning bson_value containing the key_id.
    ///
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::types::bson_value::view_or_value> const& key_id() const {
        return _key_id;
    }

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
    encrypt& key_alt_name(std::string name) {
        _key_alt_name = std::move(name);
        return *this;
    }

    ///
    /// Gets the current key alt name.
    ///
    /// @return
    ///   An optional key name.
    ///
    bsoncxx::v_noabi::stdx::optional<std::string> const& key_alt_name() const {
        return _key_alt_name;
    }

    ///
    /// Determines which AEAD_AES_256_CBC algorithm to use with HMAC_SHA_512 when
    /// encrypting data.
    ///
    using encryption_algorithm = v1::encrypt_options::encryption_algorithm;

    ///
    /// queryType only applies when algorithm is "indexed" or "range".
    /// It is an error to set queryType when algorithm is not "indexed" or "range".
    ///
    using encryption_query_type = v1::encrypt_options::encryption_query_type;

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
    encrypt& algorithm(encryption_algorithm algorithm) {
        _algorithm = algorithm;
        return *this;
    }

    ///
    /// Gets the current algorithm.
    ///
    /// Indexed and Unindexed are used for Queryable Encryption.
    ///
    /// @return
    ///   An optional algorithm.
    ///
    bsoncxx::v_noabi::stdx::optional<encryption_algorithm> const& algorithm() const {
        return _algorithm;
    }

    ///
    /// Sets the contention factor to use for encryption.
    /// contentionFactor only applies when algorithm is "Indexed" or "Range".
    /// It is an error to set contentionFactor when algorithm is not "Indexed".
    ///
    /// @param contention_factor
    ///   An integer specifiying the desired contention factor.
    ///
    encrypt& contention_factor(std::int64_t contention_factor) {
        _contention_factor = contention_factor;
        return *this;
    }

    ///
    /// Gets the current contention factor.
    ///
    /// @return
    ///   An optional contention factor.
    ///
    bsoncxx::v_noabi::stdx::optional<std::int64_t> const& contention_factor() const {
        return _contention_factor;
    }

    ///
    /// Sets the query type to use for encryption.
    ///
    /// @param query_type
    /// One of the following: - equality
    /// query_type only applies when algorithm is "Indexed" or "Range".
    /// It is an error to set query_type when algorithm is not "Indexed" or "Range".
    ///
    encrypt& query_type(encryption_query_type query_type) {
        _query_type = query_type;
        return *this;
    }

    ///
    /// Gets the current query type.
    ///
    /// @return
    ///   A query type.
    ///
    bsoncxx::v_noabi::stdx::optional<encryption_query_type> const& query_type() const {
        return _query_type;
    }

    ///
    /// Sets the range options to use for encryption.
    ///
    encrypt& range_opts(options::range opts) {
        _range_opts = std::move(opts);
        return *this;
    }

    ///
    /// Gets the current range options.
    ///
    /// @return
    ///   An optional range options.
    ///
    bsoncxx::v_noabi::stdx::optional<options::range> const& range_opts() const {
        return _range_opts;
    }

    ///
    /// Sets the text options to use for encryption.
    ///
    encrypt& text_opts(options::text opts) {
        _text_opts = std::move(opts);
        return *this;
    }

    ///
    /// Gets the current text options
    ///
    bsoncxx::v_noabi::stdx::optional<options::text> const& text_opts() const {
        return _text_opts;
    }

    class internal;

   private:
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::types::bson_value::view_or_value> _key_id;
    bsoncxx::v_noabi::stdx::optional<std::string> _key_alt_name;
    bsoncxx::v_noabi::stdx::optional<encryption_algorithm> _algorithm;
    bsoncxx::v_noabi::stdx::optional<std::int64_t> _contention_factor;
    bsoncxx::v_noabi::stdx::optional<encryption_query_type> _query_type;
    bsoncxx::v_noabi::stdx::optional<options::range> _range_opts;
    bsoncxx::v_noabi::stdx::optional<options::text> _text_opts;
};

} // namespace options
} // namespace v_noabi
} // namespace mongocxx

namespace mongocxx {
namespace v_noabi {

///
/// Convert to the @ref mongocxx::v_noabi equivalent of `v`.
///
inline v_noabi::options::encrypt from_v1(v1::encrypt_options v) {
    return {std::move(v)};
}

///
/// Convert to the @ref mongocxx::v1 equivalent of `v`.
///
inline v1::encrypt_options to_v1(v_noabi::options::encrypt const& v) {
    return v1::encrypt_options{v};
}

} // namespace v_noabi
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::options::encrypt.
///
/// @par Includes
/// - @ref mongocxx/v1/encrypt_options.hpp
///
