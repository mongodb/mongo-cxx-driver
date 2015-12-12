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

#include <mongocxx/options/find_one_and_replace.hpp>

#include <mongocxx/config/prelude.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN
namespace options {

void find_one_and_replace::bypass_document_validation(bool bypass_document_validation) {
    _bypass_document_validation = bypass_document_validation;
}

void find_one_and_replace::max_time(std::chrono::milliseconds max_time) {
    _max_time = std::move(max_time);
}

void find_one_and_replace::projection(bsoncxx::document::view projection) {
    _projection = projection;
}

void find_one_and_replace::return_document(enum class return_document return_document) {
    _return_document = return_document;
}

void find_one_and_replace::sort(bsoncxx::document::view ordering) {
    _ordering = ordering;
}

void find_one_and_replace::upsert(bool upsert) {
    _upsert = upsert;
}

const stdx::optional<bool>& find_one_and_replace::bypass_document_validation() const {
    return _bypass_document_validation;
}

const stdx::optional<std::chrono::milliseconds>& find_one_and_replace::max_time() const {
    return _max_time;
}

const stdx::optional<bsoncxx::document::view>& find_one_and_replace::projection() const {
    return _projection;
}

const stdx::optional<enum class return_document>& find_one_and_replace::return_document() const {
    return _return_document;
}

const stdx::optional<bsoncxx::document::view>& find_one_and_replace::sort() const {
    return _ordering;
}

const stdx::optional<bool>& find_one_and_replace::upsert() const {
    return _upsert;
}

}  // namespace options
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
