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

#include <utility>

#include <mongocxx/exception/error_code.hpp>
#include <mongocxx/exception/logic_error.hpp>
#include <mongocxx/options/encrypt.hpp>

#include <bsoncxx/private/convert.hh>

#include <mongocxx/private/bson.hh>
#include <mongocxx/private/mongoc.hh>
#include <mongocxx/private/scoped_bson_value.hh>

namespace mongocxx {
namespace v_noabi {
namespace options {

encrypt& encrypt::key_id(bsoncxx::v_noabi::types::bson_value::view_or_value key_id) {
    _key_id = std::move(key_id);
    return *this;
}

bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::types::bson_value::view_or_value> const& encrypt::key_id() const {
    return _key_id;
}

encrypt& encrypt::key_alt_name(std::string name) {
    _key_alt_name = std::move(name);
    return *this;
}

bsoncxx::v_noabi::stdx::optional<std::string> const& encrypt::key_alt_name() const {
    return _key_alt_name;
}

encrypt& encrypt::algorithm(encrypt::encryption_algorithm algorithm) {
    _algorithm = algorithm;
    return *this;
}

bsoncxx::v_noabi::stdx::optional<encrypt::encryption_algorithm> const& encrypt::algorithm() const {
    return _algorithm;
}

encrypt& encrypt::contention_factor(int64_t contention_factor) {
    _contention_factor = contention_factor;
    return *this;
}

bsoncxx::v_noabi::stdx::optional<int64_t> const& encrypt::contention_factor() const {
    return _contention_factor;
}

encrypt& encrypt::query_type(encrypt::encryption_query_type query_type) {
    _query_type = query_type;
    return *this;
}

bsoncxx::v_noabi::stdx::optional<encrypt::encryption_query_type> const& encrypt::query_type() const {
    return _query_type;
}

encrypt& encrypt::range_opts(options::range opts) {
    _range_opts = std::move(opts);
    return *this;
}

bsoncxx::v_noabi::stdx::optional<options::range> const& encrypt::range_opts() const {
    return _range_opts;
}

void* encrypt::convert() const {
    using libbson::scoped_bson_t;

    struct encrypt_opts_deleter {
        void operator()(mongoc_client_encryption_encrypt_opts_t* ptr) noexcept {
            libmongoc::client_encryption_encrypt_opts_destroy(ptr);
        }
    };

    auto opts_owner = std::unique_ptr<mongoc_client_encryption_encrypt_opts_t, encrypt_opts_deleter>(
        libmongoc::client_encryption_encrypt_opts_new());
    auto const opts = opts_owner.get();

    // libmongoc will error if both key_id and key_alt_name are set, so no need to check here.

    if (_key_id) {
        if (_key_id->view().type() != bsoncxx::v_noabi::type::k_binary) {
            throw exception{error_code::k_invalid_parameter, "key id myst be a binary value"};
        }

        auto key_id = _key_id->view().get_binary();

        if (key_id.sub_type != bsoncxx::v_noabi::binary_sub_type::k_uuid) {
            throw exception{error_code::k_invalid_parameter, "key id must be a binary value with subtype 4 (UUID)"};
        }

        libmongoc::client_encryption_encrypt_opts_set_keyid(opts, detail::scoped_bson_value(key_id).get());
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
                libmongoc::client_encryption_encrypt_opts_set_algorithm(opts, "AEAD_AES_256_CBC_HMAC_SHA_512-Random");
                break;
            case encryption_algorithm::k_indexed:
                libmongoc::client_encryption_encrypt_opts_set_algorithm(opts, MONGOC_ENCRYPT_ALGORITHM_INDEXED);
                break;
            case encryption_algorithm::k_unindexed:
                libmongoc::client_encryption_encrypt_opts_set_algorithm(opts, MONGOC_ENCRYPT_ALGORITHM_UNINDEXED);
                break;
            case encryption_algorithm::k_range:
                libmongoc::client_encryption_encrypt_opts_set_algorithm(opts, MONGOC_ENCRYPT_ALGORITHM_RANGE);
                break;
            default:
                throw exception{error_code::k_invalid_parameter, "unsupported encryption algorithm"};
        }
    } else {
        // libmongoc will error in this case, encryption algorithm must be set.
    }

    if (_contention_factor) {
        libmongoc::client_encryption_encrypt_opts_set_contention_factor(opts, _contention_factor.value());
    }

    if (_query_type) {
        switch (*_query_type) {
            case encryption_query_type::k_equality:
                libmongoc::client_encryption_encrypt_opts_set_query_type(opts, MONGOC_ENCRYPT_QUERY_TYPE_EQUALITY);
                break;
            case encryption_query_type::k_range:
                libmongoc::client_encryption_encrypt_opts_set_query_type(opts, MONGOC_ENCRYPT_QUERY_TYPE_RANGE);
                break;
            default:
                throw exception{error_code::k_invalid_parameter, "unsupported query type"};
        }
    }

    if (_range_opts) {
        struct range_opts_deleter {
            void operator()(mongoc_client_encryption_encrypt_range_opts_t* ptr) noexcept {
                libmongoc::client_encryption_encrypt_range_opts_destroy(ptr);
            }
        };

        auto range_opts_owner = std::unique_ptr<mongoc_client_encryption_encrypt_range_opts_t, range_opts_deleter>(
            libmongoc::client_encryption_encrypt_range_opts_new());
        auto const range_opts = range_opts_owner.get();

        auto const& min = _range_opts->min();
        auto const& max = _range_opts->max();
        auto const& precision = _range_opts->precision();
        auto const& sparsity = _range_opts->sparsity();
        auto const& trim_factor = _range_opts->trim_factor();

        if (min) {
            libmongoc::client_encryption_encrypt_range_opts_set_min(
                range_opts, detail::scoped_bson_value(min->view()).get());
        }

        if (max) {
            libmongoc::client_encryption_encrypt_range_opts_set_max(
                range_opts, detail::scoped_bson_value(max->view()).get());
        }

        if (precision) {
            libmongoc::client_encryption_encrypt_range_opts_set_precision(range_opts, *precision);
        }

        if (sparsity) {
            libmongoc::client_encryption_encrypt_range_opts_set_sparsity(range_opts, *sparsity);
        }

        if (trim_factor) {
            libmongoc::client_encryption_encrypt_range_opts_set_trim_factor(range_opts, *trim_factor);
        }

        libmongoc::client_encryption_encrypt_opts_set_range_opts(opts, range_opts);
    }

    return opts_owner.release();
}

} // namespace options
} // namespace v_noabi
} // namespace mongocxx
