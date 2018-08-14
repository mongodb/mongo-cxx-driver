// Copyright 2015 MongoDB Inc.
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

#include <mongocxx/pool.hpp>

#include <utility>

#include <bsoncxx/stdx/make_unique.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/exception/error_code.hpp>
#include <mongocxx/exception/exception.hpp>
#include <mongocxx/options/private/apm.hh>
#include <mongocxx/options/private/ssl.hh>
#include <mongocxx/private/client.hh>
#include <mongocxx/private/pool.hh>
#include <mongocxx/private/uri.hh>

#include <mongocxx/config/private/prelude.hh>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

void pool::_release(client* client) {
    libmongoc::client_pool_push(_impl->client_pool_t, client->_get_impl().client_t);
    // prevent client destructor from destroying the underlying mongoc_client_t
    client->_get_impl().client_t = nullptr;
    delete client;
}

pool::~pool() = default;

pool::pool(const uri& uri, const options::pool& options)
    : _impl{stdx::make_unique<impl>(libmongoc::client_pool_new(uri._impl->uri_t))} {
#if defined(MONGOCXX_ENABLE_SSL) && defined(MONGOC_ENABLE_SSL)
    if (options.client_opts().ssl_opts()) {
        if (!uri.ssl())
            throw exception{error_code::k_invalid_parameter,
                            "cannot set SSL options if 'ssl=true' not in URI"};

        auto mongoc_opts = options::make_ssl_opts(*options.client_opts().ssl_opts());
        _impl->ssl_options = std::move(mongoc_opts.second);
        libmongoc::client_pool_set_ssl_opts(_impl->client_pool_t, &mongoc_opts.first);
    }
#else
    if (uri.ssl() || options.client_opts().ssl_opts())
        throw exception{error_code::k_ssl_not_supported};
#endif
    if (options.client_opts().apm_opts()) {
        _impl->listeners = *options.client_opts().apm_opts();
        auto callbacks = options::make_apm_callbacks(_impl->listeners);
        // We cast the APM class to a void* so we can pass it into libmongoc's context.
        // It will be cast back to an APM class in the event handlers.
        auto context = static_cast<void*>(&(_impl->listeners));
        libmongoc::client_pool_set_apm_callbacks(_impl->client_pool_t, callbacks.get(), context);
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
    return entry(entry::unique_client(new client(libmongoc::client_pool_pop(_impl->client_pool_t)),
                                      [this](client* client) { _release(client); }));
}

stdx::optional<pool::entry> pool::try_acquire() {
    auto cli = libmongoc::client_pool_try_pop(_impl->client_pool_t);
    if (!cli)
        return stdx::nullopt;

    return entry(
        entry::unique_client(new client(cli), [this](client* client) { _release(client); }));
}

MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
