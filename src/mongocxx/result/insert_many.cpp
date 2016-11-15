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

#include <mongocxx/result/insert_many.hpp>

#include <bsoncxx/types/value.hpp>

#include <mongocxx/config/private/prelude.hh>

namespace mongocxx {
MONGOCXX_INLINE_NAMESPACE_BEGIN
namespace result {

insert_many::insert_many(result::bulk_write result, bsoncxx::array::view inserted_ids)
    : _result(std::move(result)), _inserted_ids_owned(inserted_ids) {
    std::size_t index = 0;
    for (auto&& ele : _inserted_ids_owned.view()) {
        _inserted_ids.emplace(index++, ele.get_document().value["_id"]);
    }
}

const result::bulk_write& insert_many::result() const {
    return _result;
}

std::int32_t insert_many::inserted_count() const {
    return _result.inserted_count();
}

insert_many::id_map insert_many::inserted_ids() {
    return _inserted_ids;
}

}  // namespace result
MONGOCXX_INLINE_NAMESPACE_END
}  // namespace mongocxx
