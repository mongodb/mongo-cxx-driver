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

#include <mongocxx/options/get_stream_processor_stats-fwd.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/get_stream_processor_stats_options.hpp> // IWYU pragma: export

#include <bsoncxx/stdx/optional.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
namespace v_noabi {
namespace options {

///
/// Options for retrieving stream processor statistics.
///
class get_stream_processor_stats {
   public:
    get_stream_processor_stats() = default;

    /* explicit(false) */ get_stream_processor_stats(v1::get_stream_processor_stats_options opts)
        : _opts{std::move(opts)} {}

    explicit operator v1::get_stream_processor_stats_options() const {
        return _opts;
    }

    get_stream_processor_stats& verbose(bool v) {
        _opts.verbose(v);
        return *this;
    }

    bsoncxx::v1::stdx::optional<bool> verbose() const {
        return _opts.verbose();
    }

   private:
    v1::get_stream_processor_stats_options _opts;
};

} // namespace options
} // namespace v_noabi
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::options::get_stream_processor_stats.
///
/// @par Includes
/// - @ref mongocxx/v1/get_stream_processor_stats_options.hpp
///
