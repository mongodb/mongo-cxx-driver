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

#include <memory>
#include <utility>

#include <bsoncxx/types/bson_value/view.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/client_encryption.hpp>
#include <mongocxx/exception/operation_exception.hpp>
#include <mongocxx/options/client_encryption.hpp>
#include <mongocxx/result/bulk_write.hpp>

#include <bsoncxx/types/bson_value/value.hh>

#include <bsoncxx/private/bson.hh>
#include <bsoncxx/private/convert.hh>
#include <bsoncxx/private/helpers.hh>

#include <mongocxx/private/bson.hh>
#include <mongocxx/private/client.hh>
#include <mongocxx/private/cursor.hh>
#include <mongocxx/private/mongoc.hh>
#include <mongocxx/private/mongoc_error.hh>
#include <mongocxx/private/scoped_bson_value.hh>

namespace mongocxx {
namespace v_noabi {

class client_encryption::impl {
   private:
    using scoped_bson_t = mongocxx::libbson::scoped_bson_t;

    struct encrypt_opts_deleter {
        void operator()(mongoc_client_encryption_encrypt_opts_t* ptr) noexcept {
            libmongoc::client_encryption_encrypt_opts_destroy(ptr);
        }
    };

    using encrypt_opts_ptr = std::unique_ptr<mongoc_client_encryption_encrypt_opts_t, encrypt_opts_deleter>;

   public:
    impl(options::client_encryption opts) : _opts(std::move(opts)) {
        using opts_type = mongoc_client_encryption_opts_t;

        struct opts_deleter {
            void operator()(opts_type* ptr) noexcept {
                libmongoc::client_encryption_opts_destroy(ptr);
            }
        };

        using encryption_opts_ptr = std::unique_ptr<opts_type, opts_deleter>;

        bson_error_t error;

        _client_encryption.reset(libmongoc::client_encryption_new(
            encryption_opts_ptr(static_cast<opts_type*>(_opts.convert())).get(), &error));

        if (!_client_encryption) {
            throw_exception<operation_exception>(error);
        }
    }

    bsoncxx::v_noabi::types::bson_value::value create_data_key(
        std::string kms_provider,
        options::data_key const& opts) {
        using opts_type = mongoc_client_encryption_datakey_opts_t;

        struct opts_deleter {
            void operator()(opts_type* ptr) noexcept {
                libmongoc::client_encryption_datakey_opts_destroy(ptr);
            }
        };

        using datakey_opts_ptr = std::unique_ptr<opts_type, opts_deleter>;

        auto const datakey_opts = datakey_opts_ptr(static_cast<opts_type*>(opts.convert()));

        detail::scoped_bson_value keyid;
        bson_error_t error;

        if (!libmongoc::client_encryption_create_datakey(
                _client_encryption.get(), kms_provider.c_str(), datakey_opts.get(), keyid.value_for_init(), &error)) {
            throw_exception<operation_exception>(error);
        }

        return bsoncxx::v_noabi::types::bson_value::make_owning_bson(keyid.get());
    }

    bsoncxx::v_noabi::types::bson_value::value encrypt(
        bsoncxx::v_noabi::types::bson_value::view value,
        options::encrypt const& opts) {
        auto const encrypt_opts =
            encrypt_opts_ptr(static_cast<mongoc_client_encryption_encrypt_opts_t*>(opts.convert()));

        detail::scoped_bson_value ciphertext;
        bson_error_t error;

        if (!libmongoc::client_encryption_encrypt(
                _client_encryption.get(),
                detail::scoped_bson_value(value).get(),
                encrypt_opts.get(),
                ciphertext.value_for_init(),
                &error)) {
            throw_exception<operation_exception>(error);
        }

        return bsoncxx::v_noabi::types::bson_value::make_owning_bson(ciphertext.get());
    }

