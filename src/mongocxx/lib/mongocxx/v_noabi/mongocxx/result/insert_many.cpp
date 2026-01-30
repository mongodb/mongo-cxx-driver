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

#include <mongocxx/result/insert_many.hpp>

//

#include <mongocxx/v1/insert_many_result.hh>

#include <cstddef>
#include <utility>

#include <bsoncxx/array/element.hpp>
#include <bsoncxx/array/value.hpp>
#include <bsoncxx/types/view.hpp> // IWYU pragma: keep: bsoncxx::v_noabi::array::element::get_value()

#include <mongocxx/result/bulk_write.hpp>

namespace mongocxx {
namespace v_noabi {
namespace result {

insert_many::insert_many(v1::insert_many_result res)
    : _result{std::move(v1::insert_many_result::internal::result(res))},
      _inserted_ids{bsoncxx::v_noabi::from_v1(std::move(v1::insert_many_result::internal::inserted_ids(res)))} {
    this->sync_id_map();
}

insert_many::operator v1::insert_many_result() const {
    return v1::insert_many_result::internal::make(v_noabi::to_v1(_result), bsoncxx::v_noabi::to_v1(_inserted_ids));
}

bool operator==(insert_many const& lhs, insert_many const& rhs) {
    if (lhs.result() != rhs.result()) {
        return false;
    }

    auto const lhs_ids = lhs.inserted_ids();
    auto const rhs_ids = rhs.inserted_ids();

    if (lhs_ids.size() != rhs_ids.size()) {
        return false;
    }

    auto liter = lhs_ids.begin();
    auto riter = rhs_ids.begin();

    for (; liter != lhs_ids.end(); liter++, riter++) {
        if (liter->first != riter->first || liter->second.get_oid() != riter->second.get_oid()) {
            return false;
        }
    }
    return true;
}

} // namespace result
} // namespace v_noabi
} // namespace mongocxx
