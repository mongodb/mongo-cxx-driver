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

#include <mongocxx/private/libbson.hh>
#include <mongocxx/private/libmongoc.hh>

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

    options::client_encryption _opts;
    mongoc_client_encryption_t* _client_encryption_t;
};

MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx

#include <mongocxx/config/private/postlude.hh>
