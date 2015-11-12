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

#include <mongocxx/options/find_one_and_update.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN
namespace options {

void find_one_and_update::max_time_ms(std::int64_t max_time_ms) {
    _max_time_ms = max_time_ms;
}

void find_one_and_update::projection(bsoncxx::document::view projection) {
    _projection = projection;
}

void find_one_and_update::return_document(enum return_document return_document) {
    _return_document = return_document;
}

void find_one_and_update::sort(bsoncxx::document::view ordering) {
    _ordering = ordering;
}

void find_one_and_update::upsert(bool upsert) {
    _upsert = upsert;
}

void find_one_and_update::bypass_document_validation(bool bypass_document_validation) {
    _bypass_document_validation = bypass_document_validation;
}

const stdx::optional<std::int64_t>& find_one_and_update::max_time_ms() const {
    return _max_time_ms;
}

const stdx::optional<bsoncxx::document::view>& find_one_and_update::projection() const {
    return _projection;
}

const stdx::optional<return_document>& find_one_and_update::return_document() const {
    return _return_document;
}

const stdx::optional<bsoncxx::document::view>& find_one_and_update::sort() const {
    return _ordering;
}

const stdx::optional<bool>& find_one_and_update::upsert() const {
    return _upsert;
}

const stdx::optional<bool>& find_one_and_update::bypass_document_validation() const {
    return _bypass_document_validation;
}

}  // namespace options
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
