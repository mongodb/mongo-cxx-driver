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

#include <vector>

#include <mongocxx/uri.hpp>

#include <examples/api/runner.hh>
#include <examples/macros.hh>

namespace {

// [Example]
void example() {
    mongocxx::uri uri{"mongodb://127.0.0.1,[::1]:27018,%2Fpath%2Fto.socket:27019"};

    std::vector<mongocxx::uri::host> hosts = uri.hosts();

    EXPECT(hosts.size() == 3u);

    mongocxx::uri::host const& first = hosts[0];
    mongocxx::uri::host const& second = hosts[1];
    mongocxx::uri::host const& third = hosts[2];

    EXPECT(first.name == "127.0.0.1");
    EXPECT(first.port == 27017u);
    EXPECT(first.family == 0); // AF_UNSPEC (AP_INET).

    EXPECT(second.name == "::1");
    EXPECT(second.port == 27018u);
    EXPECT(second.family != 0); // AF_INET6.

    EXPECT(third.name == "/path/to.socket");
    EXPECT(third.port == 27019u);
    EXPECT(third.family != 0); // AF_UNIX.
}
// [Example]

} // namespace

RUNNER_REGISTER_COMPONENT_WITH_INSTANCE() {
    example();
}
