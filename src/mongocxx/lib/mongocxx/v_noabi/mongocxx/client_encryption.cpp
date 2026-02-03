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

#include <mongocxx/v1/text_options.hh>

#include <mongocxx/client_encryption.hpp>
#include <mongocxx/exception/error_code.hpp>
#include <mongocxx/exception/exception.hpp>

#include <mongocxx/client_encryption.hh>
#include <mongocxx/database.hh>

#include <bsoncxx/private/make_unique.hh>

namespace mongocxx {
namespace v_noabi {

client_encryption::impl::encrypt_opts_ptr_type client_encryption::impl::to_mongoc(options::encrypt const& opts) {
    struct encrypt_opts_deleter {
        void operator()(mongoc_client_encryption_encrypt_opts_t* ptr) noexcept {
            libmongoc::client_encryption_encrypt_opts_destroy(ptr);
        }
    };

    auto ret = encrypt_opts_ptr_type{libmongoc::client_encryption_encrypt_opts_new()};
    auto const ptr = ret.get();

    // libmongoc will error if both key_id and key_alt_name are set, so no need to check here.

    if (auto const& opt = opts.key_id()) {
        if (opt->view().type() != bsoncxx::v_noabi::type::k_binary) {
            throw exception{error_code::k_invalid_parameter, "key id myst be a binary value"};
        }

        auto key_id = opt->view().get_binary();

        if (key_id.sub_type != bsoncxx::v_noabi::binary_sub_type::k_uuid) {
            throw exception{error_code::k_invalid_parameter, "key id must be a binary value with subtype 4 (UUID)"};
        }

        libmongoc::client_encryption_encrypt_opts_set_keyid(ptr, detail::scoped_bson_value(key_id).get());
    }

    if (auto const& opt = opts.key_alt_name()) {
        libmongoc::client_encryption_encrypt_opts_set_keyaltname(ptr, opt->c_str());
    }

    if (auto const& opt = opts.algorithm()) {
        switch (*opt) {
            case options::encrypt::encryption_algorithm::k_deterministic:
                libmongoc::client_encryption_encrypt_opts_set_algorithm(
                    ptr, "AEAD_AES_256_CBC_HMAC_SHA_512-Deterministic");
                break;
            case options::encrypt::encryption_algorithm::k_random:
                libmongoc::client_encryption_encrypt_opts_set_algorithm(ptr, "AEAD_AES_256_CBC_HMAC_SHA_512-Random");
                break;
            case options::encrypt::encryption_algorithm::k_indexed:
                libmongoc::client_encryption_encrypt_opts_set_algorithm(ptr, MONGOC_ENCRYPT_ALGORITHM_INDEXED);
                break;
            case options::encrypt::encryption_algorithm::k_unindexed:
                libmongoc::client_encryption_encrypt_opts_set_algorithm(ptr, MONGOC_ENCRYPT_ALGORITHM_UNINDEXED);
                break;
            case options::encrypt::encryption_algorithm::k_range:
                libmongoc::client_encryption_encrypt_opts_set_algorithm(ptr, MONGOC_ENCRYPT_ALGORITHM_RANGE);
                break;
            case options::encrypt::encryption_algorithm::k_textPreview:
                libmongoc::client_encryption_encrypt_opts_set_algorithm(ptr, MONGOC_ENCRYPT_ALGORITHM_TEXTPREVIEW);
                break;
            default:
                throw exception{error_code::k_invalid_parameter, "unsupported encryption algorithm"};
        }
    } else {
        // libmongoc will error in this case, encryption algorithm must be set.
    }

    if (auto const& opt = opts.contention_factor()) {
        libmongoc::client_encryption_encrypt_opts_set_contention_factor(ptr, *opt);
    }

    if (auto const& opt = opts.query_type()) {
        switch (*opt) {
            case options::encrypt::encryption_query_type::k_equality:
                libmongoc::client_encryption_encrypt_opts_set_query_type(ptr, MONGOC_ENCRYPT_QUERY_TYPE_EQUALITY);
                break;
            case options::encrypt::encryption_query_type::k_range:
                libmongoc::client_encryption_encrypt_opts_set_query_type(ptr, MONGOC_ENCRYPT_QUERY_TYPE_RANGE);
                break;
            case options::encrypt::encryption_query_type::k_prefixPreview:
                libmongoc::client_encryption_encrypt_opts_set_query_type(ptr, MONGOC_ENCRYPT_QUERY_TYPE_PREFIXPREVIEW);
                break;
            case options::encrypt::encryption_query_type::k_suffixPreview:
                libmongoc::client_encryption_encrypt_opts_set_query_type(ptr, MONGOC_ENCRYPT_QUERY_TYPE_SUFFIXPREVIEW);
                break;
            case options::encrypt::encryption_query_type::k_substringPreview:
                libmongoc::client_encryption_encrypt_opts_set_query_type(
                    ptr, MONGOC_ENCRYPT_QUERY_TYPE_SUBSTRINGPREVIEW);
                break;
            default:
                throw exception{error_code::k_invalid_parameter, "unsupported query type"};
        }
    }

    if (auto const& opt = opts.range_opts()) {
        struct range_opts_deleter {
            void operator()(mongoc_client_encryption_encrypt_range_opts_t* ptr) noexcept {
                libmongoc::client_encryption_encrypt_range_opts_destroy(ptr);
            }
        };

        auto range_opts_owner = std::unique_ptr<mongoc_client_encryption_encrypt_range_opts_t, range_opts_deleter>(
            libmongoc::client_encryption_encrypt_range_opts_new());
        auto const range_opts = range_opts_owner.get();

        auto const& min = opt->min();
        auto const& max = opt->max();
        auto const& precision = opt->precision();
        auto const& sparsity = opt->sparsity();
        auto const& trim_factor = opt->trim_factor();

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

        libmongoc::client_encryption_encrypt_opts_set_range_opts(ptr, range_opts);
    }

    if (auto const& opt = opts.text_opts()) {
        struct text_opts_deleter {
            void operator()(mongoc_client_encryption_encrypt_text_opts_t* ptr) noexcept {
                libmongoc::client_encryption_encrypt_text_opts_destroy(ptr);
            }
        };

        auto text_opts_owner = std::unique_ptr<mongoc_client_encryption_encrypt_text_opts_t, text_opts_deleter>(
            libmongoc::client_encryption_encrypt_text_opts_new());
        auto const text_opts = text_opts_owner.get();

        auto const& case_sensitive = opt->case_sensitive();
        auto const& diacritic_sensitive = opt->diacritic_sensitive();
        auto prefix = v1::text_options::internal::prefix_opts(*opt);
        auto suffix = v1::text_options::internal::suffix_opts(*opt);
        auto substring = v1::text_options::internal::substring_opts(*opt);

        if (case_sensitive) {
            libmongoc::client_encryption_encrypt_text_opts_set_case_sensitive(text_opts, case_sensitive.value());
        }

        if (diacritic_sensitive) {
            libmongoc::client_encryption_encrypt_text_opts_set_diacritic_sensitive(
                text_opts, diacritic_sensitive.value());
        }

        if (prefix) {
            struct prefix_opts_deleter {
                void operator()(mongoc_client_encryption_encrypt_text_prefix_opts_t* ptr) noexcept {
                    libmongoc::client_encryption_encrypt_text_prefix_opts_destroy(ptr);
                }
            };

            auto prefix_opts_owner =
                std::unique_ptr<mongoc_client_encryption_encrypt_text_prefix_opts_t, prefix_opts_deleter>(
                    libmongoc::client_encryption_encrypt_text_prefix_opts_new());
            auto const prefix_opts = prefix_opts_owner.get();

            auto const& str_max_query_length = prefix->str_max_query_length();
            auto const& str_min_query_length = prefix->str_min_query_length();

            if (str_max_query_length) {
                libmongoc::client_encryption_encrypt_text_prefix_opts_set_str_max_query_length(
                    prefix_opts, str_max_query_length.value());
            }

            if (str_min_query_length) {
                libmongoc::client_encryption_encrypt_text_prefix_opts_set_str_min_query_length(
                    prefix_opts, str_min_query_length.value());
            }

            libmongoc::client_encryption_encrypt_text_opts_set_prefix(text_opts, prefix_opts);
        }

        if (suffix) {
            struct suffix_opts_deleter {
                void operator()(mongoc_client_encryption_encrypt_text_suffix_opts_t* ptr) noexcept {
                    libmongoc::client_encryption_encrypt_text_suffix_opts_destroy(ptr);
                }
            };

            auto suffix_opts_owner =
                std::unique_ptr<mongoc_client_encryption_encrypt_text_suffix_opts_t, suffix_opts_deleter>(
                    libmongoc::client_encryption_encrypt_text_suffix_opts_new());
            auto const suffix_opts = suffix_opts_owner.get();

            auto const& str_max_query_length = suffix->str_max_query_length();
            auto const& str_min_query_length = suffix->str_min_query_length();

            if (str_max_query_length) {
                libmongoc::client_encryption_encrypt_text_suffix_opts_set_str_max_query_length(
                    suffix_opts, str_max_query_length.value());
            }

            if (str_min_query_length) {
                libmongoc::client_encryption_encrypt_text_suffix_opts_set_str_min_query_length(
                    suffix_opts, str_min_query_length.value());
            }

            libmongoc::client_encryption_encrypt_text_opts_set_suffix(text_opts, suffix_opts);
        }

        if (substring) {
            struct substring_opts_deleter {
                void operator()(mongoc_client_encryption_encrypt_text_substring_opts_t* ptr) noexcept {
                    libmongoc::client_encryption_encrypt_text_substring_opts_destroy(ptr);
                }
            };

            auto substring_opts_owner =
                std::unique_ptr<mongoc_client_encryption_encrypt_text_substring_opts_t, substring_opts_deleter>(
                    libmongoc::client_encryption_encrypt_text_substring_opts_new());
            auto const substring_opts = substring_opts_owner.get();

            auto const& str_max_query_length = substring->str_max_query_length();
            auto const& str_min_query_length = substring->str_min_query_length();
            auto const& str_max_length = substring->str_max_length();

            if (str_max_query_length) {
                libmongoc::client_encryption_encrypt_text_substring_opts_set_str_max_query_length(
                    substring_opts, str_max_query_length.value());
            }

            if (str_min_query_length) {
                libmongoc::client_encryption_encrypt_text_substring_opts_set_str_min_query_length(
                    substring_opts, str_min_query_length.value());
            }

            if (str_max_length) {
                libmongoc::client_encryption_encrypt_text_substring_opts_set_str_max_length(
                    substring_opts, str_max_length.value());
            }

            libmongoc::client_encryption_encrypt_text_opts_set_substring(text_opts, substring_opts);
        }

        libmongoc::client_encryption_encrypt_opts_set_text_opts(ptr, text_opts);
    }

    return ret;
}

client_encryption::client_encryption(options::client_encryption opts)
    : _impl(bsoncxx::make_unique<impl>(std::move(opts))) {}

client_encryption::~client_encryption() = default;
client_encryption::client_encryption(client_encryption&&) noexcept = default;
client_encryption& client_encryption::operator=(client_encryption&&) noexcept = default;

bsoncxx::v_noabi::types::bson_value::value client_encryption::create_data_key(
    std::string kms_provider,
    options::data_key const& opts) {
    return _impl->create_data_key(kms_provider, opts);
}

bsoncxx::v_noabi::types::bson_value::value client_encryption::encrypt(
    bsoncxx::v_noabi::types::bson_value::view value,
    options::encrypt const& opts) {
    return _impl->encrypt(value, opts);
}

bsoncxx::v_noabi::document::value client_encryption::encrypt_expression(
    bsoncxx::v_noabi::document::view_or_value expr,
    options::encrypt const& opts) {
    return _impl->encrypt_expression(expr, opts);
}

bsoncxx::v_noabi::types::bson_value::value client_encryption::decrypt(bsoncxx::v_noabi::types::bson_value::view value) {
    return _impl->decrypt(value);
}

collection client_encryption::create_encrypted_collection(
    database const& db,
    std::string const& coll_name,
    bsoncxx::v_noabi::document::view const& options,
    bsoncxx::v_noabi::document::value& out_options,
    std::string const& kms_provider,
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view> const& masterkey) {
    // Backward compatibility: `create_encrypted_collection()` is not logically const.
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
    auto& d = const_cast<v_noabi::database&>(db);

    return _impl->create_encrypted_collection(
        d, v_noabi::database::internal::as_mongoc(d), coll_name, options, out_options, kms_provider, masterkey);
}

result::rewrap_many_datakey client_encryption::rewrap_many_datakey(
    bsoncxx::v_noabi::document::view_or_value filter,
    options::rewrap_many_datakey const& opts) {
    return _impl->rewrap_many_datakey(filter, opts);
}

result::delete_result client_encryption::delete_key(bsoncxx::v_noabi::types::bson_value::view_or_value id) {
    return _impl->delete_key(id);
}

bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::value> client_encryption::get_key(
    bsoncxx::v_noabi::types::bson_value::view_or_value id) {
    return _impl->get_key(id);
}

mongocxx::v_noabi::cursor client_encryption::get_keys() {
    return _impl->get_keys();
}

bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::value> client_encryption::add_key_alt_name(
    bsoncxx::v_noabi::types::bson_value::view_or_value id,
    bsoncxx::v_noabi::string::view_or_value key_alt_name) {
    return _impl->add_key_alt_name(id, key_alt_name);
}

bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::value> client_encryption::get_key_by_alt_name(
    bsoncxx::v_noabi::string::view_or_value key_alt_name) {
    return _impl->get_key_by_alt_name(key_alt_name);
}

bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::value> client_encryption::remove_key_alt_name(
    bsoncxx::v_noabi::types::bson_value::view_or_value id,
    bsoncxx::v_noabi::string::view_or_value key_alt_name) {
    return _impl->remove_key_alt_name(id, key_alt_name);
}

} // namespace v_noabi
} // namespace mongocxx
