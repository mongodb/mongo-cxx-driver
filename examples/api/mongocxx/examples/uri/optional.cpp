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
        ASSERT(database.empty());

        auto try_once_opt = uri.server_selection_try_once();
        ASSERT(!try_once_opt);

        auto appname_opt = uri.appname();
        ASSERT(!appname_opt);
    }

    {
        mongocxx::uri uri{
            "mongodb://localhost:27017/dbName?appName=example&serverSelectionTryOnce=false"};

        auto database = uri.database();
        ASSERT(database.compare("dbName") == 0);

        auto try_once_opt = uri.server_selection_try_once();
        ASSERT(try_once_opt);
        ASSERT(*try_once_opt == false);

        auto appname_opt = uri.appname();
        ASSERT(appname_opt);
        ASSERT(appname_opt->compare("example") == 0);
    }
}
// [Example]

}  // namespace

RUNNER_REGISTER_COMPONENT_WITH_INSTANCE() {
    example();
}
