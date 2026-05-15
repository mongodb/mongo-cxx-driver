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

#include <mongocxx/options/get_stream_processor_samples-fwd.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/get_stream_processor_samples_options.hpp> // IWYU pragma: export

#include <bsoncxx/stdx/optional.hpp>

#include <mongocxx/config/prelude.hpp>

#include <cstdint>

namespace mongocxx {
namespace v_noabi {
namespace options {

///
/// Options for get_stream_processor_samples().
///
class get_stream_processor_samples {
   public:
    get_stream_processor_samples() = default;

    /* explicit(false) */ get_stream_processor_samples(v1::get_stream_processor_samples_options opts)
        : _opts{std::move(opts)} {}

    explicit operator v1::get_stream_processor_samples_options() const {
        return _opts;
    }

    get_stream_processor_samples& cursor_id(std::int64_t v) {
        _opts.cursor_id(v);
        return *this;
    }
    bsoncxx::v1::stdx::optional<std::int64_t> cursor_id() const {
        return _opts.cursor_id();
    }

    get_stream_processor_samples& limit(std::int32_t v) {
        _opts.limit(v);
        return *this;
    }
    bsoncxx::v1::stdx::optional<std::int32_t> limit() const {
        return _opts.limit();
    }

    get_stream_processor_samples& batch_size(std::int32_t v) {
        _opts.batch_size(v);
        return *this;
    }
    bsoncxx::v1::stdx::optional<std::int32_t> batch_size() const {
        return _opts.batch_size();
    }

   private:
    v1::get_stream_processor_samples_options _opts;
};

} // namespace options
} // namespace v_noabi
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::options::get_stream_processor_samples.
///
/// @par Includes
/// - @ref mongocxx/v1/get_stream_processor_samples_options.hpp
///