    bsoncxx::v_noabi::document::value encrypt_expression(
        bsoncxx::v_noabi::document::view_or_value expr,
        options::encrypt const& opts) {
        auto const encrypt_opts =
            encrypt_opts_ptr(static_cast<mongoc_client_encryption_encrypt_opts_t*>(opts.convert()));

        scoped_bson_t encrypted;
        bson_error_t error = {};

        if (!libmongoc::client_encryption_encrypt_expression(
                _client_encryption.get(),
                scoped_bson_t(expr).bson(),
                encrypt_opts.get(),
                encrypted.bson_for_init(),
                &error)) {
            throw_exception<operation_exception>(error);
        }

        return encrypted.steal();
    }

    bsoncxx::v_noabi::types::bson_value::value decrypt(bsoncxx::v_noabi::types::bson_value::view value) {
        detail::scoped_bson_value decrypted_value;
        bson_error_t error;

        if (!libmongoc::client_encryption_decrypt(
                _client_encryption.get(),
                detail::scoped_bson_value(value).get(),
                decrypted_value.value_for_init(),
                &error)) {
            throw_exception<operation_exception>(error);
        }

        return bsoncxx::v_noabi::types::bson_value::make_owning_bson(decrypted_value.get());
    }

    result::rewrap_many_datakey rewrap_many_datakey(
        bsoncxx::v_noabi::document::view_or_value filter,
        options::rewrap_many_datakey const& opts) {
        using result_type = mongoc_client_encryption_rewrap_many_datakey_result_t;

        struct result_deleter {
            void operator()(result_type* ptr) noexcept {
                libmongoc::client_encryption_rewrap_many_datakey_result_destroy(ptr);
            }
        };

        using result_ptr = std::unique_ptr<result_type, result_deleter>;

        auto result = result_ptr(libmongoc::client_encryption_rewrap_many_datakey_result_new());

        auto const provider_terminated = opts.provider().terminated();

        scoped_bson_t bson_master_key;

        if (auto const master_key_opt = opts.master_key()) {
            bson_master_key.init_from_static(master_key_opt->view());
        }

        bson_error_t error;

        if (!libmongoc::client_encryption_rewrap_many_datakey(
                _client_encryption.get(),
                scoped_bson_t(filter).bson(),
                provider_terminated.view().empty() ? nullptr : provider_terminated.data(),
                bson_master_key.bson(),
                result.get(),
                &error)) {
            throw_exception<operation_exception>(error);
        }

        bson_t const* bulk_write_result =
            libmongoc::client_encryption_rewrap_many_datakey_result_get_bulk_write_result(result.get());

        if (bulk_write_result) {
            auto const doc = bsoncxx::v_noabi::document::view(bson_get_data(bulk_write_result), bulk_write_result->len);
            return result::rewrap_many_datakey(result::bulk_write(bsoncxx::v_noabi::document::value(doc)));
        } else {
            return result::rewrap_many_datakey();
        }
    }

    result::delete_result delete_key(bsoncxx::v_noabi::types::bson_value::view_or_value id) {
        using bsoncxx::v_noabi::builder::basic::kvp;
        using bsoncxx::v_noabi::builder::basic::make_document;

        scoped_bson_t reply;
        bson_error_t error;

        if (!libmongoc::client_encryption_delete_key(
                _client_encryption.get(), detail::scoped_bson_value(id.view()).get(), reply.bson_for_init(), &error)) {
            throw_exception<operation_exception>(error);
        }

        // The C driver calls this field "deletedCount", but the C++ driver
        // refers to this as "nRemoved". Make a new document with the field name
        // changed to get around this.
        //
        // See: mongocxx/v_noabi/mongocxx/result/bulk_write.cpp
        // Function: std::int32_t bulk_write::deleted_count() const {
        //     return view()["nRemoved"].get_int32();
        // }
        return result::delete_result(
            result::bulk_write(make_document(kvp("nRemoved", reply.view()["deletedCount"].get_int32()))));
    }

    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::value> get_key(
        bsoncxx::v_noabi::types::bson_value::view_or_value id) {
        libbson::scoped_bson_t key_doc;
        bson_error_t error;

        if (!libmongoc::client_encryption_get_key(
                _client_encryption.get(),
                detail::scoped_bson_value(id.view()).get(),
                key_doc.bson_for_init(),
                &error)) {
            throw_exception<operation_exception>(error);
        }

        return key_doc.view().empty()
                   ? bsoncxx::v_noabi::stdx::nullopt
                   : bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::value>{key_doc.steal()};
    }

