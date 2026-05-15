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

#include <mongocxx/stream_processor_info-fwd.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/stream_processor_info.hpp> // IWYU pragma: export

#include <bsoncxx/array/view.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/stdx/optional.hpp>
#include <bsoncxx/stdx/string_view.hpp>

#include <mongocxx/config/prelude.hpp>

#include <chrono>
#include <cstdint>
#include <utility>

namespace mongocxx {
namespace v_noabi {

///
/// Information about a stream processor returned by StreamProcessors::get_info().
///
class stream_processor_info {
   public:
    /* explicit(false) */ stream_processor_info(v1::stream_processor_info info)
        : _info{std::move(info)} {}

    explicit operator v1::stream_processor_info() && {
        return std::move(_info);
    }

    explicit operator v1::stream_processor_info() const& {
        return _info;
    }

    bsoncxx::v1::stdx::string_view id() const {
        return _info.id();
    }
    bsoncxx::v1::stdx::string_view name() const {
        return _info.name();
    }
    bsoncxx::v1::stdx::string_view state() const {
        return _info.state();
    }
    bsoncxx::v1::array::view pipeline() const {
        return _info.pipeline();
    }
    bsoncxx::v1::stdx::optional<std::int32_t> pipeline_version() const {
        return _info.pipeline_version();
    }
    bsoncxx::v1::stdx::optional<bsoncxx::v1::stdx::string_view> tier() const {
        return _info.tier();
    }
    bsoncxx::v1::stdx::optional<bsoncxx::v1::document::view> dlq() const {
        return _info.dlq();
    }
    bsoncxx::v1::stdx::optional<bsoncxx::v1::stdx::string_view> stream_meta_field_name() const {
        return _info.stream_meta_field_name();
    }
    bool enable_auto_scaling() const {
        return _info.enable_auto_scaling();
    }
    bool failover_enabled() const {
        return _info.failover_enabled();
    }
    bsoncxx::v1::stdx::string_view active_region() const {
        return _info.active_region();
    }
    bsoncxx::v1::stdx::string_view workspace_default_region() const {
        return _info.workspace_default_region();
    }
    bsoncxx::v1::stdx::optional<std::chrono::system_clock::time_point> last_state_change() const {
        return _info.last_state_change();
    }
    bsoncxx::v1::stdx::optional<std::chrono::system_clock::time_point> last_modified_at() const {
        return _info.last_modified_at();
    }
    bsoncxx::v1::stdx::string_view modified_by() const {
        return _info.modified_by();
    }
    bool has_started() const {
        return _info.has_started();
    }
    bsoncxx::v1::stdx::string_view error_msg() const {
        return _info.error_msg();
    }
    bool error_retryable() const {
        return _info.error_retryable();
    }
    bsoncxx::v1::stdx::optional<std::int32_t> error_code() const {
        return _info.error_code();
    }

   private:
    v1::stream_processor_info _info;
};

inline v_noabi::stream_processor_info from_v1(v1::stream_processor_info v) {
    return {std::move(v)};
}

inline v1::stream_processor_info to_v1(v_noabi::stream_processor_info v) {
    return v1::stream_processor_info{std::move(v)};
}

} // namespace v_noabi
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::stream_processor_info.
///
/// @par Includes
/// - @ref mongocxx/v1/stream_processor_info.hpp
///
