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
#include <mongocxx/stdx.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN
namespace options {

std::string server_api::version_to_string(server_api::version version) {
    switch (version) {
        case server_api::version::version_1:
            return "1";
        default:
            throw std::logic_error{"invalid server API version"};
    }
}

server_api::version server_api::version_from_string(std::string version) {
    if (!version.compare("1")) {
        return server_api::version::version_1;
    } else {
        throw std::logic_error{"invalid server API version"};
    }
}

server_api::server_api(server_api::version version) : _version(std::move(version)) {}

server_api& server_api::strict(bool strict) {
    _strict = strict;
    return *this;
}

const stdx::optional<bool>& server_api::strict() const {
    return _strict;
}

server_api& server_api::deprecation_errors(bool deprecation_errors) {
    _deprecation_errors = deprecation_errors;
    return *this;
}

const stdx::optional<bool>& server_api::deprecation_errors() const {
    return _deprecation_errors;
}

server_api::version server_api::api_version() const {
    return _version;
}

void* server_api::convert() const {
    mongoc_server_api_version_t mongoc_api_version;

    // Convert version enum value to std::string then to c_str to create mongoc api version.
    auto r = libmongoc::server_api_version_from_string(version_to_string(_version).c_str(),
                                                       &mongoc_api_version);
    if (!r) {
        throw std::logic_error{"invalid server API version"};
    }

    auto mongoc_server_api_opts = libmongoc::server_api_new(mongoc_api_version);
    if (!mongoc_server_api_opts) {
        throw std::logic_error{"could not get object from libmongoc"};
    }

    if (_strict) {
        libmongoc::server_api_strict(mongoc_server_api_opts, _strict.value_or(false));
    }
    if (_deprecation_errors) {
        libmongoc::server_api_deprecation_errors(mongoc_server_api_opts,
                                                 _deprecation_errors.value_or(false));
    }

    return mongoc_server_api_opts;
}

}  // namespace options
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
