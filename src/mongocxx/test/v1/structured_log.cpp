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

#include <mongocxx/v1/structured_log.hh>

//

#include <bsoncxx/v1/stdx/string_view.hpp>

#include <cstring>

#include <bsoncxx/private/bson.hh>

#include <mongocxx/private/mongoc.hh>

#include <catch2/catch_test_macros.hpp>

namespace mongocxx {
namespace v1 {

namespace {

// An opaque tag used as the identity of a fabricated mongoc_structured_log_entry_t.
struct identity_type {};

} // namespace

TEST_CASE("structured_log_level to_string", "[mongocxx][test][v1][structured_log]") {
    CHECK(to_string(structured_log_level::k_emergency) == "emergency");
    CHECK(to_string(structured_log_level::k_alert) == "alert");
    CHECK(to_string(structured_log_level::k_critical) == "critical");
    CHECK(to_string(structured_log_level::k_error) == "error");
    CHECK(to_string(structured_log_level::k_warning) == "warning");
    CHECK(to_string(structured_log_level::k_notice) == "notice");
    CHECK(to_string(structured_log_level::k_info) == "info");
    CHECK(to_string(structured_log_level::k_debug) == "debug");
    CHECK(to_string(structured_log_level::k_trace) == "trace");
}

TEST_CASE("structured_log_component to_string", "[mongocxx][test][v1][structured_log]") {
    // Names must match the standardized logging specification.
    CHECK(to_string(structured_log_component::k_command) == "command");
    CHECK(to_string(structured_log_component::k_topology) == "topology");
    CHECK(to_string(structured_log_component::k_server_selection) == "serverSelection");
    CHECK(to_string(structured_log_component::k_connection) == "connection");
}

TEST_CASE("structured_log_level_from_string", "[mongocxx][test][v1][structured_log]") {
    SECTION("round-trips with to_string") {
        for (auto const level : {
                 structured_log_level::k_emergency,
                 structured_log_level::k_alert,
                 structured_log_level::k_critical,
                 structured_log_level::k_error,
                 structured_log_level::k_warning,
                 structured_log_level::k_notice,
                 structured_log_level::k_info,
                 structured_log_level::k_debug,
                 structured_log_level::k_trace,
             }) {
            auto const parsed = structured_log_level_from_string(to_string(level));
            REQUIRE(parsed);
            CHECK(*parsed == level);
        }
    }

    SECTION("is case-insensitive") {
        auto const parsed = structured_log_level_from_string("DEBUG");
        REQUIRE(parsed);
        CHECK(*parsed == structured_log_level::k_debug);
    }

    SECTION("returns nullopt for an unknown name") {
        CHECK(!structured_log_level_from_string("not-a-level"));
    }
}

TEST_CASE("structured_log_component_from_string", "[mongocxx][test][v1][structured_log]") {
    SECTION("round-trips with to_string") {
        for (auto const component : {
                 structured_log_component::k_command,
                 structured_log_component::k_topology,
                 structured_log_component::k_server_selection,
                 structured_log_component::k_connection,
             }) {
            auto const parsed = structured_log_component_from_string(to_string(component));
            REQUIRE(parsed);
            CHECK(*parsed == component);
        }
    }

    SECTION("returns nullopt for an unknown name") {
        CHECK(!structured_log_component_from_string("not-a-component"));
    }
}

TEST_CASE("structured_log_entry accessors", "[mongocxx][test][v1][structured_log]") {
    identity_type identity;
    auto const entry =
        structured_log_entry::internal::make(reinterpret_cast<mongoc_structured_log_entry_t const*>(&identity));

    SECTION("level") {
        auto get_level = libmongoc::structured_log_entry_get_level.create_instance();
        get_level->interpose([&](mongoc_structured_log_entry_t const* ptr) -> mongoc_structured_log_level_t {
            CHECK(static_cast<void const*>(ptr) == &identity);
            return MONGOC_STRUCTURED_LOG_LEVEL_DEBUG;
        });
        CHECK(entry.level() == structured_log_level::k_debug);
    }

    SECTION("component") {
        auto get_component = libmongoc::structured_log_entry_get_component.create_instance();
        get_component->interpose([&](mongoc_structured_log_entry_t const* ptr) -> mongoc_structured_log_component_t {
            CHECK(static_cast<void const*>(ptr) == &identity);
            return MONGOC_STRUCTURED_LOG_COMPONENT_SERVER_SELECTION;
        });
        CHECK(entry.component() == structured_log_component::k_server_selection);
    }

    SECTION("message") {
        auto get_message = libmongoc::structured_log_entry_get_message_string.create_instance();
        get_message->interpose([&](mongoc_structured_log_entry_t const* ptr) -> char const* {
            CHECK(static_cast<void const*>(ptr) == &identity);
            return "Command started";
        });
        CHECK(entry.message() == bsoncxx::v1::stdx::string_view{"Command started"});
    }

    SECTION("message_as_bson yields an owned copy") {
        bson_t* const expected = BCON_NEW("message", "Command started", "commandName", "find");

        auto message_as_bson = libmongoc::structured_log_entry_message_as_bson.create_instance();
        message_as_bson->interpose([&](mongoc_structured_log_entry_t const* ptr) -> bson_t* {
            CHECK(static_cast<void const*>(ptr) == &identity);
            // The caller (mongocxx) takes ownership; hand back a fresh allocation.
            return bson_copy(expected);
        });

        auto const doc = entry.message_as_bson();
        auto const view = doc.view();
        REQUIRE(view.length() == expected->len);
        CHECK(std::memcmp(view.data(), bson_get_data(expected), expected->len) == 0);

        bson_destroy(expected);
    }
}

} // namespace v1
} // namespace mongocxx
