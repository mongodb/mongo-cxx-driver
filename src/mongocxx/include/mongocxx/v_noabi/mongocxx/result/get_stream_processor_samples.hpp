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

#include <mongocxx/result/get_stream_processor_samples-fwd.hpp> // IWYU pragma: export

//

#include <mongocxx/v1/get_stream_processor_samples_result.hpp> // IWYU pragma: export

#include <bsoncxx/document/value.hpp>

#include <mongocxx/config/prelude.hpp>

#include <cstdint>
#include <utility>
#include <vector>

namespace mongocxx {
namespace v_noabi {
namespace result {

///
/// Result of get_stream_processor_samples().
///
class get_stream_processor_samples {
   public:
    /* explicit(false) */ get_stream_processor_samples(v1::get_stream_processor_samples_result res)
        : _result{std::move(res)} {}

    explicit operator v1::get_stream_processor_samples_result() && {
        return std::move(_result);
    }

    std::int64_t cursor_id() const noexcept {
        return _result.cursor_id();
    }

    std::vector<bsoncxx::v1::document::value> const& documents() const noexcept {
        return _result.documents();
    }

   private:
    v1::get_stream_processor_samples_result _result;
};

inline v_noabi::result::get_stream_processor_samples from_v1(v1::get_stream_processor_samples_result v) {
    return {std::move(v)};
}

} // namespace result
} // namespace v_noabi
} // namespace mongocxx

#include <mongocxx/config/postlude.hpp>

///
/// @file
/// Provides @ref mongocxx::v_noabi::result::get_stream_processor_samples.
///
/// @par Includes
/// - @ref mongocxx/v1/get_stream_processor_samples_result.hpp
///
