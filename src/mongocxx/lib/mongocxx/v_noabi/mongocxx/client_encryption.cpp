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

#include <mongocxx/client_encryption.hpp>

//

#include <bsoncxx/v1/document/value.hpp>

#include <mongocxx/v1/exception.hpp>

#include <bsoncxx/v1/types/value.hh>

#include <mongocxx/v1/bulk_write.hh>
#include <mongocxx/v1/collection.hh>
#include <mongocxx/v1/rewrap_many_datakey_result.hh>

#include <memory>
#include <string>
#include <utility>

#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/document/view_or_value.hpp>
#include <bsoncxx/stdx/optional.hpp>
#include <bsoncxx/string/view_or_value.hpp>
#include <bsoncxx/types/bson_value/value.hpp>
#include <bsoncxx/types/bson_value/view.hpp>
#include <bsoncxx/types/bson_value/view_or_value.hpp>

#include <mongocxx/cursor.hpp>
#include <mongocxx/exception/error_code.hpp>
#include <mongocxx/exception/exception.hpp>
#include <mongocxx/exception/operation_exception.hpp>
#include <mongocxx/options/encrypt.hpp>
#include <mongocxx/options/rewrap_many_datakey.hpp>
#include <mongocxx/result/delete.hpp>
#include <mongocxx/result/rewrap_many_datakey.hpp>

#include <mongocxx/collection.hh>
#include <mongocxx/database.hh>
#include <mongocxx/mongoc_error.hh>
#include <mongocxx/options/client_encryption.hh>
#include <mongocxx/options/data_key.hh>
#include <mongocxx/scoped_bson.hh>

#include <bsoncxx/private/bson.hh>

#include <mongocxx/private/mongoc.hh>

