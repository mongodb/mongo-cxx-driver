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
#include <mongocxx/private/client.hpp>
#include <mongocxx/private/pool.hpp>
#include <mongocxx/private/ssl.hpp>
#include <mongocxx/private/uri.hpp>

#include <mongocxx/config/private/prelude.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

void pool::_release(client* client) {
    libmongoc::client_pool_push(_impl->client_pool_t, client->_get_impl().client_t);
    // prevent client destructor from destroying the underlying mongoc_client_t
    client->_get_impl().client_t = nullptr;
    delete client;
}

pool::~pool() = default;

pool::pool(const uri& uri, stdx::optional<options::ssl> ssl_options)
    : _impl{stdx::make_unique<impl>(libmongoc::client_pool_new(uri._impl->uri_t))} {
#if !defined(MONGOC_ENABLE_SSL)
    if (uri.ssl() || ssl_options) throw exception{error_code::k_ssl_not_supported};
#else
    if (ssl_options) {
        if (!uri.ssl())
            throw exception{error_code::k_invalid_parameter,
                            "cannot set SSL options if 'ssl=true' not in URI"};

        auto mongoc_opts = options::make_ssl_opts(*ssl_options);
        _impl->ssl_options = std::move(mongoc_opts.second);
        libmongoc::client_pool_set_ssl_opts(_impl->client_pool_t, &mongoc_opts.first);
    }
#endif
}

pool::entry pool::acquire() {
    return entry(new client(libmongoc::client_pool_pop(_impl->client_pool_t)),
                 [this](client* client) { _release(client); });
}

stdx::optional<pool::entry> pool::try_acquire() {
    auto cli = libmongoc::client_pool_try_pop(_impl->client_pool_t);
    if (!cli) return stdx::nullopt;

    return pool::entry{new client(cli), [this](client* client) { _release(client); }};
}

MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
