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

#include <mongocxx/model/delete_one.hpp>

#include <mongocxx/config/private/prelude.hh>

namespace mongocxx {
inline namespace wip {
namespace model {

delete_one::delete_one(bsoncxx::document::view_or_value filter) : _filter(std::move(filter)) {}

const bsoncxx::document::view_or_value& delete_one::filter() const {
    return _filter;
}

delete_one& delete_one::collation(bsoncxx::document::view_or_value collation) {
    _collation = collation;
    return *this;
}

const stdx::optional<bsoncxx::document::view_or_value>& delete_one::collation() const {
    return _collation;
}

delete_one& delete_one::hint(mongocxx::hint index_hint) {
    _hint = std::move(index_hint);
    return *this;
}

const stdx::optional<mongocxx::hint>& delete_one::hint() const {
    return _hint;
}

}  // namespace model
}  // namespace wip
}  // namespace mongocxx
