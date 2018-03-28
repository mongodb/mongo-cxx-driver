// Copyright 2018-present MongoDB Inc.
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

#include <mongocxx/options/change_stream.hpp>

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/builder/core.hpp>

#include <mongocxx/config/private/prelude.hh>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN
namespace options {

change_stream::change_stream() = default;

change_stream& change_stream::full_document(bsoncxx::string::view_or_value full_doc) {
    _full_document = std::move(full_doc);
    return *this;
}

const bsoncxx::stdx::optional<bsoncxx::string::view_or_value>& change_stream::full_document()
    const {
    return _full_document;
}

change_stream& change_stream::batch_size(std::int32_t batch_size) {
    _batch_size = batch_size;
    return *this;
}

const stdx::optional<std::int32_t>& change_stream::batch_size() const {
    return _batch_size;
}

change_stream& change_stream::resume_after(bsoncxx::document::view_or_value resume_after) {
    _resume_after = std::move(resume_after);
    return *this;
}

const stdx::optional<bsoncxx::document::view_or_value>& change_stream::resume_after() const {
    return _resume_after;
}

change_stream& change_stream::collation(bsoncxx::document::view_or_value collation) {
    _collation = std::move(collation);
    return *this;
}

const stdx::optional<bsoncxx::document::view_or_value>& change_stream::collation() const {
    return _collation;
}

change_stream& change_stream::max_await_time(std::chrono::milliseconds max_time) {
    _max_await_time = std::move(max_time);
    return *this;
}

const stdx::optional<std::chrono::milliseconds>& change_stream::max_await_time() const {
    return _max_await_time;
}

}  // namespace options
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
