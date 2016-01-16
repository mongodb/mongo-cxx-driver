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

#include <mongocxx/options/find.hpp>

#include <mongocxx/private/read_preference.hpp>

#include <mongocxx/config/private/prelude.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN
namespace options {

void find::allow_partial_results(bool allow_partial) {
    _allow_partial_results = allow_partial;
}

void find::batch_size(std::int32_t batch_size) {
    _batch_size = batch_size;
}

void find::comment(bsoncxx::string::view_or_value comment) {
    _comment = std::move(comment);
}

void find::cursor_type(cursor::type cursor_type) {
    _cursor_type = cursor_type;
}

void find::hint(class hint index_hint) {
    _hint = std::move(index_hint);
}

void find::limit(std::int32_t limit) {
    _limit = limit;
}

void find::max_await_time(std::chrono::milliseconds max_await_time) {
    _max_await_time = std::move(max_await_time);
}

void find::max_time(std::chrono::milliseconds max_time) {
    _max_time = std::move(max_time);
}

void find::modifiers(bsoncxx::document::view_or_value modifiers) {
    _modifiers = std::move(modifiers);
}

void find::no_cursor_timeout(bool no_cursor_timeout) {
    _no_cursor_timeout = no_cursor_timeout;
}

void find::oplog_replay(bool oplog_replay) {
    _oplog_replay = oplog_replay;
}

void find::projection(bsoncxx::document::view_or_value projection) {
    _projection = std::move(projection);
}

void find::read_preference(class read_preference rp) {
    _read_preference = std::move(rp);
}

void find::skip(std::int32_t skip) {
    _skip = skip;
}

void find::sort(bsoncxx::document::view_or_value ordering) {
    _ordering = std::move(ordering);
}

const stdx::optional<bool>& find::allow_partial_results() const {
    return _allow_partial_results;
}

const stdx::optional<std::int32_t>& find::batch_size() const {
    return _batch_size;
}

const stdx::optional<bsoncxx::string::view_or_value>& find::comment() const {
    return _comment;
}

const stdx::optional<cursor::type>& find::cursor_type() const {
    return _cursor_type;
}

const stdx::optional<class hint>& find::hint() const {
    return _hint;
}

const stdx::optional<std::int32_t>& find::limit() const {
    return _limit;
}

const stdx::optional<std::chrono::milliseconds>& find::max_await_time() const {
    return _max_await_time;
}

const stdx::optional<std::chrono::milliseconds>& find::max_time() const {
    return _max_time;
}

const stdx::optional<bsoncxx::document::view_or_value>& find::modifiers() const {
    return _modifiers;
}

const stdx::optional<bool>& find::no_cursor_timeout() const {
    return _no_cursor_timeout;
}

const stdx::optional<bool>& find::oplog_replay() const {
    return _oplog_replay;
}

const stdx::optional<bsoncxx::document::view_or_value>& find::projection() const {
    return _projection;
}

const stdx::optional<std::int32_t>& find::skip() const {
    return _skip;
}

const stdx::optional<bsoncxx::document::view_or_value>& find::sort() const {
    return _ordering;
}

const stdx::optional<class read_preference>& find::read_preference() const {
    return _read_preference;
}

}  // namespace options
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
