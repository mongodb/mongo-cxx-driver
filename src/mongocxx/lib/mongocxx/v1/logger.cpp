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

#include <mongocxx/v1/logger.hpp>

//

#include <bsoncxx/v1/stdx/string_view.hpp>

#include <mongocxx/v1/logger.hh>

#include <memory>
#include <utility>

#include <bsoncxx/private/make_unique.hh>

#include <mongocxx/private/mongoc.hh>

namespace mongocxx {
namespace v1 {

static_assert(static_cast<int>(log_level::k_error) == static_cast<int>(MONGOC_LOG_LEVEL_ERROR), "");
static_assert(static_cast<int>(log_level::k_critical) == static_cast<int>(MONGOC_LOG_LEVEL_CRITICAL), "");
static_assert(static_cast<int>(log_level::k_warning) == static_cast<int>(MONGOC_LOG_LEVEL_WARNING), "");
static_assert(static_cast<int>(log_level::k_message) == static_cast<int>(MONGOC_LOG_LEVEL_MESSAGE), "");
static_assert(static_cast<int>(log_level::k_info) == static_cast<int>(MONGOC_LOG_LEVEL_INFO), "");
static_assert(static_cast<int>(log_level::k_debug) == static_cast<int>(MONGOC_LOG_LEVEL_DEBUG), "");
static_assert(static_cast<int>(log_level::k_trace) == static_cast<int>(MONGOC_LOG_LEVEL_TRACE), "");

bsoncxx::v1::stdx::string_view to_string(log_level level) {
    switch (level) {
        case log_level::k_error:
            return "error";
        case log_level::k_critical:
            return "critical";
        case log_level::k_warning:
            return "warning";
        case log_level::k_message:
            return "message";
        case log_level::k_info:
            return "info";
        case log_level::k_debug:
            return "debug";
        case log_level::k_trace:
            return "trace";
        default:
            return "unknown";
    }
}

logger::~logger() = default;

// The C callback registered with mongoc; recovers the `log_handler` from `user_data`.
// Declared in <mongocxx/v1/logger.hh> so `exchange_global_logger` can reference it.
void custom_log_handler(
    mongoc_log_level_t log_level,
    char const* domain,
    char const* message,
    void* user_data) noexcept {
    (*static_cast<log_handler*>(user_data))(
        static_cast<v1::log_level>(log_level),
        bsoncxx::v1::stdx::string_view(domain),
        bsoncxx::v1::stdx::string_view(message));
}

namespace {

// Build a `logging_config` for a custom-handler request: a stored copy of `handler` when non-empty,
// or the disabled state when empty (null).
logging_config make_custom_config(log_handler handler) {
    if (handler) {
        return logging_config{log_mode::k_custom, bsoncxx::make_unique<log_handler>(std::move(handler))};
    }

    return logging_config{log_mode::k_disabled, nullptr};
}

} // namespace

logging_config exchange_global_logger(logging_config next) {
    // The process-global logging configuration.
    //
    // This is not thread-safe: concurrent exchanges (or an exchange concurrent with unstructured
    // logging) are the caller's responsibility to avoid. See `v1::set_global_logger`.
    static logging_config config;

    switch (next.mode) {
        case log_mode::k_custom:
            libmongoc::log_set_handler(&custom_log_handler, next.handler.get());
            break;
        case log_mode::k_default:
            libmongoc::log_set_handler(mongoc_log_default_handler, nullptr);
            break;
        case log_mode::k_disabled:
            libmongoc::log_set_handler(nullptr, nullptr);
            break;
    }

    using std::swap;
    swap(config, next);

    // `next` now holds the previous configuration; returning it transfers ownership of any
    // previously-installed custom handler to the caller.
    return next;
}

void set_global_logger(log_handler handler) {
    exchange_global_logger(make_custom_config(std::move(handler)));
}

void set_global_logger(v1::default_logger) {
    exchange_global_logger(logging_config{log_mode::k_default, nullptr});
}

class logger_guard::impl {
   public:
    logging_config previous;

    explicit impl(logging_config previous) : previous{std::move(previous)} {}
};

logger_guard::~logger_guard() {
    // Restore the captured configuration; the configuration this guard installed is returned and
    // destroyed here (freeing its custom handler, if any).
    exchange_global_logger(std::move(_impl->previous));
}

logger_guard::logger_guard(log_handler handler)
    : _impl{bsoncxx::make_unique<impl>(exchange_global_logger(make_custom_config(std::move(handler))))} {}

logger_guard::logger_guard(v1::default_logger tag) {
    (void)tag;

    _impl = bsoncxx::make_unique<impl>(exchange_global_logger(logging_config{log_mode::k_default, nullptr}));
}

} // namespace v1
} // namespace mongocxx
