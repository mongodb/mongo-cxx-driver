// Copyright 2009-present MongoDB, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, // WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, //
// either express or implied. See the License for the specific language governing permissions and limitations under the
// License.

#include <mongocxx/v1/client_encryption.hh>

//

#include <bsoncxx/v1/document/value.hpp>
#include <bsoncxx/v1/document/view.hpp>
#include <bsoncxx/v1/stdx/optional.hpp>
#include <bsoncxx/v1/stdx/string_view.hpp>

#include <mongocxx/v1/encrypt_options.hpp>

#include <bsoncxx/v1/types/value.hh>

#include <mongocxx/v1/bulk_write.hh>
#include <mongocxx/v1/client.hh>
#include <mongocxx/v1/collection.hh>
#include <mongocxx/v1/cursor.hh>
#include <mongocxx/v1/data_key_options.hh>
#include <mongocxx/v1/database.hh>
#include <mongocxx/v1/delete_one_result.hh>
#include <mongocxx/v1/encrypt_options.hh>
#include <mongocxx/v1/exception.hh>
#include <mongocxx/v1/range_options.hh>
#include <mongocxx/v1/rewrap_many_datakey_options.hh>
#include <mongocxx/v1/rewrap_many_datakey_result.hh>
#include <mongocxx/v1/text_options.hh>

#include <memory>
#include <string>

#include <bsoncxx/private/bson.hh>

#include <mongocxx/private/mongoc.hh>
#include <mongocxx/private/scoped_bson.hh>
#include <mongocxx/private/utility.hh>

