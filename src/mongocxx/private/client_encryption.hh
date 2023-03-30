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

#include <bsoncxx/private/libbson.hh>
#include <bsoncxx/types/bson_value/private/value.hh>
#include <bsoncxx/types/bson_value/value.hpp>
#include <bsoncxx/types/bson_value/view.hpp>
#include <bsoncxx/types/private/convert.hh>
#include <mongocxx/client.hpp>
#include <mongocxx/exception/operation_exception.hpp>
#include <mongocxx/exception/private/mongoc_error.hh>
#include <mongocxx/options/client_encryption.hpp>
#include <mongocxx/private/client.hh>
#include <mongocxx/private/cursor.hh>
#include <mongocxx/private/libbson.hh>
#include <mongocxx/private/libmongoc.hh>
#include <mongocxx/result/bulk_write.hpp>

#include <mongocxx/config/private/prelude.hh>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

using bsoncxx::types::convert_from_libbson;
using bsoncxx::types::convert_to_libbson;

class client_encryption::impl {
   public:
    impl(options::client_encryption opts) : _opts(std::move(opts)) {
        bson_error_t error;

        auto encryption_opts = static_cast<mongoc_client_encryption_opts_t*>(_opts.convert());
        _client_encryption_t = libmongoc::client_encryption_new(encryption_opts, &error);

        libmongoc::client_encryption_opts_destroy(encryption_opts);

        if (_client_encryption_t == nullptr) {
            throw_exception<operation_exception>(error);
        }
    }

    ~impl() {
        libmongoc::client_encryption_destroy(_client_encryption_t);
    }

    bsoncxx::types::bson_value::value create_data_key(std::string kms_provider,
                                                      const options::data_key& opts) {
        bson_value_t keyid;
        bson_error_t error;

        auto datakey_opts = static_cast<mongoc_client_encryption_datakey_opts_t*>(opts.convert());

        auto cleanup = [&]() {
            bson_value_destroy(&keyid);
            libmongoc::client_encryption_datakey_opts_destroy(datakey_opts);
        };

        if (!libmongoc::client_encryption_create_datakey(
                _client_encryption_t, kms_provider.c_str(), datakey_opts, &keyid, &error)) {
            cleanup();
            throw_exception<operation_exception>(error);
        }

        bsoncxx::types::bson_value::value out =
            bsoncxx::types::bson_value::make_owning_bson(&keyid);

        cleanup();

        return out;
    }

    bsoncxx::types::bson_value::value encrypt(bsoncxx::types::bson_value::view value,
                                              const options::encrypt& opts) {
        bson_error_t error;
        bson_value_t ciphertext;

        bson_value_t libbson_value;

        convert_to_libbson(&libbson_value, value);

        mongoc_client_encryption_encrypt_opts_t* converted_opts;

        converted_opts = (mongoc_client_encryption_encrypt_opts_t*)opts.convert();

        auto r = libmongoc::client_encryption_encrypt(
            _client_encryption_t, &libbson_value, converted_opts, &ciphertext, &error);

        auto cleanup = [&]() {
            bson_value_destroy(&libbson_value);
            bson_value_destroy(&ciphertext);
            libmongoc::client_encryption_encrypt_opts_destroy(converted_opts);
        };

        if (!r) {
            cleanup();
            throw_exception<operation_exception>(error);
        }

        auto encrypted = bsoncxx::types::bson_value::make_owning_bson(&ciphertext);

        cleanup();

        return encrypted;
    }

    bsoncxx::types::bson_value::value decrypt(bsoncxx::types::bson_value::view value) {
        bson_error_t error;
        bson_value_t decrypted_value;

        bson_value_t encrypted;

        convert_to_libbson(&encrypted, value);

        auto r = libmongoc::client_encryption_decrypt(
            _client_encryption_t, &encrypted, &decrypted_value, &error);

        auto cleanup = [&]() {
            bson_value_destroy(&decrypted_value);
            bson_value_destroy(&encrypted);
        };

        if (!r) {
            cleanup();
            throw_exception<operation_exception>(error);
        }

        auto decrypted = bsoncxx::types::bson_value::make_owning_bson(&decrypted_value);

        cleanup();

        return decrypted;
    }

    result::rewrap_many_datakey rewrap_many_datakey(bsoncxx::document::view_or_value filter,
                                                    const options::rewrap_many_datakey& opts) {
        bson_error_t error;

        std::unique_ptr<mongoc_client_encryption_rewrap_many_datakey_result_t,
                        decltype(libmongoc::client_encryption_rewrap_many_datakey_result_destroy)>
            result_ptr(libmongoc::client_encryption_rewrap_many_datakey_result_new(),
                       libmongoc::client_encryption_rewrap_many_datakey_result_destroy);

        auto provider = opts.provider();
        auto provider_terminated = provider.terminated();
        const char* provider_ptr =
            provider_terminated.view().empty() ? nullptr : provider_terminated.data();

        auto optional_master_key = opts.master_key();
        stdx::optional<mongocxx::libbson::scoped_bson_t> bson_master_key;
        if (optional_master_key) {
            bson_master_key.emplace();
            bson_master_key->init_from_static(optional_master_key.value().view());
        }

        libbson::scoped_bson_t bson_filter;
        bson_filter.init_from_static(filter);

        auto r = libmongoc::client_encryption_rewrap_many_datakey(
            _client_encryption_t,
            bson_filter.bson(),
            provider_ptr,
            bson_master_key ? bson_master_key->bson() : nullptr,
            result_ptr.get(),
            &error);

        if (!r) {
            throw_exception<operation_exception>(error);
        }

        const bson_t* bulk_write_result =
            libmongoc::client_encryption_rewrap_many_datakey_result_get_bulk_write_result(
                result_ptr.get());

        if (bulk_write_result) {
            const auto doc =
                bsoncxx::document::view(bson_get_data(bulk_write_result), bulk_write_result->len);
            auto val = bsoncxx::document::value(doc);
            result::bulk_write bulk_write_result{val};
            return result::rewrap_many_datakey(bulk_write_result);
        } else {
            return result::rewrap_many_datakey();
        }
    }

