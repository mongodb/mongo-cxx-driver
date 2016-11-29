// Copyright 2016 MongoDB Inc.
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

#include <mongocxx/test_util/client_helpers.hh>

#include <bsoncxx/builder/stream/document.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/exception/error_code.hpp>
#include <mongocxx/exception/operation_exception.hpp>

#include <mongocxx/config/private/prelude.hh>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN
namespace test_util {

std::int32_t get_max_wire_version(const client& client) {
    auto reply =
        client["admin"].run_command(bsoncxx::builder::stream::document{}
                                    << "isMaster" << 1 << bsoncxx::builder::stream::finalize);
    auto max_wire_version = reply.view()["maxWireVersion"];
    if (!max_wire_version) {
        // If wire version is not available (i.e. server version too old), it is assumed to be zero.
        return 0;
    }
    if (max_wire_version.type() != bsoncxx::type::k_int32) {
        throw operation_exception{error_code::k_server_response_malformed};
    }
    return max_wire_version.get_int32().value;
}

bool supports_collation(const client& client) {
    return get_max_wire_version(client) >= 5;
}

}  // namespace test_util
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
