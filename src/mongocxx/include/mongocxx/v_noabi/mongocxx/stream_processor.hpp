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

#include <mongocxx/stream_processor-fwd.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/stream_processor.hpp> // IWYU pragma: export

#include <bsoncxx/document/value.hpp>
#include <bsoncxx/stdx/string_view.hpp>

#include <mongocxx/options/get_stream_processor_samples.hpp>
#include <mongocxx/options/get_stream_processor_stats.hpp>
#include <mongocxx/options/start_stream_processor.hpp>
#include <mongocxx/result/get_stream_processor_samples.hpp>

#include <mongocxx/config/prelude.hpp>

#include <utility>

namespace mongocxx {
namespace v_noabi {

///
/// A handle for a specific named stream processor.
///
class stream_processor {
   public:
    /* explicit(false) */ stream_processor(v1::stream_processor proc) : _proc{std::move(proc)} {}

    explicit operator v1::stream_processor() && {
        return std::move(_proc);
    }

    bsoncxx::v1::stdx::string_view name() const {
        return _proc.name();
    }

    void start(v_noabi::options::start_stream_processor const& options = {}) {
        _proc.start(v1::start_stream_processor_options{options});
    }

    void stop() {
        _proc.stop();
    }

    void drop() {
        _proc.drop();
    }

    bsoncxx::v1::document::value stats(v_noabi::options::get_stream_processor_stats const& options = {}) {
        return _proc.stats(v1::get_stream_processor_stats_options{options});
    }

    v_noabi::result::get_stream_processor_samples get_stream_processor_samples(
        v_noabi::options::get_stream_processor_samples const& options = {}) {
        return from_v1(_proc.get_stream_processor_samples(v1::get_stream_processor_samples_options{options}));
    }

   private:
    v1::stream_processor _proc;
};

inline v_noabi::stream_processor from_v1(v1::stream_processor v) {
    return {std::move(v)};
}

} // namespace v_noabi
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::stream_processor.
///
/// @par Includes
/// - @ref mongocxx/v1/stream_processor.hpp
///
