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

#include <cassert>

#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>

// NOTE: Any time this file is modified, a DOCS ticket should be opened to sync the changes with the
// corresponding page on docs.mongodb.com. See CXX-1514, CXX-1249, and DRIVERS-356 for more info.

void runcommand_examples(mongocxx::database& db) {
    {
        // Start runCommand Example 1
        using namespace bsoncxx::builder::basic;
        auto buildInfo = db.run_command(make_document(kvp("buildInfo", 1)));
        // End runCommand Example 1

        assert(buildInfo.view()["ok"].get_double() == double{1});
    }

    {
        using namespace bsoncxx::builder::basic;
        // drop and recreate dummy data so command succeeds
        db["restaurants"].drop();
        db["restaurants"].insert_one(make_document(kvp("name", "Noodle-Sushi")));

        // Start runCommand Example 2
        using namespace bsoncxx::builder::basic;
        auto buildInfo = db.run_command(make_document(kvp("collStats", "restaurants")));
        // End runCommand Example 1

        assert(buildInfo.view()["ok"].get_double() == double{1});
    }
}

int main() {
    // The mongocxx::instance constructor and destructor initialize and shut down the driver,
    // respectively. Therefore, a mongocxx::instance must be created before using the driver and
    // must remain alive for as long as the driver is in use.
    const mongocxx::instance inst{};

    const mongocxx::client conn{mongocxx::uri{}};
    auto db = conn["documentation_examples"];
    runcommand_examples(db);

    return EXIT_SUCCESS;
}