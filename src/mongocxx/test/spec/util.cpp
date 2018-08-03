// Copyright 2018-present MongoDB Inc.
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

#include <mongocxx/test/spec/util.hh>

#include <bsoncxx/json.hpp>
#include <bsoncxx/string/to_string.hpp>
#include <mongocxx/test_util/client_helpers.hh>
#include <third_party/catch/include/catch.hpp>

#include <mongocxx/config/private/prelude.hh>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN
namespace spec {

using namespace mongocxx;
using namespace bsoncxx;

bool should_skip_spec_test(const client& client, document::view test) {
    std::string server_version = test_util::get_server_version(client);

    // "If topology does not include the topology of the server instance(s), skip this test."
    if (test["topology"]) {
        auto required_topologies = test["topology"].get_array().value;
        auto topology = test_util::get_topology(client);
        bool found = false;
        for (auto&& el : required_topologies) {
            if (std::string(el.get_utf8().value) == topology) {
                found = true;
                break;
            }
        }
        if (!found) {
            WARN("Skipping - supported topologies are: " + to_json(required_topologies));
            return true;
        }
    }

    if (test["minServerVersion"]) {
        auto min_server_version = string::to_string(test["minServerVersion"].get_utf8().value);
        if (test_util::compare_versions(server_version, min_server_version) < 0) {
            WARN("Skipping - server is version: " + server_version + " but must be higher than " +
                 min_server_version);
            return true;
        }
    }

    if (test["maxServerVersion"]) {
        auto max_server_version = string::to_string(test["maxServerVersion"].get_utf8().value);
        if (test_util::compare_versions(server_version, max_server_version) > 0) {
            WARN("Skipping - server is version: " + server_version + " but must be lower than " +
                 max_server_version);
            return true;
        }
    }
    return false;
}

}  // namespace spec
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx

#include <mongocxx/config/private/postlude.hh>
