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

#include <mongocxx/client.hpp>

#include <mongocxx/private/client.hpp>
#include <mongocxx/private/read_preference.hpp>
#include <mongocxx/private/write_concern.hpp>
#include <mongocxx/private/uri.hpp>
#include <bsoncxx/stdx/make_unique.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

client::client(const class uri& uri, const options::client&)
    : _impl(bsoncxx::stdx::make_unique<impl>(libmongoc::client_new_from_uri(uri._impl->uri_t))) {
}

client::client(client&&) noexcept = default;
client& client::operator=(client&&) noexcept = default;

client::~client() = default;

void* client::implementation() const {
    return _impl->client_t;
}

void client::read_preference(class read_preference rp) {
    libmongoc::client_set_read_prefs(_impl->client_t, rp._impl->read_preference_t);
}

class read_preference client::read_preference() const {
    class read_preference rp(bsoncxx::stdx::make_unique<read_preference::impl>(
        libmongoc::read_prefs_copy(libmongoc::client_get_read_prefs(_impl->client_t))));
    return rp;
}

class uri client::uri() const {
    class uri connection_string(bsoncxx::stdx::make_unique<uri::impl>(
        libmongoc::uri_copy(libmongoc::client_get_uri(_impl->client_t))));
    return connection_string;
}

void client::write_concern(class write_concern wc) {
    libmongoc::client_set_write_concern(_impl->client_t, wc._impl->write_concern_t);
}

class write_concern client::write_concern() const {
    class write_concern wc(bsoncxx::stdx::make_unique<write_concern::impl>(
        libmongoc::write_concern_copy(libmongoc::client_get_write_concern(_impl->client_t))));
    return wc;
}

class database client::database(bsoncxx::stdx::string_view name) const & {
    return mongocxx::database(*this, name);
}

MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
