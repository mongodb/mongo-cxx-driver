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

#pragma once

#include <mongocxx/options/apm.hpp>

//

#include <iostream>

#include <mongocxx/private/mongoc.hh>

namespace mongocxx {
namespace v_noabi {
namespace options {

using apm_unique_callbacks = std::unique_ptr<mongoc_apm_callbacks_t, decltype(libmongoc::apm_callbacks_destroy)>;

// An APM callback exiting via an exception is documented as being undefined behavior.
// For QoI, terminate the program before allowing the exception to bypass libmongoc code.
template <typename Fn>
inline void exception_guard(char const* source, Fn fn) noexcept {
    try {
        fn();
    } catch (...) {
        std::cerr << "fatal error: APM callback " << source << " exited via an exception" << std::endl;
        std::terminate();
    }
}

inline void command_started(mongoc_apm_command_started_t const* event) noexcept {
    events::command_started_event started_event(static_cast<void const*>(event));
    auto context = static_cast<apm*>(libmongoc::apm_command_started_get_context(event));
    exception_guard(__func__, [&] { context->command_started()(started_event); });
}

inline void command_failed(mongoc_apm_command_failed_t const* event) noexcept {
    events::command_failed_event failed_event(static_cast<void const*>(event));
    auto context = static_cast<apm*>(libmongoc::apm_command_failed_get_context(event));
    exception_guard(__func__, [&] { context->command_failed()(failed_event); });
}

inline void command_succeeded(mongoc_apm_command_succeeded_t const* event) noexcept {
    events::command_succeeded_event succeeded_event(static_cast<void const*>(event));
    auto context = static_cast<apm*>(libmongoc::apm_command_succeeded_get_context(event));
    exception_guard(__func__, [&] { context->command_succeeded()(succeeded_event); });
}

inline void server_closed(mongoc_apm_server_closed_t const* event) noexcept {
    events::server_closed_event e(static_cast<void const*>(event));
    auto context = static_cast<apm*>(libmongoc::apm_server_closed_get_context(event));
    exception_guard(__func__, [&] { context->server_closed()(e); });
}

inline void server_changed(mongoc_apm_server_changed_t const* event) noexcept {
    events::server_changed_event e(static_cast<void const*>(event));
    auto context = static_cast<apm*>(libmongoc::apm_server_changed_get_context(event));
    exception_guard(__func__, [&] { context->server_changed()(e); });
}

inline void server_opening(mongoc_apm_server_opening_t const* event) noexcept {
    events::server_opening_event e(static_cast<void const*>(event));
    auto context = static_cast<apm*>(libmongoc::apm_server_opening_get_context(event));
    exception_guard(__func__, [&] { context->server_opening()(e); });
}

inline void topology_closed(mongoc_apm_topology_closed_t const* event) noexcept {
    events::topology_closed_event e(static_cast<void const*>(event));
    auto context = static_cast<apm*>(libmongoc::apm_topology_closed_get_context(event));
    exception_guard(__func__, [&] { context->topology_closed()(e); });
}

inline void topology_changed(mongoc_apm_topology_changed_t const* event) noexcept {
    events::topology_changed_event e(static_cast<void const*>(event));
    auto context = static_cast<apm*>(libmongoc::apm_topology_changed_get_context(event));
    exception_guard(__func__, [&] { context->topology_changed()(e); });
}

inline void topology_opening(mongoc_apm_topology_opening_t const* event) noexcept {
    events::topology_opening_event e(static_cast<void const*>(event));
    auto context = static_cast<apm*>(libmongoc::apm_topology_opening_get_context(event));
    exception_guard(__func__, [&] { context->topology_opening()(e); });
}

inline void heartbeat_started(mongoc_apm_server_heartbeat_started_t const* event) noexcept {
    events::heartbeat_started_event started_event(static_cast<void const*>(event));
    auto context = static_cast<apm*>(libmongoc::apm_server_heartbeat_started_get_context(event));
    exception_guard(__func__, [&] { context->heartbeat_started()(started_event); });
}

inline void heartbeat_failed(mongoc_apm_server_heartbeat_failed_t const* event) noexcept {
    events::heartbeat_failed_event failed_event(static_cast<void const*>(event));
    auto context = static_cast<apm*>(libmongoc::apm_server_heartbeat_failed_get_context(event));
    exception_guard(__func__, [&] { context->heartbeat_failed()(failed_event); });
}

inline void heartbeat_succeeded(mongoc_apm_server_heartbeat_succeeded_t const* event) noexcept {
    events::heartbeat_succeeded_event succeeded_event(static_cast<void const*>(event));
    auto context = static_cast<apm*>(libmongoc::apm_server_heartbeat_succeeded_get_context(event));
    exception_guard(__func__, [&] { context->heartbeat_succeeded()(succeeded_event); });
}

inline apm_unique_callbacks make_apm_callbacks(apm const& apm_opts) {
    mongoc_apm_callbacks_t* callbacks = libmongoc::apm_callbacks_new();

    if (apm_opts.command_started()) {
        libmongoc::apm_set_command_started_cb(callbacks, command_started);
    }

    if (apm_opts.command_failed()) {
        libmongoc::apm_set_command_failed_cb(callbacks, command_failed);
    }

    if (apm_opts.command_succeeded()) {
        libmongoc::apm_set_command_succeeded_cb(callbacks, command_succeeded);
    }

    if (apm_opts.server_closed()) {
        libmongoc::apm_set_server_closed_cb(callbacks, server_closed);
    }

    if (apm_opts.server_changed()) {
        libmongoc::apm_set_server_changed_cb(callbacks, server_changed);
    }

    if (apm_opts.server_opening()) {
        libmongoc::apm_set_server_opening_cb(callbacks, server_opening);
    }

    if (apm_opts.topology_closed()) {
        libmongoc::apm_set_topology_closed_cb(callbacks, topology_closed);
    }

    if (apm_opts.topology_changed()) {
        libmongoc::apm_set_topology_changed_cb(callbacks, topology_changed);
    }

    if (apm_opts.topology_opening()) {
        libmongoc::apm_set_topology_opening_cb(callbacks, topology_opening);
    }

    if (apm_opts.heartbeat_started()) {
        libmongoc::apm_set_server_heartbeat_started_cb(callbacks, heartbeat_started);
    }

    if (apm_opts.heartbeat_failed()) {
        libmongoc::apm_set_server_heartbeat_failed_cb(callbacks, heartbeat_failed);
    }

    if (apm_opts.heartbeat_succeeded()) {
        libmongoc::apm_set_server_heartbeat_succeeded_cb(callbacks, heartbeat_succeeded);
    }

    return {callbacks, libmongoc::apm_callbacks_destroy};
}

} // namespace options
} // namespace v_noabi
} // namespace mongocxx
