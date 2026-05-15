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

#include <mongocxx/v1/get_stream_processor_samples_result.hh>

//

#include <bsoncxx/v1/document/value.hpp>

#include <cstdint>
#include <vector>

namespace mongocxx {
namespace v1 {

get_stream_processor_samples_result::get_stream_processor_samples_result(
    std::int64_t cursor_id,
    std::vector<bsoncxx::v1::document::value> documents)
    : _cursor_id{cursor_id}, _documents{std::move(documents)} {}

get_stream_processor_samples_result::get_stream_processor_samples_result(
    get_stream_processor_samples_result&&) noexcept = default;

get_stream_processor_samples_result& get_stream_processor_samples_result::operator=(
    get_stream_processor_samples_result&&) noexcept = default;

get_stream_processor_samples_result::~get_stream_processor_samples_result() = default;

std::int64_t get_stream_processor_samples_result::cursor_id() const noexcept {
    return _cursor_id;
}

std::vector<bsoncxx::v1::document::value> const& get_stream_processor_samples_result::documents() const noexcept {
    return _documents;
}

} // namespace v1
} // namespace mongocxx
