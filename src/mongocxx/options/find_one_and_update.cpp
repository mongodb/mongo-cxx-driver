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

#include <mongocxx/config/private/prelude.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN
namespace options {

find_one_and_update& find_one_and_update::bypass_document_validation(
    bool bypass_document_validation) {
    _bypass_document_validation = bypass_document_validation;
    return *this;
}

find_one_and_update& find_one_and_update::max_time(std::chrono::milliseconds max_time) {
    _max_time = std::move(max_time);
    return *this;
}

find_one_and_update& find_one_and_update::projection(bsoncxx::document::view_or_value projection) {
    _projection = std::move(projection);
    return *this;
}

find_one_and_update& find_one_and_update::return_document(
    mongocxx::options::return_document return_document) {
    _return_document = return_document;
    return *this;
}

find_one_and_update& find_one_and_update::sort(bsoncxx::document::view_or_value ordering) {
    _ordering = std::move(ordering);
    return *this;
}

find_one_and_update& find_one_and_update::upsert(bool upsert) {
    _upsert = upsert;
    return *this;
}

const stdx::optional<bool>& find_one_and_update::bypass_document_validation() const {
    return _bypass_document_validation;
}

const stdx::optional<std::chrono::milliseconds>& find_one_and_update::max_time() const {
    return _max_time;
}

const stdx::optional<bsoncxx::document::view_or_value>& find_one_and_update::projection() const {
    return _projection;
}

const stdx::optional<return_document>& find_one_and_update::return_document() const {
    return _return_document;
}

const stdx::optional<bsoncxx::document::view_or_value>& find_one_and_update::sort() const {
    return _ordering;
}

const stdx::optional<bool>& find_one_and_update::upsert() const {
    return _upsert;
}

}  // namespace options
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
