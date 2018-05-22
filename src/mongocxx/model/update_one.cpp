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

#include <mongocxx/model/update_one.hpp>

#include <bsoncxx/array/view_or_value.hpp>
#include <mongocxx/config/private/prelude.hh>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN
namespace model {

update_one::update_one(bsoncxx::document::view_or_value filter,
                       bsoncxx::document::view_or_value update)
    : _filter(std::move(filter)), _update(std::move(update)) {}

const bsoncxx::document::view_or_value& update_one::filter() const {
    return _filter;
}

const bsoncxx::document::view_or_value& update_one::update() const {
    return _update;
}

update_one& update_one::collation(bsoncxx::document::view_or_value collation) {
    _collation = collation;
    return *this;
}

const stdx::optional<bsoncxx::document::view_or_value>& update_one::collation() const {
    return _collation;
}

update_one& update_one::upsert(bool upsert) {
    _upsert = upsert;
    return *this;
}

const stdx::optional<bool>& update_one::upsert() const {
    return _upsert;
}

update_one& update_one::array_filters(bsoncxx::array::view_or_value array_filters) {
    _array_filters = std::move(array_filters);
    return *this;
}

const stdx::optional<bsoncxx::array::view_or_value>& update_one::array_filters() const {
    return _array_filters;
}

}  // namespace model
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
