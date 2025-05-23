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

#include <algorithm>
#include <cstring>

#include <bsoncxx/document/view.hpp>
#include <bsoncxx/json.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/database.hpp>
#include <mongocxx/exception/operation_exception.hpp>
#include <mongocxx/exception/server_error_code.hpp>
#include <mongocxx/uri.hpp>

#include <examples/api/runner.hh>
#include <examples/macros.hh>

namespace {

// [Example]
void example(mongocxx::database db) {
    EXPECT(db.name() == "db");

    // The `getParameter` command can only be run in the `admin` database.
    auto cmd = bsoncxx::from_json(R"({"getParameter": "*"})");

    // This error handling pattern applies to all commands which may throw a
    // `mongocxx::operation_exception` exception.
    try {
        auto reply = db.run_command(cmd.view());

        EXPECT(false && "should not reach this point");
    } catch (mongocxx::operation_exception const& ex) {
        EXPECT(ex.code().category() == mongocxx::server_error_category());
        EXPECT(ex.code().value() == 13); // Unauthorized
        EXPECT(std::strstr(ex.what(), "admin") != nullptr);

        if (auto server_error_opt = ex.raw_server_error()) {
            bsoncxx::document::view server_error = server_error_opt->view();

            EXPECT(server_error["ok"]);
            EXPECT(server_error["ok"].get_double().value == 0.0);

            EXPECT(server_error["code"]);
            EXPECT(server_error["code"].get_int32().value == ex.code().value());

            EXPECT(server_error["codeName"]);
            EXPECT(server_error["errmsg"]);
            // ... other error fields.
        }
    }
}
// [Example]

} // namespace

RUNNER_REGISTER_COMPONENT_FOR_SINGLE() {
    mongocxx::client client{mongocxx::uri{}};

    example(client["db"]);
}