    mongocxx::v_noabi::cursor get_keys() {
        bson_error_t error;

        mongoc_cursor_t* const cursor = mongoc_client_encryption_get_keys(_client_encryption.get(), &error);

        if (!cursor) {
            throw_exception<operation_exception>(error);
        }

        return mongocxx::v_noabi::cursor(cursor);
    }

    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::value> add_key_alt_name(
        bsoncxx::v_noabi::types::bson_value::view_or_value id,
        bsoncxx::v_noabi::string::view_or_value key_alt_name) {
        scoped_bson_t key_doc;
        bson_error_t error;

        if (!libmongoc::client_encryption_add_key_alt_name(
                _client_encryption.get(),
                detail::scoped_bson_value(id.view()).get(),
                key_alt_name.terminated().data(),
                key_doc.bson_for_init(),
                &error)) {
            throw_exception<operation_exception>(error);
        }

        return key_doc.view().empty()
                   ? bsoncxx::v_noabi::stdx::nullopt
                   : bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::value>{key_doc.steal()};
    }

    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::value> get_key_by_alt_name(
        bsoncxx::v_noabi::string::view_or_value key_alt_name) {
        scoped_bson_t key_doc;
        bson_error_t error;

        if (!libmongoc::client_encryption_get_key_by_alt_name(
                _client_encryption.get(), key_alt_name.terminated().data(), key_doc.bson_for_init(), &error)) {
            throw_exception<operation_exception>(error);
        }

        return key_doc.view().empty()
                   ? bsoncxx::v_noabi::stdx::nullopt
                   : bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::value>{key_doc.steal()};
    }

    bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::value> remove_key_alt_name(
        bsoncxx::v_noabi::types::bson_value::view_or_value id,
        bsoncxx::v_noabi::string::view_or_value key_alt_name) {
        scoped_bson_t key_doc;
        bson_error_t error;

        if (!libmongoc::client_encryption_remove_key_alt_name(
                _client_encryption.get(),
                detail::scoped_bson_value(id.view()).get(),
                key_alt_name.terminated().data(),
                key_doc.bson_for_init(),
                &error)) {
            throw_exception<operation_exception>(error);
        }

        return key_doc.view().empty()
                   ? bsoncxx::v_noabi::stdx::nullopt
                   : bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::value>{key_doc.steal()};
    }

    collection create_encrypted_collection(
        database const& dbcxx,
        mongoc_database_t* const db,
        std::string const& coll_name,
        bsoncxx::v_noabi::document::view const opts,
        bsoncxx::v_noabi::document::value& out_options,
        std::string const& kms_provider,
        bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view> const& masterkey) {
        bson_error_t error = {};
        scoped_bson_t out_opts;
        out_opts.init();

        bson_t* opt_mkey_ptr = nullptr;
        scoped_bson_t opt_mkey;
        if (masterkey) {
            opt_mkey.init_from_static(*masterkey);
            opt_mkey_ptr = opt_mkey.bson();
        }

        scoped_bson_t coll_opts{opts};

        auto coll_ptr = libmongoc::client_encryption_create_encrypted_collection(
            _client_encryption.get(),
            db,
            coll_name.data(),
            coll_opts.bson(),
            out_opts.bson(),
            kms_provider.data(),
            opt_mkey_ptr,
            &error);
        out_options = bsoncxx::helpers::value_from_bson_t(out_opts.bson());
        if (!coll_ptr) {
            throw_exception<operation_exception>(error);
        }
        return collection(dbcxx, coll_ptr);
    }

   private:
    struct encryption_deleter {
        void operator()(mongoc_client_encryption_t* ptr) noexcept {
            libmongoc::client_encryption_destroy(ptr);
        }
    };

    options::client_encryption _opts;
    std::unique_ptr<mongoc_client_encryption_t, encryption_deleter> _client_encryption;
};

} // namespace v_noabi
} // namespace mongocxx
