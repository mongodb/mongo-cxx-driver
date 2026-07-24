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

#include <bsoncxx/v1/document/value.hpp>
#include <bsoncxx/v1/stdx/optional.hpp>
#include <bsoncxx/v1/stdx/string_view.hpp>

#include <string>

#include <mongocxx/private/mongoc.hh>
#include <mongocxx/private/scoped_bson.hh>

namespace mongocxx {
namespace v1 {

static_assert(
    static_cast<int>(structured_log_level::k_emergency) == static_cast<int>(MONGOC_STRUCTURED_LOG_LEVEL_EMERGENCY),
    "");
static_assert(
    static_cast<int>(structured_log_level::k_alert) == static_cast<int>(MONGOC_STRUCTURED_LOG_LEVEL_ALERT),
    "");
static_assert(
    static_cast<int>(structured_log_level::k_critical) == static_cast<int>(MONGOC_STRUCTURED_LOG_LEVEL_CRITICAL),
    "");
static_assert(
    static_cast<int>(structured_log_level::k_error) == static_cast<int>(MONGOC_STRUCTURED_LOG_LEVEL_ERROR),
    "");
static_assert(
    static_cast<int>(structured_log_level::k_warning) == static_cast<int>(MONGOC_STRUCTURED_LOG_LEVEL_WARNING),
    "");
static_assert(
    static_cast<int>(structured_log_level::k_notice) == static_cast<int>(MONGOC_STRUCTURED_LOG_LEVEL_NOTICE),
    "");
static_assert(static_cast<int>(structured_log_level::k_info) == static_cast<int>(MONGOC_STRUCTURED_LOG_LEVEL_INFO), "");
static_assert(
    static_cast<int>(structured_log_level::k_debug) == static_cast<int>(MONGOC_STRUCTURED_LOG_LEVEL_DEBUG),
    "");
static_assert(
    static_cast<int>(structured_log_level::k_trace) == static_cast<int>(MONGOC_STRUCTURED_LOG_LEVEL_TRACE),
    "");

static_assert(
    static_cast<int>(structured_log_component::k_command) == static_cast<int>(MONGOC_STRUCTURED_LOG_COMPONENT_COMMAND),
    "");
static_assert(
    static_cast<int>(structured_log_component::k_topology) ==
        static_cast<int>(MONGOC_STRUCTURED_LOG_COMPONENT_TOPOLOGY),
    "");
static_assert(
    static_cast<int>(structured_log_component::k_server_selection) ==
        static_cast<int>(MONGOC_STRUCTURED_LOG_COMPONENT_SERVER_SELECTION),
    "");
static_assert(
    static_cast<int>(structured_log_component::k_connection) ==
        static_cast<int>(MONGOC_STRUCTURED_LOG_COMPONENT_CONNECTION),
    "");

bsoncxx::v1::stdx::string_view to_string(structured_log_level level) {
    // mongoc's `structured_log_get_level_name` returns display names (e.g. "Informational") which
    // do not match the standardized logging specification. Emit the spec severity names directly.
    switch (level) {
        case structured_log_level::k_emergency:
            return "emergency";
        case structured_log_level::k_alert:
            return "alert";
        case structured_log_level::k_critical:
            return "critical";
        case structured_log_level::k_error:
            return "error";
        case structured_log_level::k_warning:
            return "warning";
        case structured_log_level::k_notice:
            return "notice";
        case structured_log_level::k_info:
            return "info";
        case structured_log_level::k_debug:
            return "debug";
        case structured_log_level::k_trace:
            return "trace";
        default:
            return "unknown";
    }
}

bsoncxx::v1::stdx::string_view to_string(structured_log_component component) {
    if (auto const name =
            libmongoc::structured_log_get_component_name(static_cast<mongoc_structured_log_component_t>(component))) {
        return name;
    }

    return "unknown";
}

bsoncxx::v1::stdx::optional<structured_log_level> structured_log_level_from_string(
    bsoncxx::v1::stdx::string_view name) {
    mongoc_structured_log_level_t level = {};

    // mongoc requires a null-terminated string.
    if (libmongoc::structured_log_get_named_level(std::string{name}.c_str(), &level)) {
        return static_cast<structured_log_level>(level);
    }

    return bsoncxx::v1::stdx::nullopt;
}

bsoncxx::v1::stdx::optional<structured_log_component> structured_log_component_from_string(
    bsoncxx::v1::stdx::string_view name) {
    mongoc_structured_log_component_t component = {};

    // mongoc requires a null-terminated string.
    if (libmongoc::structured_log_get_named_component(std::string{name}.c_str(), &component)) {
        return static_cast<structured_log_component>(component);
    }

    return bsoncxx::v1::stdx::nullopt;
}

namespace {

mongoc_structured_log_entry_t const* to_mongoc(void const* ptr) {
    return static_cast<mongoc_structured_log_entry_t const*>(ptr);
}

} // namespace

structured_log_level structured_log_entry::level() const {
    return static_cast<structured_log_level>(libmongoc::structured_log_entry_get_level(to_mongoc(_impl)));
}

structured_log_component structured_log_entry::component() const {
    return static_cast<structured_log_component>(libmongoc::structured_log_entry_get_component(to_mongoc(_impl)));
}

bsoncxx::v1::stdx::string_view structured_log_entry::message() const {
    return libmongoc::structured_log_entry_get_message_string(to_mongoc(_impl));
}

bsoncxx::v1::document::value structured_log_entry::message_as_bson() const {
    // mongoc returns a newly-allocated bson_t; scoped_bson takes ownership.
    return scoped_bson{libmongoc::structured_log_entry_message_as_bson(to_mongoc(_impl))}.value();
}

structured_log_entry::structured_log_entry(void const* impl) : _impl{impl} {}

structured_log_entry structured_log_entry::internal::make(mongoc_structured_log_entry_t const* ptr) {
    return {ptr};
}

mongoc_structured_log_entry_t const* structured_log_entry::internal::as_mongoc(structured_log_entry const& self) {
    return to_mongoc(self._impl);
}

} // namespace v1
} // namespace mongocxx
