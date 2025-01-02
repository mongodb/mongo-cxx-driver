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

#include <string>
#include <vector>

#include <bsoncxx/stdx/string_view.hpp>

#include <mongocxx/uri.hpp>

#include <examples/api/runner.hh>
#include <examples/macros.hh>

namespace {

// [Example]
void example() {
    {
        mongocxx::uri uri{"mongodb://localhost:27017/"};

        std::string database = uri.database();
        EXPECT(database.empty());

        auto try_once_opt = uri.server_selection_try_once();
        EXPECT(!try_once_opt);

        auto appname_opt = uri.appname();
        EXPECT(!appname_opt);
    }

    {
        mongocxx::uri uri{"mongodb://localhost:27017/dbName?appName=example&retryReads=true"};

        auto database = uri.database();
        EXPECT(database == "dbName");

        auto retry_reads_opt = uri.retry_reads();
        EXPECT(retry_reads_opt);
        EXPECT(*retry_reads_opt == true);

        auto appname_opt = uri.appname();
        EXPECT(appname_opt);
        EXPECT(appname_opt->compare("example") == 0);
    }
}
// [Example]

} // namespace

RUNNER_REGISTER_COMPONENT_WITH_INSTANCE() {
    example();
}
