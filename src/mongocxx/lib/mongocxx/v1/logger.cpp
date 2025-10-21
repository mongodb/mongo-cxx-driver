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

} // namespace v1
} // namespace mongocxx