    result::delete_result delete_key(bsoncxx::types::bson_value::view_or_value id) {
        using bsoncxx::builder::basic::kvp;
        using bsoncxx::builder::basic::make_document;

        bson_error_t error;
        libbson::scoped_bson_t reply_ptr;

        bson_value_t libbson_key;

        convert_to_libbson(&libbson_key, id);

        auto r = libmongoc::client_encryption_delete_key(
            _client_encryption_t, &libbson_key, reply_ptr.bson_for_init(), &error);

        bson_value_destroy(&libbson_key);

        if (!r) {
            throw_exception<operation_exception>(error);
        }

        bsoncxx::document::value val = reply_ptr.steal();

        // The C driver calls this field "deletedCount", but the C++ driver
        // refers to this as "nRemoved". Make a new document with the field name
        // changed to get around this.
        //
        // See: mongo-cxx-driver/src/mongocxx/result/bulk_write.cpp
        // Function: std::int32_t bulk_write::deleted_count() const {
        //     return view()["nRemoved"].get_int32();
        // }
        auto new_val = make_document(kvp("nRemoved", val["deletedCount"].get_int32()));
        return result::delete_result(result::bulk_write(new_val));
    }

    stdx::optional<bsoncxx::document::value> get_key(bsoncxx::types::bson_value::view_or_value id) {
        bson_error_t error;
        libbson::scoped_bson_t key_doc;

        bson_value_t libbson_value;

        convert_to_libbson(&libbson_value, id);

        auto r = libmongoc::client_encryption_get_key(
            _client_encryption_t, &libbson_value, key_doc.bson_for_init(), &error);

        auto cleanup = [&]() { bson_value_destroy(&libbson_value); };

        if (!r) {
            cleanup();
            throw_exception<operation_exception>(error);
        }

        bsoncxx::document::value val(key_doc.steal());

        cleanup();
        return val.empty() ? stdx::nullopt : stdx::optional<bsoncxx::document::value>{val};
    }

    mongocxx::cursor get_keys() {
        bson_error_t error;

        mongoc_cursor_t* cursor = mongoc_client_encryption_get_keys(_client_encryption_t, &error);

        if (!cursor) {
            throw_exception<operation_exception>(error);
        }

        mongocxx::cursor wrapped_cursor{cursor};
        return wrapped_cursor;
    }

    stdx::optional<bsoncxx::document::value> add_key_alt_name(
        bsoncxx::types::bson_value::view_or_value id, const std::string& key_alt_name) {
        bson_error_t error;
        libbson::scoped_bson_t key_doc;
        bson_value_t key_id;

        convert_to_libbson(&key_id, id);

        auto r = libmongoc::client_encryption_add_key_alt_name(
            _client_encryption_t, &key_id, key_alt_name.c_str(), key_doc.bson_for_init(), &error);

        auto cleanup = [&]() { bson_value_destroy(&key_id); };

        if (!r) {
            cleanup();
            throw_exception<operation_exception>(error);
        }

        bsoncxx::document::value val(key_doc.steal());

        cleanup();
        return val.empty() ? stdx::nullopt : stdx::optional<bsoncxx::document::value>{val};
    }

    stdx::optional<bsoncxx::document::value> get_key_by_alt_name(const std::string& key_alt_name) {
        bson_error_t error;
        libbson::scoped_bson_t key_doc;

        auto r = libmongoc::client_encryption_get_key_by_alt_name(
            _client_encryption_t, key_alt_name.c_str(), key_doc.bson_for_init(), &error);

        if (!r) {
            throw_exception<operation_exception>(error);
        }
        bsoncxx::document::value val = key_doc.steal();
        return val.empty() ? stdx::nullopt : stdx::optional<bsoncxx::document::value>{val};
    }

    stdx::optional<bsoncxx::document::value> remove_key_alt_name(
        bsoncxx::types::bson_value::view_or_value id, const std::string& key_alt_name) {
        bson_error_t error;
        libbson::scoped_bson_t key_doc;
        bson_value_t key_id;

        convert_to_libbson(&key_id, id);

        auto r = libmongoc::client_encryption_remove_key_alt_name(
            _client_encryption_t, &key_id, key_alt_name.c_str(), key_doc.bson_for_init(), &error);

        auto cleanup = [&]() { bson_value_destroy(&key_id); };

        if (!r) {
            cleanup();
            throw_exception<operation_exception>(error);
        }

        bsoncxx::document::value val(key_doc.steal());

        cleanup();
        return val.empty() ? stdx::nullopt : stdx::optional<bsoncxx::document::value>{val};
    }

    options::client_encryption _opts;
    mongoc_client_encryption_t* _client_encryption_t;
};

MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx

#include <mongocxx/config/private/postlude.hh>
