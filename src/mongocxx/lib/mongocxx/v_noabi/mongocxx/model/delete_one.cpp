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

#include <mongocxx/model/delete_one.hpp>

namespace mongocxx {
namespace v_noabi {
namespace model {

delete_one::delete_one(bsoncxx::v_noabi::document::view_or_value filter) : _filter(std::move(filter)) {}

bsoncxx::v_noabi::document::view_or_value const& delete_one::filter() const {
    return _filter;
}

delete_one& delete_one::collation(bsoncxx::v_noabi::document::view_or_value collation) {
    _collation = collation;
    return *this;
}

bsoncxx::v_noabi::stdx::optional<bsoncxx::v_noabi::document::view_or_value> const& delete_one::collation() const {
    return _collation;
}

delete_one& delete_one::hint(mongocxx::v_noabi::hint index_hint) {
    _hint = std::move(index_hint);
    return *this;
}

bsoncxx::v_noabi::stdx::optional<mongocxx::v_noabi::hint> const& delete_one::hint() const {
    return _hint;
}

} // namespace model
} // namespace v_noabi
} // namespace mongocxx
