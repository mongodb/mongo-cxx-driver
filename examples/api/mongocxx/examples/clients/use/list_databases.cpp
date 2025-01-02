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

#include <bsoncxx/document/view.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/cursor.hpp>
#include <mongocxx/uri.hpp>

#include <examples/api/runner.hh>
#include <examples/macros.hh>

namespace {

// [Example]
void example(mongocxx::client client) {
    mongocxx::cursor databases = client.list_databases();

    int count = 0;

    for (bsoncxx::document::view doc : databases) {
        EXPECT(doc["name"]);
        EXPECT(doc["sizeOnDisk"]);
        EXPECT(doc["empty"]);

        if (doc["name"].get_string().value == "admin") {
            ++count;
        }
    }

    EXPECT(count == 1);
}
// [Example]

} // namespace

RUNNER_REGISTER_COMPONENT_FOR_SINGLE() {
    example(mongocxx::client{mongocxx::uri{}});
}
