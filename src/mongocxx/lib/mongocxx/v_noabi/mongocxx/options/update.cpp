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
#include <bsoncxx/document/view_or_value.hpp>

#include <mongocxx/options/update.hpp>

namespace mongocxx {
namespace v_noabi {
namespace options {

update& update::bypass_document_validation(bool bypass_document_validation) {
    _bypass_document_validation = bypass_document_validation;
    return *this;
}

update& update::collation(bsoncxx::v_noabi::document::view_or_value collation) {
    _collation = std::move(collation);
    return *this;
}

update& update::hint(mongocxx::v_noabi::hint index_hint) {
    _hint = std::move(index_hint);
    return *this;
}

update& update::let(bsoncxx::v_noabi::document::view_or_value let) {
    _let = let;
    return *this;
}

update& update::sort(bsoncxx::v_noabi::document::view_or_value sort) {
    _sort = std::move(sort);
    return *this;
}

update& update::comment(bsoncxx::v_noabi::types::bson_value::view_or_value comment) {
    _comment = std::move(comment);
    return *this;
}

update& update::upsert(bool upsert) {
    _upsert = upsert;
    return *this;
}

update& update::write_concern(mongocxx::v_noabi::write_concern wc) {
    _write_concern = std::move(wc);
    return *this;
}

bsoncxx::v_noabi::stdx::optional<bool> const& update::bypass_document_validation() const {
    return _bypass_document_validation;
}

bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view_or_value> const& update::collation() const {
    return _collation;
}

bsoncxx::v_noabi::stdx::optional<mongocxx::v_noabi::hint> const& update::hint() const {
    return _hint;
}

bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view_or_value> const update::let() const {
    return _let;
}

bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view_or_value> const& update::sort() const {
    return _sort;
}

bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::types::bson_value::view_or_value> const update::comment() const {
    return _comment;
}

bsoncxx::v_noabi::stdx::optional<bool> const& update::upsert() const {
    return _upsert;
}

bsoncxx::v_noabi::stdx::optional<mongocxx::v_noabi::write_concern> const& update::write_concern() const {
    return _write_concern;
}

update& update::array_filters(bsoncxx::v_noabi::array::view_or_value array_filters) {
    _array_filters = std::move(array_filters);
    return *this;
}

bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::array::view_or_value> const& update::array_filters() const {
    return _array_filters;
}

} // namespace options
} // namespace v_noabi
} // namespace mongocxx
