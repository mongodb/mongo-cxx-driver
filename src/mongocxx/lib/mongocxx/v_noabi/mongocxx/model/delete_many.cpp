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

#include <mongocxx/model/delete_many.hpp>

#include <mongocxx/config/private/prelude.hh>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN
namespace model {

delete_many::delete_many(bsoncxx::document::view_or_value filter) : _filter(std::move(filter)) {}

const bsoncxx::document::view_or_value& delete_many::filter() const {
    return _filter;
}

delete_many& delete_many::collation(bsoncxx::document::view_or_value collation) {
    _collation = collation;
    return *this;
}

delete_many& delete_many::hint(class hint index_hint) {
    _hint = std::move(index_hint);
    return *this;
}

const stdx::optional<class hint>& delete_many::hint() const {
    return _hint;
}

const stdx::optional<bsoncxx::document::view_or_value>& delete_many::collation() const {
    return _collation;
}

}  // namespace model
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
