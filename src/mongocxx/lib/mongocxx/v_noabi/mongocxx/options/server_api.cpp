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

#include <mongocxx/options/server_api.hh>

//

#include <memory>
#include <string>
#include <system_error>

#include <bsoncxx/stdx/string_view.hpp>

#include <mongocxx/exception/error_code.hpp>
#include <mongocxx/exception/logic_error.hpp>

#include <mongocxx/private/mongoc.hh>

namespace mongocxx {
namespace v_noabi {
namespace options {

std::string server_api::version_to_string(server_api::version version) {
    switch (version) {
        case version::k_version_1:
            return "1";
        default:
            throw v_noabi::logic_error{v_noabi::error_code::k_invalid_parameter, "invalid server API version"};
    }
}

server_api::version server_api::version_from_string(bsoncxx::v_noabi::stdx::string_view version) {
    if (version == "1") {
        return version::k_version_1;
    }
    throw v_noabi::logic_error{v_noabi::error_code::k_invalid_parameter, "invalid server API version"};
}

std::unique_ptr<mongoc_server_api_t, server_api::internal::mongoc_server_api_deleter> server_api::internal::to_mongoc(
    server_api const& opts) {
    mongoc_server_api_version_t mongoc_api_version = {};

    // Convert version enum value to std::string then to c_str to create mongoc api version.
    auto result = libmongoc::server_api_version_from_string(
        server_api::version_to_string(opts.get_version()).c_str(), &mongoc_api_version);
    if (!result) {
        throw v_noabi::logic_error{
            v_noabi::error_code::k_invalid_parameter,
            "invalid server API version" + server_api::version_to_string(opts.get_version())};
    }

    std::unique_ptr<mongoc_server_api_t, server_api::internal::mongoc_server_api_deleter> ret{
        libmongoc::server_api_new(mongoc_api_version)};

    auto const ptr = ret.get();

    if (!ptr) {
        throw v_noabi::logic_error{v_noabi::error_code::k_create_resource_fail, "could not create server API"};
    }

    libmongoc::server_api_strict(ptr, opts._strict.value_or(false));
    libmongoc::server_api_deprecation_errors(ptr, opts._deprecation_errors.value_or(false));

    return ret;
}

} // namespace options
} // namespace v_noabi
} // namespace mongocxx
