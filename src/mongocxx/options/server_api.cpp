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

#include <mongocxx/config/private/prelude.hh>

#include <mongocxx/exception/logic_error.hpp>
#include <mongocxx/options/server_api.hpp>
#include <mongocxx/private/libmongoc.hh>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN
namespace options {

server_api::server_api(const char* server_api_version)
    : _server_api_version(std::move(server_api_version)) {}

server_api& server_api::strict(bool strict) {
    _strict = strict;
    return *this;
}

bool server_api::strict() const {
    return _strict;
}

server_api& server_api::deprecation_errors(bool deprecation_errors) {
    _deprecation_errors = deprecation_errors;
    return *this;
}

bool server_api::deprecation_errors() const {
    return _deprecation_errors;
}

const char* server_api::server_api_version() const {
    return _server_api_version;
}

void* server_api::convert() const {
    mongoc_server_api_version_t server_api_version;
    auto r = libmongoc::server_api_version_from_string(_server_api_version, &server_api_version);
    if (!r) {
        throw std::logic_error{"invalid server API version"};
    }

    auto mongoc_server_api_opts = libmongoc::server_api_new(server_api_version);
    if (!mongoc_server_api_opts) {
        throw std::logic_error{"could not get object from libmongoc"};
    }

    // Set both strict and deprecation errors to false by default.
    libmongoc::server_api_strict(mongoc_server_api_opts, false);
    libmongoc::server_api_deprecation_errors(mongoc_server_api_opts, false);

    if (_strict) {
        libmongoc::server_api_strict(mongoc_server_api_opts, _strict);
    }
    if (_deprecation_errors) {
        libmongoc::server_api_deprecation_errors(mongoc_server_api_opts, _deprecation_errors);
    }

    return mongoc_server_api_opts;
}

}  // namespace options
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
