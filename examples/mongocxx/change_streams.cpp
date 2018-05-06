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

#include <iostream>

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/string/to_string.hpp>
#include <mongocxx/change_stream.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/pool.hpp>
#include <mongocxx/uri.hpp>

namespace {

std::string get_server_version(const mongocxx::client& client) {
    bsoncxx::builder::basic::document server_status{};
    server_status.append(bsoncxx::builder::basic::kvp("serverStatus", 1));
    bsoncxx::document::value output = client["test"].run_command(server_status.extract());

    return bsoncxx::string::to_string(output.view()["version"].get_utf8().value);
}

void watch_until(const mongocxx::client& client,
                 const std::chrono::time_point<std::chrono::system_clock> end) {
    mongocxx::options::change_stream options;
    // Wait up to 1 second before polling again.
    const std::chrono::milliseconds await_time{1000};
    options.max_await_time(await_time);

    auto collection = client["db"]["coll"];
    mongocxx::change_stream stream = collection.watch(options);

    while (std::chrono::system_clock::now() < end) {
        for (const auto& event : stream) {
            std::cout << bsoncxx::to_json(event) << std::endl;
            if (std::chrono::system_clock::now() >= end)
                break;
        }
    }
}

}  // namespace

int main() {
    mongocxx::instance inst{};
    mongocxx::pool pool{mongocxx::uri{}};

    try {
        auto entry = pool.acquire();

        if (get_server_version(*entry) < "3.6") {
            std::cerr << "Change streams are only supported on Mongo versions >= 3.6." << std::endl;
            // CXX-1548: Should return EXIT_FAILURE, but Travis is currently running Mongo 3.4
            return EXIT_SUCCESS;
        }

        // End in 10 seconds:
        const auto end = std::chrono::system_clock::now() + std::chrono::seconds{10};

        watch_until(*entry, end);

        return EXIT_SUCCESS;
    } catch (const std::exception& exception) {
        std::cerr << "Caught exception \"" << exception.what() << "\"" << std::endl;
    } catch (...) {
        std::cerr << "Caught unknown exception type" << std::endl;
    }

    return EXIT_FAILURE;
}
