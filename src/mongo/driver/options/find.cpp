// Copyright 2014 MongoDB Inc.
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

#include <mongo/driver/options/find.hpp>
#include <mongo/driver/private/read_preference.hpp>

namespace mongo {
namespace driver {
namespace options {

enum class find::cursor_type : std::uint8_t { k_non_tailable, k_tailable, k_tailable_await };

void find::allow_partial_results(bool allow_partial) {
    _allow_partial_results = allow_partial;
}

void find::batch_size(std::int32_t batch_size) {
    _batch_size = batch_size;
}

void find::comment(std::string comment) {
    _comment = comment;
}

void find::limit(std::int32_t limit) {
    _limit = limit;
}

void find::max_time_ms(std::int64_t max_time_ms) {
    _max_time_ms = max_time_ms;
}

void find::modifiers(bson::document::view modifiers) {
    _modifiers = modifiers;
}

void find::no_cursor_timeout(bool no_cursor_timeout) {
    _no_cursor_timeout = no_cursor_timeout;
}

void find::oplog_replay(bool oplog_replay) {
    _oplog_replay = oplog_replay;
}

void find::projection(bson::document::view projection) {
    _projection = projection;
}

void find::read_preference(class read_preference rp) {
    _read_preference = std::move(rp);
}

void find::skip(std::int32_t skip) {
    _skip = skip;
}

void find::sort(bson::document::view ordering) {
    _ordering = ordering;
}

const stdx::optional<bool>& find::allow_partial_results() const {
    return _allow_partial_results;
}

const stdx::optional<std::int32_t>& find::batch_size() const {
    return _batch_size;
}

const stdx::optional<std::int32_t>& find::limit() const {
    return _limit;
}

const stdx::optional<std::int64_t>& find::max_time_ms() const {
    return _max_time_ms;
}

const stdx::optional<bson::document::view>& find::modifiers() const {
    return _modifiers;
}

const stdx::optional<bool>& find::no_cursor_timeout() const {
    return _no_cursor_timeout;
}

const stdx::optional<bool>& find::oplog_replay() const {
    return _oplog_replay;
}

const stdx::optional<bson::document::view>& find::projection() const {
    return _projection;
}

const stdx::optional<std::int32_t>& find::skip() const {
    return _skip;
}

const stdx::optional<bson::document::view>& find::sort() const {
    return _ordering;
}

const stdx::optional<class read_preference>& find::read_preference() const {
    return _read_preference;
}

}  // namespace options
}  // namespace driver
}  // namespace mongo

#include <mongo/driver/config/postlude.hpp>
