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

#include <mongocxx/v1/logger.hpp> // IWYU pragma: export

//

#include <memory>
#include <utility>

namespace mongocxx {
namespace v1 {

// The mongocxx-tracked state of the process-global unstructured log handler.
//
// mongoc exposes no getter for the current handler, so mongocxx tracks the "mode" and any owned
// custom handler itself. This is authoritative only so long as the handler is manipulated
// exclusively through the mongocxx API (`set_global_logger`, `logger_guard`, and `instance`).
enum class log_mode {
    k_default,  // mongoc's default handler (`mongoc_log_default_handler`).
    k_disabled, // Unstructured logging disabled (null handler).
    k_custom,   // A custom handler function.
};

// A snapshot of the global logging configuration.
//
// `handler` is non-null if and only if `mode == log_mode::k_custom`. It is heap-allocated so that
// its address (registered with mongoc as `user_data`) remains stable as the owning `logging_config`
// is moved. The handler's lifetime is bound to this object: destroying a `logging_config` frees the
// handler it owns.
//
// Explicit constructors (rather than aggregate initialization with a default member initializer)
// are used to keep this a movable, C++11-compatible type.
struct logging_config {
    log_mode mode;
    std::unique_ptr<log_handler> handler;

    logging_config() noexcept : mode{log_mode::k_default} {}

    logging_config(log_mode mode, std::unique_ptr<log_handler> handler) noexcept
        : mode{mode}, handler{std::move(handler)} {}
};

// Install `next` as the process-global logging configuration and return the configuration it
// replaced.
//
// Registers the mongoc handler that corresponds to `next.mode` (`custom_log_handler` for a custom
// handler, `mongoc_log_default_handler` for the default, or a null handler when disabled), then
// swaps `next` into the global slot.
//
// The returned (previous) configuration owns any custom handler that was previously installed; the
// caller is responsible for its destruction. This is the shared primitive backing the public
// `set_global_logger` overloads and `logger_guard`, as well as `instance` construction/teardown.
//
// @warning Not thread-safe; see `v1::set_global_logger`.
logging_config exchange_global_logger(logging_config next);

} // namespace v1
} // namespace mongocxx
