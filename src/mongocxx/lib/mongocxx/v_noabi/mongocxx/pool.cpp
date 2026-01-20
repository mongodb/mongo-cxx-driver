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

#include <mongocxx/client.hpp>

//

#include <mongocxx/v1/client.hh>

#include <utility>

#include <mongocxx/exception/error_code.hpp>
#include <mongocxx/exception/exception.hpp>
#include <mongocxx/exception/operation_exception.hpp>
#include <mongocxx/pool.hpp>

#include <mongocxx/client.hh>
#include <mongocxx/mongoc_error.hh>
#include <mongocxx/options/apm.hh>
#include <mongocxx/options/auto_encryption.hh>
#include <mongocxx/options/server_api.hh>
#include <mongocxx/options/tls.hh>
#include <mongocxx/pool.hh>
#include <mongocxx/uri.hh>

#include <bsoncxx/private/make_unique.hh>

#include <mongocxx/private/ssl.hh>

namespace mongocxx {
namespace v_noabi {

// Attempts to create a new client pool using the uri. Throws an exception upon error.
static mongoc_client_pool_t* construct_client_pool(mongoc_uri_t const* uri) {
    bson_error_t error;
    auto pool = libmongoc::client_pool_new_with_error(uri, &error);
    if (!pool) {
        // If constructing a client pool failed, throw an exception from the bson_error_t.
        throw_exception<operation_exception>(error);
    }

    return pool;
}

void pool::_release(client* client) {
    libmongoc::client_pool_push(_impl->client_pool_t, v_noabi::client::internal::as_mongoc(*client));
    // prevent client destructor from destroying the underlying mongoc_client_t
    v_noabi::client::internal::disown(*client);
    delete client; // NOLINT(cppcoreguidelines-owning-memory): custom deleter.
}

pool::~pool() = default;

pool::pool(uri const& uri, options::pool const& options)
    : _impl{bsoncxx::make_unique<impl>(construct_client_pool(v_noabi::uri::internal::as_mongoc(uri)))} {
#if MONGOCXX_SSL_IS_ENABLED()
    if (auto const& opts = options.client_opts().tls_opts()) {
        if (!uri.tls()) {
            throw v_noabi::exception{
                v_noabi::error_code::k_invalid_parameter, "cannot set TLS options if 'tls=true' not in URI"};
        }

        auto const v = v_noabi::options::tls::internal::to_mongoc(*opts);
        libmongoc::client_pool_set_ssl_opts(_impl->client_pool_t, &v.opt);
    }
#else
    if (uri.tls() || options.client_opts().tls_opts().has_value())
        throw v_noabi::exception{v_noabi::error_code::k_ssl_not_supported};
#endif

    if (auto const& opts = options.client_opts().auto_encryption_opts()) {
        bson_error_t error = {};

        if (!libmongoc::client_pool_enable_auto_encryption(
                _impl->client_pool_t, v_noabi::options::auto_encryption::internal::to_mongoc(*opts).get(), &error)) {
            v_noabi::throw_exception<operation_exception>(error);
        }
    }

    if (auto const& opts = options.client_opts().apm_opts()) {
        _impl->listeners = *opts;
        libmongoc::client_pool_set_apm_callbacks(
            _impl->client_pool_t,
            v_noabi::options::make_apm_callbacks(_impl->listeners).get(),
            static_cast<void*>(&_impl->listeners));
    }

    if (auto const& opts = options.client_opts().server_api_opts()) {
        bson_error_t error = {};

        if (!libmongoc::client_pool_set_server_api(
                _impl->client_pool_t, v_noabi::options::server_api::internal::to_mongoc(*opts).get(), &error)) {
            v_noabi::throw_exception<v_noabi::operation_exception>(error);
        }
    }
}

client* pool::entry::operator->() const& noexcept {
    return _client.get();
}

client& pool::entry::operator*() const& noexcept {
    return *_client;
}

pool::entry& pool::entry::operator=(std::nullptr_t) noexcept {
    _client = nullptr;
    return *this;
}

pool::entry::operator bool() const noexcept {
    return static_cast<bool>(_client);
}

// construct a pool entry from a pointer to a client
pool::entry::entry(pool::entry::unique_client p) : _client(std::move(p)) {}

pool::entry pool::acquire() {
    auto const cli = libmongoc::client_pool_pop(_impl->client_pool_t);
    if (!cli) {
        throw exception{
            error_code::k_pool_wait_queue_timeout,
            "failed to acquire client, possibly due to parameter 'waitQueueTimeoutMS' limits."};
    }

    return entry(entry::unique_client(new v_noabi::client{v1::client::internal::make(cli)}, [this](client* client) {
        _release(client);
    }));
}

bsoncxx::v_noabi::stdx::optional<pool::entry> pool::try_acquire() {
    auto const cli = libmongoc::client_pool_try_pop(_impl->client_pool_t);
    if (!cli) {
        return bsoncxx::v_noabi::stdx::nullopt;
    }

    return entry(entry::unique_client(new v_noabi::client{v1::client::internal::make(cli)}, [this](client* client) {
        _release(client);
    }));
}

} // namespace v_noabi
} // namespace mongocxx
