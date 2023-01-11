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

#include <bsoncxx/types/private/convert.hh>
#include <mongocxx/exception/error_code.hpp>
#include <mongocxx/exception/logic_error.hpp>
#include <mongocxx/options/encrypt.hpp>
#include <mongocxx/private/libbson.hh>
#include <mongocxx/private/libmongoc.hh>

#include <mongocxx/config/private/prelude.hh>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN
namespace options {

encrypt& encrypt::key_id(bsoncxx::types::bson_value::view_or_value key_id) {
    _key_id = std::move(key_id);
    return *this;
}

encrypt& encrypt::key_alt_name(std::string name) {
    _key_alt_name = std::move(name);
    return *this;
}

const stdx::optional<std::string>& encrypt::key_alt_name() const {
    return _key_alt_name;
}

encrypt& encrypt::algorithm(encrypt::encryption_algorithm algorithm) {
    _algorithm = algorithm;
    return *this;
}

const stdx::optional<encrypt::encryption_algorithm>& encrypt::algorithm() const {
    return _algorithm;
}

encrypt& encrypt::contention_factor(int64_t contention_factor) {
    _contention_factor = contention_factor;
    return *this;
}

const stdx::optional<int64_t>& encrypt::contention_factor() const {
    return _contention_factor;
}

encrypt& encrypt::query_type(encrypt::encryption_query_type query_type) {
    _query_type = query_type;
    return *this;
}

const stdx::optional<encrypt::encryption_query_type>& encrypt::query_type() const {
    return _query_type;
}

const stdx::optional<bsoncxx::types::bson_value::view_or_value>& encrypt::key_id() const {
    return _key_id;
}

void* encrypt::convert() const {
    using libbson::scoped_bson_t;

    mongoc_client_encryption_encrypt_opts_t* opts = libmongoc::client_encryption_encrypt_opts_new();

    // libmongoc will error if both key_id and key_alt_name are set, so no need to check here.

    if (_key_id) {
        if (_key_id->view().type() != bsoncxx::type::k_binary) {
            libmongoc::client_encryption_encrypt_opts_destroy(opts);
            throw exception{error_code::k_invalid_parameter, "key id myst be a binary value"};
        }

        auto key_id = _key_id->view().get_binary();

        if (key_id.sub_type != bsoncxx::binary_sub_type::k_uuid) {
            libmongoc::client_encryption_encrypt_opts_destroy(opts);
            throw exception{error_code::k_invalid_parameter,
                            "key id must be a binary value with subtype 4 (UUID)"};
        }

        bson_value_t bson_uuid;
        convert_to_libbson(key_id, &bson_uuid);

        libmongoc::client_encryption_encrypt_opts_set_keyid(opts, &bson_uuid);

        bson_value_destroy(&bson_uuid);
    }

    if (_key_alt_name) {
        libmongoc::client_encryption_encrypt_opts_set_keyaltname(opts, _key_alt_name->c_str());
    }

    if (_algorithm) {
        switch (*_algorithm) {
            case encryption_algorithm::k_deterministic:
                libmongoc::client_encryption_encrypt_opts_set_algorithm(
                    opts, "AEAD_AES_256_CBC_HMAC_SHA_512-Deterministic");
                break;
            case encryption_algorithm::k_random:
                libmongoc::client_encryption_encrypt_opts_set_algorithm(
                    opts, "AEAD_AES_256_CBC_HMAC_SHA_512-Random");
                break;
            case encryption_algorithm::k_indexed:
                libmongoc::client_encryption_encrypt_opts_set_algorithm(
                    opts, MONGOC_ENCRYPT_ALGORITHM_INDEXED);
                break;
            case encryption_algorithm::k_unindexed:
                libmongoc::client_encryption_encrypt_opts_set_algorithm(
                    opts, MONGOC_ENCRYPT_ALGORITHM_UNINDEXED);
                break;
            default:
                libmongoc::client_encryption_encrypt_opts_destroy(opts);
                throw exception{error_code::k_invalid_parameter,
                                "unsupported encryption algorithm"};
        }
    } else {
        // libmongoc will error in this case, encryption algorithm must be set.
    }

    if (_contention_factor) {
        libmongoc::client_encryption_encrypt_opts_set_contention_factor(opts,
                                                                        _contention_factor.value());
    }

    if (_query_type) {
        switch (*_query_type) {
            case encryption_query_type::k_equality:
                libmongoc::client_encryption_encrypt_opts_set_query_type(
                    opts, MONGOC_ENCRYPT_QUERY_TYPE_EQUALITY);
                break;
            default:
                libmongoc::client_encryption_encrypt_opts_destroy(opts);
                throw exception{error_code::k_invalid_parameter, "unsupported query type"};
        }
    }
    return opts;
}

}  // namespace options
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