namespace mongocxx {
namespace v1 {

namespace {

mongoc_client_encryption_t* to_mongoc(void* ptr) {
    return static_cast<mongoc_client_encryption_t*>(ptr);
}

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

encrypt_opts_ptr_type to_mongoc(v1::encrypt_options const& opts) {
    struct encrypt_opts_deleter {
        void operator()(mongoc_client_encryption_encrypt_opts_t* ptr) noexcept {
            libmongoc::client_encryption_encrypt_opts_destroy(ptr);
        }
    };

    auto ret = encrypt_opts_ptr_type{libmongoc::client_encryption_encrypt_opts_new()};
    auto const ptr = ret.get();

    // libmongoc will error if both key_id and key_alt_name are set, so no need to check here.

    if (auto const& opt = v1::encrypt_options::internal::key_id(opts)) {
        libmongoc::client_encryption_encrypt_opts_set_keyid(ptr, &get_bson_value(*opt));
    }

    if (auto const& opt = v1::encrypt_options::internal::key_alt_name(opts)) {
        libmongoc::client_encryption_encrypt_opts_set_keyaltname(ptr, opt->c_str());
    }

    if (auto const opt = opts.algorithm()) {
        switch (*opt) {
            case encrypt_options::encryption_algorithm::k_deterministic:
                libmongoc::client_encryption_encrypt_opts_set_algorithm(
                    ptr, MONGOC_AEAD_AES_256_CBC_HMAC_SHA_512_DETERMINISTIC);
                break;
            case encrypt_options::encryption_algorithm::k_random:
                libmongoc::client_encryption_encrypt_opts_set_algorithm(
                    ptr, MONGOC_AEAD_AES_256_CBC_HMAC_SHA_512_RANDOM);
                break;
            case encrypt_options::encryption_algorithm::k_indexed:
                libmongoc::client_encryption_encrypt_opts_set_algorithm(ptr, MONGOC_ENCRYPT_ALGORITHM_INDEXED);
                break;
            case encrypt_options::encryption_algorithm::k_unindexed:
                libmongoc::client_encryption_encrypt_opts_set_algorithm(ptr, MONGOC_ENCRYPT_ALGORITHM_UNINDEXED);
                break;
            case encrypt_options::encryption_algorithm::k_range:
                libmongoc::client_encryption_encrypt_opts_set_algorithm(ptr, MONGOC_ENCRYPT_ALGORITHM_RANGE);
                break;
            case encrypt_options::encryption_algorithm::k_textPreview:
                libmongoc::client_encryption_encrypt_opts_set_algorithm(ptr, MONGOC_ENCRYPT_ALGORITHM_TEXTPREVIEW);
                break;
            default:
                // When no valid encryption algorithm is given, mongoc will return an error.
                break;
        }
    }

    if (auto const opt = opts.contention_factor()) {
        libmongoc::client_encryption_encrypt_opts_set_contention_factor(ptr, *opt);
    }

    if (auto const opt = opts.query_type()) {
        switch (*opt) {
            case encrypt_options::encryption_query_type::k_equality:
                libmongoc::client_encryption_encrypt_opts_set_query_type(ptr, MONGOC_ENCRYPT_QUERY_TYPE_EQUALITY);
                break;
            case encrypt_options::encryption_query_type::k_range:
                libmongoc::client_encryption_encrypt_opts_set_query_type(ptr, MONGOC_ENCRYPT_QUERY_TYPE_RANGE);
                break;
            case encrypt_options::encryption_query_type::k_prefixPreview:
                libmongoc::client_encryption_encrypt_opts_set_query_type(ptr, MONGOC_ENCRYPT_QUERY_TYPE_PREFIXPREVIEW);
                break;
            case encrypt_options::encryption_query_type::k_suffixPreview:
                libmongoc::client_encryption_encrypt_opts_set_query_type(ptr, MONGOC_ENCRYPT_QUERY_TYPE_SUFFIXPREVIEW);
                break;
            case encrypt_options::encryption_query_type::k_substringPreview:
                libmongoc::client_encryption_encrypt_opts_set_query_type(
                    ptr, MONGOC_ENCRYPT_QUERY_TYPE_SUBSTRINGPREVIEW);
                break;
            default:
                // // When no valid query type is given, libmongocrypt will return an error.
                break;
        }
    }

    if (auto const& opt = v1::encrypt_options::internal::range_opts(opts)) {
        struct range_opts_deleter {
            void operator()(mongoc_client_encryption_encrypt_range_opts_t* ptr) noexcept {
                libmongoc::client_encryption_encrypt_range_opts_destroy(ptr);
            }
        };

        auto const range_opts_owner =
            std::unique_ptr<mongoc_client_encryption_encrypt_range_opts_t, range_opts_deleter>(
                libmongoc::client_encryption_encrypt_range_opts_new());
        auto const range_opts = range_opts_owner.get();

        auto const& min = v1::range_options::internal::min(*opt);
        auto const& max = v1::range_options::internal::max(*opt);
        auto const precision = opt->precision();
        auto const sparsity = opt->sparsity();
        auto const trim_factor = opt->trim_factor();

        if (min) {
            libmongoc::client_encryption_encrypt_range_opts_set_min(range_opts, &get_bson_value(*min));
        }

        if (max) {
            libmongoc::client_encryption_encrypt_range_opts_set_max(range_opts, &get_bson_value(*max));
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

    if (auto const& opt = v1::encrypt_options::internal::text_opts(opts)) {
        struct text_opts_deleter {
            void operator()(mongoc_client_encryption_encrypt_text_opts_t* ptr) noexcept {
                libmongoc::client_encryption_encrypt_text_opts_destroy(ptr);
            }
        };

        auto const text_opts_owner = std::unique_ptr<mongoc_client_encryption_encrypt_text_opts_t, text_opts_deleter>(
            libmongoc::client_encryption_encrypt_text_opts_new());
        auto const text_opts = text_opts_owner.get();

        auto const case_sensitive = opt->case_sensitive();
        auto const diacritic_sensitive = opt->diacritic_sensitive();
        auto const& prefix = v1::text_options::internal::prefix_opts(*opt);
        auto const& suffix = v1::text_options::internal::suffix_opts(*opt);
        auto const& substring = v1::text_options::internal::substring_opts(*opt);

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

            auto const str_max_query_length = prefix->str_max_query_length();
            auto const str_min_query_length = prefix->str_min_query_length();

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

            auto const str_max_query_length = suffix->str_max_query_length();
            auto const str_min_query_length = suffix->str_min_query_length();

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

            auto const str_max_query_length = substring->str_max_query_length();
            auto const str_min_query_length = substring->str_min_query_length();
            auto const str_max_length = substring->str_max_length();

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

client_encryption::~client_encryption() {
    libmongoc::client_encryption_destroy(to_mongoc(_impl));
}

client_encryption::client_encryption(client_encryption&& other) noexcept : _impl{exchange(other._impl, nullptr)} {}

client_encryption& client_encryption::operator=(client_encryption&& other) noexcept {
    if (this != &other) {
        libmongoc::client_encryption_destroy(to_mongoc(exchange(_impl, exchange(other._impl, nullptr))));
    }

    return *this;
}

client_encryption::client_encryption(options const& opts) {
    bson_error_t error = {};
    if (!(_impl =
              libmongoc::client_encryption_new(client_encryption::options::internal::to_mongoc(opts).get(), &error))) {
        v1::throw_exception(error);
    }
}

bsoncxx::v1::types::value client_encryption::create_data_key(
    bsoncxx::v1::stdx::string_view kms_provider,
    v1::data_key_options const& opts) {
    bsoncxx::v1::types::value keyid;
    bson_error_t error = {};

    if (!libmongoc::client_encryption_create_datakey(
            to_mongoc(_impl),
            std::string{kms_provider}.c_str(),
            v1::data_key_options::internal::to_mongoc(opts).get(),
            &get_bson_value(keyid),
            &error)) {
        v1::throw_exception(error);
    }

    return bsoncxx::v1::types::value{std::move(keyid)};
}

bsoncxx::v1::types::value client_encryption::create_data_key(bsoncxx::v1::stdx::string_view kms_provider) {
    return this->create_data_key(kms_provider, v1::data_key_options{});
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

    v1::throw_exception(error);
}

} // namespace

v1::collection client_encryption::create_encrypted_collection(
    v1::database& db,
    bsoncxx::v1::stdx::string_view name,
    bsoncxx::v1::document::view opts,
    bsoncxx::v1::document::value& coll_opts,
    bsoncxx::v1::stdx::string_view kms_provider,
    bsoncxx::v1::document::view master_key) {
    return create_encrypted_collection_impl(
        to_mongoc(_impl),
        v1::database::internal::get_client(db),
        v1::database::internal::as_mongoc(db),
        std::string{name}.c_str(),
        scoped_bson_view{opts}.bson(),
        coll_opts,
        std::string{kms_provider}.c_str(),
        scoped_bson_view{master_key}.bson());
}

v1::collection client_encryption::create_encrypted_collection(
    v1::database& db,
    bsoncxx::v1::stdx::string_view name,
    bsoncxx::v1::document::view opts,
    bsoncxx::v1::document::value& coll_opts,
    bsoncxx::v1::stdx::string_view kms_provider) {
    return create_encrypted_collection_impl(
        to_mongoc(_impl),
        v1::database::internal::get_client(db),
        v1::database::internal::as_mongoc(db),
        std::string{name}.c_str(),
        scoped_bson_view{opts}.bson(),
        coll_opts,
        std::string{kms_provider}.c_str(),
        nullptr);
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

bsoncxx::v1::types::value client_encryption::encrypt(bsoncxx::v1::types::value value, v1::encrypt_options const& opts) {
    bsoncxx::v1::types::value ciphertext;
    bson_error_t error = {};

    if (!libmongoc::client_encryption_encrypt(
            to_mongoc(_impl),
            &get_bson_value(ensure_not_null_string(std::move(value))),
            to_mongoc(opts).get(),
            &get_bson_value(ciphertext),
            &error)) {
        v1::throw_exception(error);
    }

    return ciphertext;
}

bsoncxx::v1::document::value client_encryption::encrypt_expression(
    bsoncxx::v1::document::view expr,
    v1::encrypt_options const& opts) {
    auto const encrypt_opts = to_mongoc(opts);

    bson_error_t error = {};

    scoped_bson encrypted;
    if (!libmongoc::client_encryption_encrypt_expression(
            to_mongoc(_impl), scoped_bson_view{expr}.bson(), encrypt_opts.get(), encrypted.out_ptr(), &error)) {
        v1::throw_exception(error);
    }

    return std::move(encrypted).value();
}

bsoncxx::v1::types::value client_encryption::decrypt(bsoncxx::v1::types::value value) {
    bsoncxx::v1::types::value decrypted_value;
    bson_error_t error = {};

    if (!libmongoc::client_encryption_decrypt(
            to_mongoc(_impl),
            &get_bson_value(ensure_not_null_string(std::move(value))),
            &get_bson_value(decrypted_value),
            &error)) {
        v1::throw_exception(error);
    }

    return decrypted_value;
}

v1::rewrap_many_datakey_result client_encryption::rewrap_many_datakey(
    bsoncxx::v1::document::view filter,
    v1::rewrap_many_datakey_options const& opts) {
    struct result_deleter {
        void operator()(mongoc_client_encryption_rewrap_many_datakey_result_t* ptr) noexcept {
            libmongoc::client_encryption_rewrap_many_datakey_result_destroy(ptr);
        }
    };

    using result_ptr = std::unique_ptr<mongoc_client_encryption_rewrap_many_datakey_result_t, result_deleter>;

    auto const result_owner = result_ptr{libmongoc::client_encryption_rewrap_many_datakey_result_new()};
    auto const result = result_owner.get();

    auto const& provider = v1::rewrap_many_datakey_options::internal::provider(opts);
    auto const& master_key_opt = v1::rewrap_many_datakey_options::internal::master_key(opts);

    bson_error_t error = {};

    if (!libmongoc::client_encryption_rewrap_many_datakey(
            to_mongoc(_impl),
            scoped_bson_view{filter}.bson(),
            provider.empty() ? nullptr : provider.c_str(),
            master_key_opt ? scoped_bson_view{*master_key_opt}.bson() : nullptr,
            result,
            &error)) {
        v1::throw_exception(error);
    }

    auto ret = v1::rewrap_many_datakey_result::internal::make();

    if (bson_t const* bulk_write_result =
            libmongoc::client_encryption_rewrap_many_datakey_result_get_bulk_write_result(result)) {
        v1::rewrap_many_datakey_result::internal::result(ret) =
            v1::bulk_write::result::internal::make(scoped_bson_view{bulk_write_result}.value());
    }

    return ret;
}

v1::delete_one_result client_encryption::delete_key(bsoncxx::v1::types::value id) {
    scoped_bson reply;
    bson_error_t error = {};

    if (!libmongoc::client_encryption_delete_key(
            to_mongoc(_impl), &get_bson_value(ensure_not_null_string(std::move(id))), reply.out_ptr(), &error)) {
        v1::throw_exception(error);
    }

    // Use mongoc convention for bulk write result fields.
    return v1::delete_one_result::internal::make(
        v1::bulk_write::result::internal::make(
            scoped_bson{BCON_NEW("nRemoved", BCON_INT32(reply.view()["deletedCount"].get_int32().value))}.value()));
}

bsoncxx::v1::document::value client_encryption::get_key(bsoncxx::v1::types::value id) {
    scoped_bson key = {};
    bson_error_t error;

    if (!libmongoc::client_encryption_get_key(
            to_mongoc(_impl), &get_bson_value(ensure_not_null_string(std::move(id))), key.out_ptr(), &error)) {
        v1::throw_exception(error);
    }

    return std::move(key).value();
}

v1::cursor client_encryption::get_keys() {
    bson_error_t error = {};

    if (auto const ptr = mongoc_client_encryption_get_keys(to_mongoc(_impl), &error)) {
        return v1::cursor::internal::make(ptr);
    }

    v1::throw_exception(error);
}

bsoncxx::v1::document::value client_encryption::add_key_alt_name(
    bsoncxx::v1::types::value id,
    bsoncxx::v1::stdx::string_view key_alt_name) {
    scoped_bson key;
    bson_error_t error = {};

    if (!libmongoc::client_encryption_add_key_alt_name(
            to_mongoc(_impl),
            &get_bson_value(ensure_not_null_string(std::move(id))),
            std::string{key_alt_name}.c_str(),
            key.out_ptr(),
            &error)) {
        v1::throw_exception(error);
    }

    return std::move(key).value();
}

bsoncxx::v1::document::value client_encryption::remove_key_alt_name(
    bsoncxx::v1::types::value id,
    bsoncxx::v1::stdx::string_view key_alt_name) {
    scoped_bson key;
    bson_error_t error = {};

    if (!libmongoc::client_encryption_remove_key_alt_name(
            to_mongoc(_impl),
            &get_bson_value(ensure_not_null_string(std::move(id))),
            std::string{key_alt_name}.c_str(),
            key.out_ptr(),
            &error)) {
        v1::throw_exception(error);
    }

    return std::move(key).value();
}

bsoncxx::v1::document::value client_encryption::get_key_by_alt_name(bsoncxx::v1::stdx::string_view key_alt_name) {
    scoped_bson key;
    bson_error_t error;

    if (!libmongoc::client_encryption_get_key_by_alt_name(
            to_mongoc(_impl), std::string{key_alt_name}.c_str(), key.out_ptr(), &error)) {
        v1::throw_exception(error);
    }

    return std::move(key).value();
}

client_encryption::client_encryption(void* impl) : _impl{impl} {}

class client_encryption::options::impl {
   public:
    mongocxx::v1::client* _key_vault_client = nullptr;
    bsoncxx::v1::stdx::optional<ns_pair> _key_vault_namespace;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> _kms_providers;
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> _tls_opts;

    static impl const& with(options const& self) {
        return *static_cast<impl const*>(self._impl);
    }

    static impl const* with(options const* self) {
        return static_cast<impl const*>(self->_impl);
    }

    static impl& with(options& self) {
        return *static_cast<impl*>(self._impl);
    }

    static impl* with(options* self) {
        return static_cast<impl*>(self->_impl);
    }

    static impl* with(void* ptr) {
        return static_cast<impl*>(ptr);
    }
};

// NOLINTBEGIN(cppcoreguidelines-owning-memory): owning void* for ABI stability.

client_encryption::options::~options() {
    delete impl::with(this);
}

client_encryption::options::options(options&& other) noexcept : _impl{exchange(other._impl, nullptr)} {}

client_encryption::options& client_encryption::options::operator=(options&& other) noexcept {
    if (this != &other) {
        delete impl::with(exchange(_impl, exchange(other._impl, nullptr)));
    }

    return *this;
}

client_encryption::options::options(options const& other) : _impl{new impl{impl::with(other)}} {}

client_encryption::options& client_encryption::options::operator=(options const& other) {
    if (this != &other) {
        delete impl::with(exchange(_impl, new impl{impl::with(other)}));
    }

    return *this;
}

client_encryption::options::options() : _impl{new impl{}} {}

// NOLINTEND(cppcoreguidelines-owning-memory)

client_encryption::options& client_encryption::options::key_vault_client(mongocxx::v1::client* v) {
    impl::with(this)->_key_vault_client = std::move(v);
    return *this;
}

mongocxx::v1::client* client_encryption::options::key_vault_client() const {
    return impl::with(this)->_key_vault_client;
}

client_encryption::options& client_encryption::options::key_vault_namespace(ns_pair v) {
    impl::with(this)->_key_vault_namespace = std::move(v);
    return *this;
}

bsoncxx::v1::stdx::optional<client_encryption::options::ns_pair> client_encryption::options::key_vault_namespace()
    const {
    return impl::with(this)->_key_vault_namespace;
}

client_encryption::options& client_encryption::options::kms_providers(bsoncxx::v1::document::value v) {
    impl::with(this)->_kms_providers = std::move(v);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> client_encryption::options::kms_providers() const {
    return impl::with(this)->_kms_providers;
}

client_encryption::options& client_encryption::options::tls_opts(bsoncxx::v1::document::value v) {
    impl::with(this)->_tls_opts = std::move(v);
    return *this;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> client_encryption::options::tls_opts() const {
    return impl::with(this)->_tls_opts;
}

client_encryption client_encryption::internal::make(mongoc_client_encryption_t* ptr) {
    return {ptr};
}

mongoc_client_encryption_t* client_encryption::internal::as_mongoc(client_encryption const& self) {
    return to_mongoc(self._impl);
}

bsoncxx::v1::stdx::optional<client_encryption::options::ns_pair> const&
client_encryption::options::internal::key_vault_namespace(options const& self) {
    return impl::with(self)._key_vault_namespace;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> const& client_encryption::options::internal::kms_providers(
    options const& self) {
    return impl::with(self)._kms_providers;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value> const& client_encryption::options::internal::tls_opts(
    options const& self) {
    return impl::with(self)._tls_opts;
}

bsoncxx::v1::stdx::optional<client_encryption::options::ns_pair>&
client_encryption::options::internal::key_vault_namespace(options& self) {
    return impl::with(self)._key_vault_namespace;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>& client_encryption::options::internal::kms_providers(
    options& self) {
    return impl::with(self)._kms_providers;
}

bsoncxx::v1::stdx::optional<bsoncxx::v1::document::value>& client_encryption::options::internal::tls_opts(
    options& self) {
    return impl::with(self)._tls_opts;
}

client_encryption::options::internal::opts_ptr_type client_encryption::options::internal::to_mongoc(
    client_encryption::options const& opts) {
    auto ret = opts_ptr_type{libmongoc::client_encryption_opts_new(), opts_deleter{}};
    auto const ptr = ret.get();

    if (auto const opt = opts.key_vault_client()) {
        libmongoc::client_encryption_opts_set_keyvault_client(ptr, v1::client::internal::as_mongoc(*opt));
    }

    if (auto const& opt = client_encryption::options::internal::key_vault_namespace(opts)) {
        auto const& ns = *opt;
        libmongoc::client_encryption_opts_set_keyvault_namespace(ptr, ns.first.c_str(), ns.second.c_str());
    }

    if (auto const& opt = client_encryption::options::internal::kms_providers(opts)) {
        libmongoc::client_encryption_opts_set_kms_providers(ptr, scoped_bson_view{*opt}.bson());
    }

    if (auto const& opt = client_encryption::options::internal::tls_opts(opts)) {
        libmongoc::client_encryption_opts_set_tls_opts(ptr, scoped_bson_view{*opt}.bson());
    }

    return ret;
}

} // namespace v1
} // namespace mongocxx
