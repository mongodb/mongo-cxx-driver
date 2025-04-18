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

#include <bsoncxx/array/view_or_value.hpp>

#include <mongocxx/options/find_one_and_update.hpp>

namespace mongocxx {
namespace v_noabi {
namespace options {

find_one_and_update& find_one_and_update::bypass_document_validation(bool bypass_document_validation) {
    _bypass_document_validation = bypass_document_validation;
    return *this;
}

find_one_and_update& find_one_and_update::collation(bsoncxx::v_noabi::document::view_or_value collation) {
    _collation = std::move(collation);
    return *this;
}

find_one_and_update& find_one_and_update::hint(mongocxx::v_noabi::hint index_hint) {
    _hint = std::move(index_hint);
    return *this;
}

find_one_and_update& find_one_and_update::let(bsoncxx::v_noabi::document::view_or_value let) {
    _let = let;
    return *this;
}

find_one_and_update& find_one_and_update::comment(bsoncxx::v_noabi::types::bson_value::view_or_value comment) {
    _comment = std::move(comment);
    return *this;
}

find_one_and_update& find_one_and_update::max_time(std::chrono::milliseconds max_time) {
    _max_time = std::move(max_time);
    return *this;
}

find_one_and_update& find_one_and_update::projection(bsoncxx::v_noabi::document::view_or_value projection) {
    _projection = std::move(projection);
    return *this;
}

find_one_and_update& find_one_and_update::return_document(mongocxx::v_noabi::options::return_document return_document) {
    _return_document = return_document;
    return *this;
}

find_one_and_update& find_one_and_update::sort(bsoncxx::v_noabi::document::view_or_value ordering) {
    _ordering = std::move(ordering);
    return *this;
}

find_one_and_update& find_one_and_update::upsert(bool upsert) {
    _upsert = upsert;
    return *this;
}

find_one_and_update& find_one_and_update::write_concern(mongocxx::v_noabi::write_concern write_concern) {
    _write_concern = std::move(write_concern);
    return *this;
}

bsoncxx::v_noabi::stdx::optional<bool> const& find_one_and_update::bypass_document_validation() const {
    return _bypass_document_validation;
}

bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view_or_value> const& find_one_and_update::collation()
    const {
    return _collation;
}

bsoncxx::v_noabi::stdx::optional<mongocxx::v_noabi::hint> const& find_one_and_update::hint() const {
    return _hint;
}

bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view_or_value> const find_one_and_update::let() const {
    return _let;
}

bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::types::bson_value::view_or_value> const
find_one_and_update::comment() const {
    return _comment;
}

bsoncxx::v_noabi::stdx::optional<std::chrono::milliseconds> const& find_one_and_update::max_time() const {
    return _max_time;
}

bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view_or_value> const& find_one_and_update::projection()
    const {
    return _projection;
}

bsoncxx::v_noabi::stdx::optional<return_document> const& find_one_and_update::return_document() const {
    return _return_document;
}

bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view_or_value> const& find_one_and_update::sort() const {
    return _ordering;
}

bsoncxx::v_noabi::stdx::optional<bool> const& find_one_and_update::upsert() const {
    return _upsert;
}

bsoncxx::v_noabi::stdx::optional<mongocxx::v_noabi::write_concern> const& find_one_and_update::write_concern() const {
    return _write_concern;
}

find_one_and_update& find_one_and_update::array_filters(bsoncxx::v_noabi::array::view_or_value array_filters) {
    _array_filters = std::move(array_filters);
    return *this;
}

bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::array::view_or_value> const& find_one_and_update::array_filters()
    const {
    return _array_filters;
}

} // namespace options
} // namespace v_noabi
} // namespace mongocxx