namespace mongocxx {
namespace v_noabi {

namespace {

bson_value_t const& get_bson_value(bsoncxx::v1::types::value const& value) {
    return bsoncxx::v1::types::value::internal::get_bson_value(value);
}

bson_value_t& get_bson_value(bsoncxx::v1::types::value& value) {
    return bsoncxx::v1::types::value::internal::get_bson_value(value);
}

struct encrypt_opts_deleter {
    void operator()(mongoc_client_encryption_encrypt_opts_t* ptr) noexcept {
        libmongoc::client_encryption_encrypt_opts_destroy(ptr);
    }
};

using encrypt_opts_ptr_type = std::unique_ptr<mongoc_client_encryption_encrypt_opts_t, encrypt_opts_deleter>;

encrypt_opts_ptr_type to_mongoc(v_noabi::options::encrypt const& opts) {
    auto ret = encrypt_opts_ptr_type{libmongoc::client_encryption_encrypt_opts_new()};
    auto const ptr = ret.get();

    // libmongoc will error if both key_id and key_alt_name are set, so no need to check here.

    if (auto const& opt = opts.key_id()) {
        libmongoc::client_encryption_encrypt_opts_set_keyid(
            ptr, &get_bson_value(bsoncxx::v1::types::value{bsoncxx::v_noabi::to_v1(opt->view())}));
    }

    if (auto const& opt = opts.key_alt_name()) {
        libmongoc::client_encryption_encrypt_opts_set_keyaltname(ptr, opt->c_str());
    }

    // When no valid encryption algorithm is given, mongoc will return an error.
    if (auto const& opt = opts.algorithm()) {
        switch (*opt) {
            case v_noabi::options::encrypt::encryption_algorithm::k_deterministic:
                libmongoc::client_encryption_encrypt_opts_set_algorithm(
                    ptr, MONGOC_AEAD_AES_256_CBC_HMAC_SHA_512_DETERMINISTIC);
                break;
            case v_noabi::options::encrypt::encryption_algorithm::k_random:
                libmongoc::client_encryption_encrypt_opts_set_algorithm(
                    ptr, MONGOC_AEAD_AES_256_CBC_HMAC_SHA_512_RANDOM);
                break;
            case v_noabi::options::encrypt::encryption_algorithm::k_indexed:
                libmongoc::client_encryption_encrypt_opts_set_algorithm(ptr, MONGOC_ENCRYPT_ALGORITHM_INDEXED);
                break;
            case v_noabi::options::encrypt::encryption_algorithm::k_unindexed:
                libmongoc::client_encryption_encrypt_opts_set_algorithm(ptr, MONGOC_ENCRYPT_ALGORITHM_UNINDEXED);
                break;
            case v_noabi::options::encrypt::encryption_algorithm::k_range:
                libmongoc::client_encryption_encrypt_opts_set_algorithm(ptr, MONGOC_ENCRYPT_ALGORITHM_RANGE);
                break;
            case v_noabi::options::encrypt::encryption_algorithm::k_textPreview:
                libmongoc::client_encryption_encrypt_opts_set_algorithm(ptr, MONGOC_ENCRYPT_ALGORITHM_TEXTPREVIEW);
                break;
            default:
                throw v_noabi::exception{v_noabi::error_code::k_invalid_parameter, "unsupported encryption algorithm"};
        }
    }

    if (auto const& opt = opts.contention_factor()) {
        libmongoc::client_encryption_encrypt_opts_set_contention_factor(ptr, *opt);
    }

    if (auto const& opt = opts.query_type()) {
        switch (*opt) {
            case v_noabi::options::encrypt::encryption_query_type::k_equality:
                libmongoc::client_encryption_encrypt_opts_set_query_type(ptr, MONGOC_ENCRYPT_QUERY_TYPE_EQUALITY);
                break;
            case v_noabi::options::encrypt::encryption_query_type::k_range:
                libmongoc::client_encryption_encrypt_opts_set_query_type(ptr, MONGOC_ENCRYPT_QUERY_TYPE_RANGE);
                break;
            case v_noabi::options::encrypt::encryption_query_type::k_prefixPreview:
                libmongoc::client_encryption_encrypt_opts_set_query_type(ptr, MONGOC_ENCRYPT_QUERY_TYPE_PREFIXPREVIEW);
                break;
            case v_noabi::options::encrypt::encryption_query_type::k_suffixPreview:
                libmongoc::client_encryption_encrypt_opts_set_query_type(ptr, MONGOC_ENCRYPT_QUERY_TYPE_SUFFIXPREVIEW);
                break;
            case v_noabi::options::encrypt::encryption_query_type::k_substringPreview:
                libmongoc::client_encryption_encrypt_opts_set_query_type(
                    ptr, MONGOC_ENCRYPT_QUERY_TYPE_SUBSTRINGPREVIEW);
                break;
            default:
                throw v_noabi::exception{v_noabi::error_code::k_invalid_parameter, "unsupported query type"};
        }
    }

    if (auto const& opt = opts.range_opts()) {
        struct range_opts_deleter {
            void operator()(mongoc_client_encryption_encrypt_range_opts_t* ptr) noexcept {
                libmongoc::client_encryption_encrypt_range_opts_destroy(ptr);
            }
        };

        auto const range_opts_owner =
            std::unique_ptr<mongoc_client_encryption_encrypt_range_opts_t, range_opts_deleter>(
                libmongoc::client_encryption_encrypt_range_opts_new());
        auto const range_opts = range_opts_owner.get();

        auto const& min = opt->min();
        auto const& max = opt->max();
        auto const& precision = opt->precision();
        auto const& sparsity = opt->sparsity();
        auto const& trim_factor = opt->trim_factor();

        if (min) {
            libmongoc::client_encryption_encrypt_range_opts_set_min(
                range_opts, &get_bson_value(bsoncxx::v1::types::value{bsoncxx::v_noabi::to_v1(min->view())}));
        }

        if (max) {
            libmongoc::client_encryption_encrypt_range_opts_set_max(
                range_opts, &get_bson_value(bsoncxx::v1::types::value{bsoncxx::v_noabi::to_v1(max->view())}));
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

        auto const text_opts_owner = std::unique_ptr<mongoc_client_encryption_encrypt_text_opts_t, text_opts_deleter>(
            libmongoc::client_encryption_encrypt_text_opts_new());
        auto const text_opts = text_opts_owner.get();

        auto const& case_sensitive = opt->case_sensitive();
        auto const& diacritic_sensitive = opt->diacritic_sensitive();
        auto const& prefix = opt->prefix_opts();
        auto const& suffix = opt->suffix_opts();
        auto const& substring = opt->substring_opts();

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

            auto const prefix_opts_owner =
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

            auto const suffix_opts_owner =
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

            auto const substring_opts_owner =
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

} // namespace

client_encryption::client_encryption(v_noabi::options::client_encryption opts)
    : _ce{[&] {
          bson_error_t error = {};

          if (auto const ptr = libmongoc::client_encryption_new(
                  options::client_encryption::internal::to_mongoc(opts).get(), &error)) {
              return v1::client_encryption::internal::make(ptr);
          }

          v_noabi::throw_exception<v_noabi::operation_exception>(error);
      }()} {}

bsoncxx::v_noabi::types::value client_encryption::create_data_key(
    std::string kms_provider,
    v_noabi::options::data_key const& opts) {
    bsoncxx::v1::types::value keyid;
    bson_error_t error = {};

    if (!libmongoc::client_encryption_create_datakey(
            v1::client_encryption::internal::as_mongoc(_ce),
            std::string{kms_provider}.c_str(),
            v_noabi::options::data_key::internal::to_mongoc(opts).get(),
            &get_bson_value(keyid),
            &error)) {
        v_noabi::throw_exception<v_noabi::operation_exception>(error);
    }

    return bsoncxx::v1::types::value{std::move(keyid)};
}

namespace {

v1::collection create_encrypted_collection_impl(
    mongoc_client_encryption_t* ce,
    mongoc_client_t* client,
    mongoc_database_t* db,
    char const* name,
    bson_t const* opts,
    bsoncxx::v1::document::value& coll_opts,
    char const* kms_provider,
    bson_t const* masterkey) {
    scoped_bson out;
    bson_error_t error = {};

    if (auto const ptr = libmongoc::client_encryption_create_encrypted_collection(
            ce, db, name, opts, out.out_ptr(), kms_provider, masterkey, &error)) {
        coll_opts = std::move(out).value();
        return v1::collection::internal::make(ptr, client);
    }

    v_noabi::throw_exception<v_noabi::operation_exception>(error);
}

} // namespace

v_noabi::collection client_encryption::create_encrypted_collection(
    v_noabi::database const& db,
    std::string const& coll_name,
    bsoncxx::v_noabi::document::view const& options,
    bsoncxx::v_noabi::document::value& out_options,
    std::string const& kms_provider,
    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view> const& masterkey) {
    // Backward compatibility: `create_encrypted_collection()` is not logically const.
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
    auto& d = const_cast<v_noabi::database&>(db);

    bsoncxx::v1::document::value coll_opts;

    auto ret = create_encrypted_collection_impl(
        v1::client_encryption::internal::as_mongoc(_ce),
        v_noabi::database::internal::get_client(d),
        v_noabi::database::internal::as_mongoc(d),
        coll_name.c_str(),
        to_scoped_bson_view(options).bson(),
        coll_opts,
        kms_provider.c_str(),
        masterkey ? to_scoped_bson_view(*masterkey).bson() : nullptr);

    out_options = bsoncxx::v_noabi::from_v1(std::move(coll_opts));

    return v_noabi::from_v1(std::move(ret));
}

namespace {

// CSFLE API requires empty strings to be not-null.
bsoncxx::v1::types::value ensure_not_null_string(bsoncxx::v1::types::value value) {
    auto& v = bsoncxx::v1::types::value::internal::get_bson_value(value);

    if (v.value_type == BSON_TYPE_UTF8 && v.value.v_utf8.str == nullptr) {
        v.value.v_utf8.str = static_cast<char*>(bson_malloc0(1u));
    }

    return value;
}

} // namespace

bsoncxx::v_noabi::types::value client_encryption::encrypt(
    bsoncxx::v_noabi::types::bson_value::view value,
    v_noabi::options::encrypt const& opts) {
    bsoncxx::v1::types::value ciphertext;
    bson_error_t error = {};

    if (!libmongoc::client_encryption_encrypt(
            v1::client_encryption::internal::as_mongoc(_ce),
            &get_bson_value(ensure_not_null_string(bsoncxx::v1::types::value{bsoncxx::v_noabi::to_v1(value)})),
            to_mongoc(opts).get(),
            &get_bson_value(ciphertext),
            &error)) {
        v_noabi::throw_exception<v_noabi::operation_exception>(error);
    }

    return ciphertext;
}

bsoncxx::v_noabi::document::value client_encryption::encrypt_expression(
    bsoncxx::v_noabi::document::view_or_value expr,
    v_noabi::options::encrypt const& opts) {
    auto const encrypt_opts = to_mongoc(opts);

    bson_error_t error = {};

    scoped_bson encrypted;
    if (!libmongoc::client_encryption_encrypt_expression(
            v1::client_encryption::internal::as_mongoc(_ce),
            to_scoped_bson_view(expr).bson(),
            encrypt_opts.get(),
            encrypted.out_ptr(),
            &error)) {
        v_noabi::throw_exception<v_noabi::operation_exception>(error);
    }

    return bsoncxx::v_noabi::from_v1(std::move(encrypted).value());
}

bsoncxx::v_noabi::types::value client_encryption::decrypt(bsoncxx::v_noabi::types::bson_value::view value) try {
    return _ce.decrypt(bsoncxx::v1::types::value{bsoncxx::v_noabi::to_v1(value)});
} catch (v1::exception const& ex) {
    v_noabi::throw_exception<v_noabi::operation_exception>(ex);
}

v_noabi::result::rewrap_many_datakey client_encryption::rewrap_many_datakey(
    bsoncxx::v_noabi::document::view_or_value filter,
    v_noabi::options::rewrap_many_datakey const& opts) {
    struct result_deleter {
        void operator()(mongoc_client_encryption_rewrap_many_datakey_result_t* ptr) noexcept {
            libmongoc::client_encryption_rewrap_many_datakey_result_destroy(ptr);
        }
    };

    using result_ptr = std::unique_ptr<mongoc_client_encryption_rewrap_many_datakey_result_t, result_deleter>;

    auto const result_owner = result_ptr{libmongoc::client_encryption_rewrap_many_datakey_result_new()};
    auto const result = result_owner.get();

    auto const& provider = opts.provider();
    auto const& master_key_opt = opts.master_key();

    bson_error_t error = {};

    if (!libmongoc::client_encryption_rewrap_many_datakey(
            v1::client_encryption::internal::as_mongoc(_ce),
            to_scoped_bson_view(filter).bson(),
            provider.view().empty() ? nullptr : provider.terminated().data(),
            master_key_opt ? to_scoped_bson_view(*master_key_opt).bson() : nullptr,
            result,
            &error)) {
        v_noabi::throw_exception<v_noabi::operation_exception>(error);
    }

    auto ret = v1::rewrap_many_datakey_result::internal::make();

    if (bson_t const* bulk_write_result =
            libmongoc::client_encryption_rewrap_many_datakey_result_get_bulk_write_result(result)) {
        v1::rewrap_many_datakey_result::internal::result(ret) =
            v1::bulk_write::result::internal::make(scoped_bson_view{bulk_write_result}.value());
    }

    return v_noabi::result::rewrap_many_datakey{std::move(ret)};
}

v_noabi::result::delete_result client_encryption::delete_key(
    bsoncxx::v_noabi::types::bson_value::view_or_value id) try {
    return _ce.delete_key(bsoncxx::v1::types::value{bsoncxx::v_noabi::to_v1(id.view())});
} catch (v1::exception const& ex) {
    v_noabi::throw_exception<v_noabi::operation_exception>(ex);
}

namespace {

// Backward compatibility: v_noabi translates an empty document into a null optional.
bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::value> empty_as_null(bsoncxx::v1::document::value v) {
    if (v.empty()) {
        return {};
    }

    return bsoncxx::v_noabi::from_v1(v);
}

} // namespace

bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::value> client_encryption::get_key(
    bsoncxx::v_noabi::types::bson_value::view_or_value id) try {
    return empty_as_null(_ce.get_key(bsoncxx::v1::types::value{bsoncxx::v_noabi::to_v1(id.view())}));
} catch (v1::exception const& ex) {
    v_noabi::throw_exception<v_noabi::operation_exception>(ex);
}

v_noabi::cursor client_encryption::get_keys() try { return _ce.get_keys(); } catch (v1::exception const& ex) {
    v_noabi::throw_exception<v_noabi::operation_exception>(ex);
}

bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::value> client_encryption::add_key_alt_name(
    bsoncxx::v_noabi::types::bson_value::view_or_value id,
    bsoncxx::v_noabi::string::view_or_value key_alt_name) try {
    return empty_as_null(
        _ce.add_key_alt_name(bsoncxx::v1::types::value{bsoncxx::v_noabi::to_v1(id.view())}, key_alt_name.view()));
} catch (v1::exception const& ex) {
    v_noabi::throw_exception<v_noabi::operation_exception>(ex);
}

bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::value> client_encryption::remove_key_alt_name(
    bsoncxx::v_noabi::types::bson_value::view_or_value id,
    bsoncxx::v_noabi::string::view_or_value key_alt_name) try {
    return empty_as_null(
        _ce.remove_key_alt_name(bsoncxx::v1::types::value{bsoncxx::v_noabi::to_v1(id.view())}, key_alt_name.view()));
} catch (v1::exception const& ex) {
    v_noabi::throw_exception<v_noabi::operation_exception>(ex);
}

bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::value> client_encryption::get_key_by_alt_name(
    bsoncxx::v_noabi::string::view_or_value key_alt_name) try {
    return empty_as_null(_ce.get_key_by_alt_name(key_alt_name.view()));
} catch (v1::exception const& ex) {
    v_noabi::throw_exception<v_noabi::operation_exception>(ex);
}

} // namespace v_noabi
} // namespace mongocxx
