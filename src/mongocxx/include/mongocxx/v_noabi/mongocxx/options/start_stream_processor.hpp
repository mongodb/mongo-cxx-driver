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

#include <mongocxx/options/start_stream_processor-fwd.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/start_stream_processor_options.hpp> // IWYU pragma: export

#include <bsoncxx/stdx/optional.hpp>
#include <bsoncxx/string/view_or_value.hpp>
#include <bsoncxx/v1/types/view.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
namespace v_noabi {
namespace options {

///
/// Options for starting a stream processor.
///
class start_stream_processor {
   public:
    start_stream_processor() = default;

    /* explicit(false) */ start_stream_processor(v1::start_stream_processor_options opts)
        : _opts{std::move(opts)} {}

    explicit operator v1::start_stream_processor_options() const {
        return _opts;
    }

    start_stream_processor& workers(std::int32_t v) {
        _opts.workers(v);
        return *this;
    }
    bsoncxx::v1::stdx::optional<std::int32_t> workers() const {
        return _opts.workers();
    }

    start_stream_processor& clear_checkpoints(bool v) {
        _opts.clear_checkpoints(v);
        return *this;
    }
    bsoncxx::v1::stdx::optional<bool> clear_checkpoints() const {
        return _opts.clear_checkpoints();
    }

    start_stream_processor& start_at_operation_time(bsoncxx::v1::types::b_timestamp v) {
        _opts.start_at_operation_time(v);
        return *this;
    }
    bsoncxx::v1::stdx::optional<bsoncxx::v1::types::b_timestamp> start_at_operation_time() const {
        return _opts.start_at_operation_time();
    }

    start_stream_processor& tier(bsoncxx::v_noabi::string::view_or_value v) {
        _opts.tier(std::string{v.view()});
        return *this;
    }
    bsoncxx::v1::stdx::optional<bsoncxx::v1::stdx::string_view> tier() const {
        return _opts.tier();
    }

    start_stream_processor& enable_auto_scaling(bool v) {
        _opts.enable_auto_scaling(v);
        return *this;
    }
    bsoncxx::v1::stdx::optional<bool> enable_auto_scaling() const {
        return _opts.enable_auto_scaling();
    }

    start_stream_processor& failover(v1::failover_options v) {
        _opts.failover(std::move(v));
        return *this;
    }
    bsoncxx::v1::stdx::optional<v1::failover_options> failover() const {
        return _opts.failover();
    }

   private:
    v1::start_stream_processor_options _opts;
};

} // namespace options
} // namespace v_noabi
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::options::start_stream_processor.
///
/// @par Includes
/// - @ref mongocxx/v1/start_stream_processor_options.hpp
///
