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

#include <mongocxx/v1/structured_logging.hh>

//

#include <mongocxx/v1/structured_log.hpp>

#include <cstddef>

#include <mongocxx/private/mongoc.hh>

#include <catch2/catch_test_macros.hpp>

namespace mongocxx {
namespace v1 {

namespace {

// Opaque tags used as identities for fabricated mongoc pointers.
struct opts_identity {};
struct client_identity {};
struct pool_identity {};
struct entry_identity {};

mongoc_structured_log_opts_t* fake_opts(opts_identity* id) {
    return reinterpret_cast<mongoc_structured_log_opts_t*>(id);
}

} // namespace

TEST_CASE("structured_logging builder", "[mongocxx][test][v1][structured_logging]") {
    structured_logging cfg;

    SECTION("defaults are empty") {
        CHECK(!cfg.handler());
        CHECK(!cfg.max_level_for_all_components());
        CHECK(!cfg.max_level_for_component(structured_log_component::k_command));
        CHECK(!cfg.max_document_length());
    }

    SECTION("setters are reflected by getters") {
        cfg.max_level_for_all_components(structured_log_level::k_warning)
            .max_level_for_component(structured_log_component::k_topology, structured_log_level::k_debug)
            .max_document_length(500);

        CHECK(cfg.max_level_for_all_components() == structured_log_level::k_warning);
        CHECK(cfg.max_level_for_component(structured_log_component::k_topology) == structured_log_level::k_debug);
        CHECK(!cfg.max_level_for_component(structured_log_component::k_command));
        CHECK(cfg.max_document_length() == std::size_t{500});

        bool called = false;
        cfg.handler([&](structured_log_entry const&) { called = true; });
        CHECK(static_cast<bool>(cfg.handler()));
        CHECK(!called);
    }

    SECTION("is copyable") {
        cfg.max_document_length(500);
        auto const copy = cfg;
        CHECK(copy.max_document_length() == std::size_t{500});
    }
}

TEST_CASE("structured_logging apply_to", "[mongocxx][test][v1][structured_logging]") {
    opts_identity opts_id;

    auto opts_new = libmongoc::structured_log_opts_new.create_instance();
    opts_new->interpose([&]() -> mongoc_structured_log_opts_t* { return fake_opts(&opts_id); });

    auto opts_destroy = libmongoc::structured_log_opts_destroy.create_instance();
    bool destroyed = false;
    opts_destroy->interpose([&](mongoc_structured_log_opts_t* opts) -> void {
        CHECK(opts == fake_opts(&opts_id));
        destroyed = true;
    });

    SECTION("client: installs opts and handler") {
        int calls = 0;
        structured_logging cfg;
        cfg.handler([&](structured_log_entry const&) { ++calls; })
            .max_level_for_all_components(structured_log_level::k_debug)
            .max_level_for_component(structured_log_component::k_command, structured_log_level::k_trace)
            .max_document_length(2000);

        bool all_set = false;
        auto set_all = libmongoc::structured_log_opts_set_max_level_for_all_components.create_instance();
        set_all->interpose([&](mongoc_structured_log_opts_t*, mongoc_structured_log_level_t level) -> bool {
            all_set = true;
            CHECK(level == MONGOC_STRUCTURED_LOG_LEVEL_DEBUG);
            return true;
        });

        bool comp_set = false;
        auto set_comp = libmongoc::structured_log_opts_set_max_level_for_component.create_instance();
        set_comp->interpose(
            [&](mongoc_structured_log_opts_t*,
                mongoc_structured_log_component_t component,
                mongoc_structured_log_level_t level) -> bool {
                comp_set = true;
                CHECK(component == MONGOC_STRUCTURED_LOG_COMPONENT_COMMAND);
                CHECK(level == MONGOC_STRUCTURED_LOG_LEVEL_TRACE);
                return true;
            });

        bool doclen_set = false;
        auto set_doclen = libmongoc::structured_log_opts_set_max_document_length.create_instance();
        set_doclen->interpose([&](mongoc_structured_log_opts_t*, std::size_t length) -> bool {
            doclen_set = true;
            CHECK(length == std::size_t{2000});
            return true;
        });

        mongoc_structured_log_func_t captured_func = nullptr;
        void* captured_user_data = nullptr;
        auto set_handler = libmongoc::structured_log_opts_set_handler.create_instance();
        set_handler->interpose(
            [&](mongoc_structured_log_opts_t*, mongoc_structured_log_func_t func, void* user_data) -> void {
                captured_func = func;
                captured_user_data = user_data;
            });

        bool client_set = false;
        auto client_set_opts = libmongoc::client_set_structured_log_opts.create_instance();
        client_set_opts->interpose([&](mongoc_client_t*, mongoc_structured_log_opts_t const* opts) -> bool {
            client_set = true;
            CHECK(opts == fake_opts(&opts_id));
            return true;
        });

        client_identity client_id;
        structured_logging::internal::apply_to(reinterpret_cast<mongoc_client_t*>(&client_id), cfg);

        CHECK(all_set);
        CHECK(comp_set);
        CHECK(doclen_set);
        CHECK(client_set);
        CHECK(destroyed);

        // The handler user_data must be the stable address of the stored configuration.
        REQUIRE(captured_func != nullptr);
        CHECK(captured_user_data == &cfg);

        // Simulate mongoc invoking the handler and confirm it routes to the C++ functor.
        entry_identity entry_id;
        captured_func(reinterpret_cast<mongoc_structured_log_entry_t const*>(&entry_id), captured_user_data);
        CHECK(calls == 1);
    }

    SECTION("empty handler disables logging (NULL func)") {
        structured_logging cfg; // no handler

        bool handler_disabled = false;
        auto set_handler = libmongoc::structured_log_opts_set_handler.create_instance();
        set_handler->interpose(
            [&](mongoc_structured_log_opts_t*, mongoc_structured_log_func_t func, void* user_data) -> void {
                handler_disabled = (func == nullptr);
                CHECK(user_data == nullptr);
            });

        auto client_set_opts = libmongoc::client_set_structured_log_opts.create_instance();
        client_set_opts->interpose([&](mongoc_client_t*, mongoc_structured_log_opts_t const*) -> bool { return true; });

        client_identity client_id;
        structured_logging::internal::apply_to(reinterpret_cast<mongoc_client_t*>(&client_id), cfg);

        CHECK(handler_disabled);
        CHECK(destroyed);
    }

    SECTION("pool: uses the pool set entry point") {
        structured_logging cfg;
        cfg.handler([](structured_log_entry const&) {});

        auto set_handler = libmongoc::structured_log_opts_set_handler.create_instance();
        set_handler->interpose([](mongoc_structured_log_opts_t*, mongoc_structured_log_func_t, void*) -> void {});

        bool pool_set = false;
        auto pool_set_opts = libmongoc::client_pool_set_structured_log_opts.create_instance();
        pool_set_opts->interpose([&](mongoc_client_pool_t*, mongoc_structured_log_opts_t const* opts) -> bool {
            pool_set = true;
            CHECK(opts == fake_opts(&opts_id));
            return true;
        });

        pool_identity pool_id;
        structured_logging::internal::apply_to(reinterpret_cast<mongoc_client_pool_t*>(&pool_id), cfg);

        CHECK(pool_set);
        CHECK(destroyed);
    }
}

} // namespace v1
} // namespace mongocxx
