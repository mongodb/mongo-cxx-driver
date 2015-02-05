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

#include <mongo/driver/uri.hpp>

#include <mongo/driver/private/uri.hpp>
#include <mongo/driver/private/libmongoc.hpp>

#include <mongo/bson/stdx/make_unique.hpp>

namespace mongo {
namespace driver {

const std::string uri::k_default_uri = "mongodb://localhost:27017";

uri::uri(const std::string& uri_string)
    : _impl(stdx::make_unique<impl>(mongoc_uri_new(uri_string.c_str()))) {
}

uri::uri(uri&&) noexcept = default;
uri& uri::operator=(uri&&) noexcept = default;

uri::~uri() = default;

std::string uri::auth_mechanism() const {
    return mongoc_uri_get_auth_mechanism(_impl->uri_t);
}

std::string uri::auth_source() const {
    return mongoc_uri_get_auth_source(_impl->uri_t);
}

std::string uri::database() const {
    return mongoc_uri_get_database(_impl->uri_t);
}

std::vector<uri::host> uri::hosts() const {
    std::vector<host> result;

    for (auto host_list = mongoc_uri_get_hosts(_impl->uri_t); host_list;
         host_list = host_list->next) {
        result.push_back(host{host_list->host, host_list->port, host_list->family});
    }

    return result;
}

std::string uri::password() const {
    return mongoc_uri_get_password(_impl->uri_t);
}

std::string uri::replica_set() const {
    return mongoc_uri_get_replica_set(_impl->uri_t);
}

std::string uri::to_string() const {
    return mongoc_uri_get_string(_impl->uri_t);
}

bool uri::ssl() const {
    return mongoc_uri_get_ssl(_impl->uri_t);
}

std::string uri::username() const {
    return mongoc_uri_get_username(_impl->uri_t);
}

// const write_concern uri::write_concern() const {
// return mongoc_uri_get_write_concern(_impl->uri_t);
//}

}  // namespace driver
}  // namespace mongo
