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

#include <cstring>

#include <bsoncxx/json.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/exception/error_code.hpp>
#include <mongocxx/exception/exception.hpp>
#include <mongocxx/options/client.hpp>
#include <mongocxx/options/tls.hpp>
#include <mongocxx/uri.hpp>

#include <examples/api/runner.hh>
#include <examples/macros.hh>

namespace {

// [Example]
mongocxx::client example() {
    mongocxx::options::tls tls_opts;

    // ... set TLS options.

    mongocxx::options::client client_opts;
    client_opts.tls_opts(tls_opts);

    mongocxx::uri uri{"mongodb://bob:pwd123@localhost:27017/?tls=true"};
    mongocxx::client client{uri, client_opts};

    EXPECT(client);
    EXPECT(client.uri().to_string() == uri.to_string());

    return client;
}
// [Example]

} // namespace

RUNNER_REGISTER_COMPONENT_FOR_SINGLE() {
    try {
        auto client = example();

        auto reply = client["admin"].run_command(bsoncxx::from_json(R"({"ping": 1})"));

        EXPECT(reply["ok"] && reply["ok"].get_double().value == 1.0);
    } catch (mongocxx::exception const& ex) {
        if (ex.code() == mongocxx::error_code::k_ssl_not_supported) {
            // Library may not be configured with TLS/SSL support enabled.
        } else if (std::strstr(ex.what(), "suitable server") != nullptr) {
            // Authentication may not be supported by the live server.
        } else {
            throw;
        }
    }
}
