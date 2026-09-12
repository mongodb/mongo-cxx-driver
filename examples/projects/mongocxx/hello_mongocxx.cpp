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

// Support validation of install prefix with build configuration `MONGOCXX_ENABLE_UNSTABLE_ABI=OFF`.
#if !defined(MONGOCXX_ENABLE_UNSTABLE_ABI)
#define MONGOCXX_ENABLE_UNSTABLE_ABI 1
#endif

#if MONGOCXX_ENABLE_UNSTABLE_ABI == 0
#include <bsoncxx/v1/document/value.hpp>
#include <bsoncxx/v1/document/view.hpp>

#include <mongocxx/v1/client.hpp>
#include <mongocxx/v1/database.hpp>
#include <mongocxx/v1/instance.hpp>
#include <mongocxx/v1/uri.hpp>
#else
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/database.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#endif

#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <system_error>

namespace {

// clang-format off
static constexpr std::array<std::uint8_t, 15> bytes = {{
    0x0f, 0x00, 0x00, 0x00,               // (15 bytes) {
        0x10,                             //   (int32)
            0x70, 0x69, 0x6e, 0x67, 0x00, //     "ping":
            0x01, 0x00, 0x00, 0x00,       //     1
    0x00,                                 // }
}};
// clang-format on

} // namespace

int main(int argc, char** argv) {
#if MONGOCXX_ENABLE_UNSTABLE_ABI == 0
    using view_type = bsoncxx::v1::document::view;
    using instance_type = mongocxx::v1::instance;
    using uri_type = mongocxx::v1::uri;
    using client_type = mongocxx::v1::client;
#else
    using view_type = bsoncxx::document::view;
    using instance_type = mongocxx::instance;
    using uri_type = mongocxx::uri;
    using client_type = mongocxx::client;
#endif

    try {
        instance_type instance;

        client_type client{uri_type{(argc >= 2) ? argv[1] : uri_type::k_default_uri}};
        auto admin = client["admin"];

        // {"ping": 1}
        auto const reply = admin.run_command(view_type{bytes.data(), bytes.size()});
        auto const ok = static_cast<int>(reply["ok"].get_double().value);

        if (ok) {
            std::cout << "Successfully connected to a live MongoDB server!" << std::endl;
            return EXIT_SUCCESS;
        } else {
            std::cout << "Failed to ping the MongoDB server." << std::endl;
            return EXIT_FAILURE;
        }
    } catch (std::system_error const& ex) {
        static constexpr auto server_selection_failure = 13053; // MONGOC_ERROR_SERVER_SELECTION_FAILURE

        // Allow opt-out of live server requirement.
        if (std::getenv("MONGOCXX_EXAMPLE_ALLOW_NO_SERVER") != nullptr &&
            ex.code().value() == server_selection_failure) {
            std::cout << "Failed to connect to a MongoDB server, but MONGOCXX_EXAMPLE_ALLOW_NO_SERVER was set!"
                      << std::endl;
            return EXIT_SUCCESS;
        }

        std::cerr << "unexpected failure: " << ex.what() << std::endl;
        return EXIT_FAILURE;
    } catch (...) {
        std::cerr << "unexpected failure: unknown exception" << std::endl;
        return EXIT_FAILURE;
    }
}
