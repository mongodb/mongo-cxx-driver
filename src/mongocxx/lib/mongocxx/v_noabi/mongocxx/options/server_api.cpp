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

#include <string>
#include <system_error>

#include <bsoncxx/stdx/string_view.hpp>

#include <mongocxx/exception/error_code.hpp>
#include <mongocxx/exception/logic_error.hpp>

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

} // namespace options
} // namespace v_noabi
} // namespace mongocxx
