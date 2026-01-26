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

#include <mongocxx/v1/exception.hpp>

#include <mongocxx/pool.hh>

//

#include <mongocxx/v1/client.hh>
#include <mongocxx/v1/pool.hh>

#include <cstddef>
#include <utility>

#include <bsoncxx/stdx/optional.hpp>

#include <mongocxx/exception/error_code.hpp>
#include <mongocxx/exception/exception.hpp>
#include <mongocxx/exception/operation_exception.hpp>
#include <mongocxx/options/apm.hpp>
#include <mongocxx/options/pool.hpp>

#include <mongocxx/client.hh>
#include <mongocxx/mongoc_error.hh>
#include <mongocxx/options/auto_encryption.hh>
#include <mongocxx/options/server_api.hh>
#include <mongocxx/options/tls.hh>
#include <mongocxx/uri.hh>

#include <bsoncxx/private/bson.hh>

#include <mongocxx/private/mongoc.hh>
#include <mongocxx/private/ssl.hh>

namespace mongocxx {
namespace v_noabi {

pool::pool(v_noabi::uri const& mongodb_uri, v_noabi::options::pool const& options) try
    : _pool{v_noabi::uri::internal::as_v1(mongodb_uri)} {
    auto const ptr = v1::pool::internal::as_mongoc(_pool);

    auto const& client_opts = options.client_opts();

    if (auto const& opt = client_opts.apm_opts()) {
        v1::pool::internal::set_apm(_pool, v_noabi::to_v1(*opt));
    }

    if (auto const& opt = client_opts.auto_encryption_opts()) {
        bson_error_t error = {};

        if (!libmongoc::client_pool_enable_auto_encryption(
                ptr, v_noabi::options::auto_encryption::internal::to_mongoc(*opt).get(), &error)) {
            v_noabi::throw_exception<v_noabi::operation_exception>(error);
        }
    }

    if (auto const& opt = client_opts.server_api_opts()) {
        bson_error_t error = {};

        if (!libmongoc::client_pool_set_server_api(
                ptr, v_noabi::options::server_api::internal::to_mongoc(*opt).get(), &error)) {
            v_noabi::throw_exception<v_noabi::operation_exception>(error);
        }
    }

    {
        auto const tls_enabled = mongodb_uri.tls();

#if MONGOCXX_SSL_IS_ENABLED()
        if (auto const& opt = client_opts.tls_opts()) {
            if (!tls_enabled) {
                throw v_noabi::exception{
                    v_noabi::error_code::k_invalid_parameter, "cannot set TLS options if 'tls=true' not in URI"};
            }

            auto const v = v_noabi::options::tls::internal::to_mongoc(*opt);
            libmongoc::client_pool_set_ssl_opts(ptr, &v.opt);
        }
#else
        if (tls_enabled || client_opts.tls_opts()) {
            throw v_noabi::exception{v_noabi::error_code::k_ssl_not_supported};
        }
#endif
    }
} catch (v1::exception const& ex) {
    v_noabi::throw_exception<v_noabi::operation_exception>(ex);
}

pool::entry pool::acquire() {
    auto const ptr = v1::pool::internal::as_mongoc(_pool);

    if (auto const client = libmongoc::client_pool_pop(ptr)) {
        return entry::internal::make(v1::client::internal::make(client), ptr);
    }

    throw v_noabi::exception{
        v_noabi::error_code::k_pool_wait_queue_timeout,
        "failed to acquire client, possibly due to parameter 'waitQueueTimeoutMS' limits."};
}

bsoncxx::v_noabi::stdx::optional<pool::entry> pool::try_acquire() {
    bsoncxx::v_noabi::stdx::optional<pool::entry> ret;

    auto const ptr = v1::pool::internal::as_mongoc(_pool);

    if (auto const client = libmongoc::client_pool_try_pop(ptr)) {
        return entry::internal::make(v1::client::internal::make(client), ptr);
    }

    return ret;
}

mongoc_client_pool_t* pool::internal::as_mongoc(pool& self) {
    return v1::pool::internal::as_mongoc(self._pool);
}

pool::entry pool::entry::internal::make(v_noabi::client client, mongoc_client_pool_t* pool) {
    return ptr_type{new v_noabi::client{std::move(client)}, [pool](v_noabi::client* ptr) {
                        if (ptr) {
                            libmongoc::client_pool_push(pool, v_noabi::client::internal::release(*ptr));
                            delete ptr; // NOLINT(cppcoreguidelines-owning-memory): custom deleter.
                        }
                    }};
}

} // namespace v_noabi
} // namespace mongocxx
