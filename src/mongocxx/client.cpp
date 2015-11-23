// Copyright 2014 MongoDB Inc.
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

#include <mongocxx/config/prelude.hpp>

#include <mongocxx/client.hpp>

#include <bsoncxx/stdx/make_unique.hpp>
#include <mongocxx/exception/error_category.hpp>
#include <mongocxx/exception/operation.hpp>
#include <mongocxx/exception/private/mongoc_error.hpp>
#include <mongocxx/private/client.hpp>
#include <mongocxx/private/read_preference.hpp>
#include <mongocxx/private/ssl.hpp>
#include <mongocxx/private/uri.hpp>
#include <mongocxx/private/write_concern.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

client::client() noexcept = default;

client::client(const class uri& uri, const options::client& options)
    : _impl(stdx::make_unique<impl>(libmongoc::client_new_from_uri(uri._impl->uri_t))) {
    if (options.ssl_opts()) {
        auto mongoc_opts = options::make_ssl_opts(*options.ssl_opts());
        libmongoc::client_set_ssl_opts(_impl->client_t, &mongoc_opts);
    }
}

client::client(void* implementation)
    : _impl{stdx::make_unique<impl>(static_cast<::mongoc_client_t*>(implementation))} {
}

client::client(client&&) noexcept = default;
client& client::operator=(client&&) noexcept = default;

client::~client() = default;

client::operator bool() const noexcept {
    return static_cast<bool>(_impl);
}

void* client::implementation() const {
    return _impl->client_t;
}

void client::read_preference(class read_preference rp) {
    libmongoc::client_set_read_prefs(_impl->client_t, rp._impl->read_preference_t);
}

class read_preference client::read_preference() const {
    class read_preference rp(stdx::make_unique<read_preference::impl>(
        libmongoc::read_prefs_copy(libmongoc::client_get_read_prefs(_impl->client_t))));
    return rp;
}

class uri client::uri() const {
    class uri connection_string(stdx::make_unique<uri::impl>(
        libmongoc::uri_copy(libmongoc::client_get_uri(_impl->client_t))));
    return connection_string;
}

void client::write_concern(class write_concern wc) {
    libmongoc::client_set_write_concern(_impl->client_t, wc._impl->write_concern_t);
}

class write_concern client::write_concern() const {
    class write_concern wc(stdx::make_unique<write_concern::impl>(
        libmongoc::write_concern_copy(libmongoc::client_get_write_concern(_impl->client_t))));
    return wc;
}

class database client::database(stdx::string_view name) const & {
    return mongocxx::database(*this, name);
}

cursor client::list_databases() const {
    bson_error_t error;
    auto result = libmongoc::client_find_databases(_impl->client_t, &error);

    if (!result) {
        exception::throw_exception<exception::operation>(error);
    }

    return cursor(result);
}

MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
