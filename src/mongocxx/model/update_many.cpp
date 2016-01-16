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

#include <mongocxx/model/update_many.hpp>

#include <mongocxx/config/private/prelude.hpp>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN
namespace model {

update_many::update_many(bsoncxx::document::view_or_value filter,
                         bsoncxx::document::view_or_value update)
    : _filter(std::move(filter)), _update(std::move(update)) {
}

update_many& update_many::upsert(bool upsert) {
    _upsert = upsert;
    return *this;
}

const stdx::optional<bool>& update_many::upsert() const {
    return _upsert;
}

const bsoncxx::document::view_or_value& update_many::filter() const {
    return _filter;
}

const bsoncxx::document::view_or_value& update_many::update() const {
    return _update;
}

}  // namespace model
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
