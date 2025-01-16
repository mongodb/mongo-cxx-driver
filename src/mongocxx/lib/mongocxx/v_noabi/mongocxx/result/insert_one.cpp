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

#include <bsoncxx/builder/basic/array.hpp>

#include <mongocxx/result/insert_one.hpp>

namespace mongocxx {
namespace v_noabi {
namespace result {

insert_one::insert_one(result::bulk_write result, bsoncxx::v_noabi::types::bson_value::view inserted_id)
    : _result(std::move(result)),
      _inserted_id_owned(bsoncxx::v_noabi::builder::basic::make_array(inserted_id)),
      _inserted_id(_inserted_id_owned.view()[0].get_value()) {}

result::bulk_write const& insert_one::result() const {
    return _result;
}

bsoncxx::v_noabi::types::bson_value::view const& insert_one::inserted_id() const {
    return _inserted_id;
}

bool operator==(insert_one const& lhs, insert_one const& rhs) {
    return ((lhs.result() == rhs.result()) && (lhs.inserted_id() == rhs.inserted_id()));
}
bool operator!=(insert_one const& lhs, insert_one const& rhs) {
    return !(lhs == rhs);
}

} // namespace result
} // namespace v_noabi
} // namespace mongocxx
