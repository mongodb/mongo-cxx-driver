// Copyright 2017-present MongoDB Inc.
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

#include <bsoncxx/private/helpers.hh>
#include <bsoncxx/private/libbson.hh>
#include <mongocxx/client_session.hpp>
#include <mongocxx/exception/error_code.hpp>
#include <mongocxx/exception/logic_error.hpp>
#include <mongocxx/exception/operation_exception.hpp>
#include <mongocxx/exception/private/mongoc_error.hh>
#include <mongocxx/options/private/transaction.hh>
#include <mongocxx/private/client.hh>
#include <mongocxx/private/libbson.hh>
#include <mongocxx/private/libmongoc.hh>

#include <mongocxx/config/private/prelude.hh>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

class client_session::impl {
   public:
    impl(const class client* client, const options::client_session& session_options)
        : _client(client), _options(session_options), _session_t(nullptr, nullptr) {
        // Create a mongoc_session_opts_t from session_options.
        std::unique_ptr<mongoc_session_opt_t, decltype(libmongoc::session_opts_destroy)> opt_t{
            libmongoc::session_opts_new(), libmongoc::session_opts_destroy};

        libmongoc::session_opts_set_causal_consistency(opt_t.get(), _options.causal_consistency());

        if (session_options.default_transaction_opts()) {
            libmongoc::session_opts_set_default_transaction_opts(
                opt_t.get(),
                (session_options.default_transaction_opts())->_get_impl().get_transaction_opt_t());
        }

        bson_error_t error;
        auto s =
            libmongoc::client_start_session(_client->_get_impl().client_t, opt_t.get(), &error);
        if (!s) {
            throw mongocxx::exception{error_code::k_cannot_create_session, error.message};
        }

        _session_t = unique_session{
            s, [](mongoc_client_session_t* cs) { libmongoc::client_session_destroy(cs); }};
    }

    const class client& client() const noexcept {
        return *_client;
    }

    const options::client_session& options() const noexcept {
        return _options;
    }

    // Get session id, also known as "logical session id" or "lsid".
    bsoncxx::document::view id() const noexcept {
        return bsoncxx::helpers::view_from_bson_t(
            libmongoc::client_session_get_lsid(_session_t.get()));
    }

    bsoncxx::document::view cluster_time() const noexcept {
        const bson_t* ct = libmongoc::client_session_get_cluster_time(_session_t.get());
        if (ct) {
            return bsoncxx::helpers::view_from_bson_t(ct);
        }

        return bsoncxx::helpers::view_from_bson_t(&_empty_cluster_time);
    }

    bsoncxx::types::b_timestamp operation_time() const noexcept {
        bsoncxx::types::b_timestamp ts;
        libmongoc::client_session_get_operation_time(
            _session_t.get(), &ts.timestamp, &ts.increment);
        return ts;
    }

    void advance_cluster_time(const bsoncxx::document::view& cluster_time) noexcept {
        bson_t bson;
        bson_init_static(&bson, cluster_time.data(), cluster_time.length());
        libmongoc::client_session_advance_cluster_time(_session_t.get(), &bson);
    }

    void advance_operation_time(const bsoncxx::types::b_timestamp& operation_time) noexcept {
        libmongoc::client_session_advance_operation_time(
            _session_t.get(), operation_time.timestamp, operation_time.increment);
    }

    void start_transaction(const stdx::optional<options::transaction>& transaction_opts) {
        bson_error_t error;
        mongoc_transaction_opt_t* transaction_opt_t = nullptr;

        if (transaction_opts) {
            transaction_opt_t = transaction_opts->_get_impl().get_transaction_opt_t();
        }

        if (!libmongoc::client_session_start_transaction(
                _session_t.get(), transaction_opt_t, &error)) {
            throw_exception<operation_exception>(error);
        }
    }

    void commit_transaction() {
        libbson::scoped_bson_t reply;
        bson_error_t error;
        if (!libmongoc::client_session_commit_transaction(
                _session_t.get(), reply.bson_for_init(), &error)) {
            throw_exception<operation_exception>(reply.steal(), error);
        }
    }

    void abort_transaction() {
        bson_error_t error;
        if (!libmongoc::client_session_abort_transaction(_session_t.get(), &error)) {
            throw_exception<operation_exception>(error);
        }
    }

    bsoncxx::document::value to_document() const {
        bson_error_t error;
        bson_t bson = BSON_INITIALIZER;
        if (!libmongoc::client_session_append(_session_t.get(), &bson, &error)) {
            throw mongocxx::logic_error{error_code::k_invalid_session, error.message};
        }

        // document::value takes ownership of the bson buffer.
        return bsoncxx::helpers::value_from_bson_t(&bson);
    }

    mongoc_client_session_t* get_session_t() const noexcept {
        return _session_t.get();
    }

   private:
    const class client* _client;
    options::client_session _options;

    using unique_session =
        std::unique_ptr<mongoc_client_session_t,
                        std::function<void MONGOCXX_CALL(mongoc_client_session_t*)>>;

    unique_session _session_t;

    bson_t _empty_cluster_time = BSON_INITIALIZER;
};

MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx

#include <mongocxx/config/private/postlude.hh>
