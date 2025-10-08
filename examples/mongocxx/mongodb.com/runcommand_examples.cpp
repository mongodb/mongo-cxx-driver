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

#include <iostream>

#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>

#include <examples/macros.hh>

namespace {

void runcommand_examples(mongocxx::database& db) {
    {
        // Start runCommand Example 1
        using namespace bsoncxx::builder::basic;
        auto buildInfo = db.run_command(make_document(kvp("buildInfo", 1)));
        // End runCommand Example 1

        if (buildInfo.view()["ok"].get_double() != 1.0) {
            throw std::logic_error("buildInfo command failed in runCommand example 1");
        }
    }
}

} // namespace

int EXAMPLES_CDECL main() {
    // The mongocxx::instance constructor and destructor initialize and shut down the driver,
    // respectively. Therefore, a mongocxx::instance must be created before using the driver and
    // must remain alive for as long as the driver is in use.
    mongocxx::instance const inst{};

    mongocxx::client const conn{mongocxx::uri{}};
    auto db = conn["documentation_examples"];

    try {
        runcommand_examples(db);
    } catch (std::logic_error const& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
