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

#include <mongocxx/uri.hpp>

#include <bsoncxx/stdx/make_unique.hpp>
#include <mongocxx/exception/error_code.hpp>
#include <mongocxx/exception/logic_error.hpp>
#include <mongocxx/private/libmongoc.hh>
#include <mongocxx/private/read_concern.hh>
#include <mongocxx/private/read_preference.hh>
#include <mongocxx/private/uri.hh>
#include <mongocxx/private/write_concern.hh>

#include <mongocxx/config/private/prelude.hh>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN

namespace {

// Some of the 'uri_get_*' string accessors may return nullptr.  Check for this case and convert to
// the empty string.
std::string to_string_null_safe(const char* str) {
    if (str == nullptr) {
        return std::string{};
    }
    return str;
}

}  // namespace

const std::string uri::k_default_uri = "mongodb://localhost:27017";

uri::uri(std::unique_ptr<impl>&& implementation) {
    _impl.reset(implementation.release());
}

uri::uri(bsoncxx::string::view_or_value uri_string)
    : _impl(stdx::make_unique<impl>(libmongoc::uri_new(uri_string.terminated().data()))) {
    if (_impl->uri_t == nullptr) {
        throw logic_error{error_code::k_invalid_uri};
    }
}

uri::uri(uri&&) noexcept = default;
uri& uri::operator=(uri&&) noexcept = default;

uri::~uri() = default;

std::string uri::auth_mechanism() const {
    return to_string_null_safe(libmongoc::uri_get_auth_mechanism(_impl->uri_t));
}

std::string uri::auth_source() const {
    return libmongoc::uri_get_auth_source(_impl->uri_t);
}

std::string uri::database() const {
    return to_string_null_safe(libmongoc::uri_get_database(_impl->uri_t));
}

std::vector<uri::host> uri::hosts() const {
    std::vector<host> result;

    for (auto host_list = libmongoc::uri_get_hosts(_impl->uri_t); host_list;
         host_list = host_list->next) {
        result.push_back(host{host_list->host, host_list->port, host_list->family});
    }

    return result;
}

bsoncxx::document::view uri::options() const {
    auto opts_bson = libmongoc::uri_get_options(_impl->uri_t);
    return bsoncxx::document::view{::bson_get_data(opts_bson), opts_bson->len};
}

std::string uri::password() const {
    return to_string_null_safe(libmongoc::uri_get_password(_impl->uri_t));
}

class read_concern uri::read_concern() const {
    auto rc = libmongoc::uri_get_read_concern(_impl->uri_t);
    return (class read_concern)(
        stdx::make_unique<read_concern::impl>(libmongoc::read_concern_copy(rc)));
}

class read_preference uri::read_preference() const {
    auto rp = libmongoc::uri_get_read_prefs_t(_impl->uri_t);
    return (class read_preference)(
        stdx::make_unique<read_preference::impl>(libmongoc::read_prefs_copy(rp)));
}

std::string uri::replica_set() const {
    return to_string_null_safe(libmongoc::uri_get_replica_set(_impl->uri_t));
}

std::string uri::to_string() const {
    return libmongoc::uri_get_string(_impl->uri_t);
}

bool uri::ssl() const {
    return libmongoc::uri_get_ssl(_impl->uri_t);
}

std::string uri::username() const {
    return to_string_null_safe(libmongoc::uri_get_username(_impl->uri_t));
}

class write_concern uri::write_concern() const {
    auto wc = libmongoc::uri_get_write_concern(_impl->uri_t);
    return (class write_concern)(
        stdx::make_unique<write_concern::impl>(libmongoc::write_concern_copy(wc)));
}

MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
