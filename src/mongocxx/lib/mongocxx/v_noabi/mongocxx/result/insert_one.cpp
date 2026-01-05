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

#include <mongocxx/result/insert_one.hpp>

//

#include <mongocxx/v1/insert_one_result.hh>

#include <utility>

#include <bsoncxx/types/value.hpp>

#include <mongocxx/result/bulk_write.hpp>

namespace mongocxx {
namespace v_noabi {
namespace result {

insert_one::insert_one(v1::insert_one_result opts)
    : _result{std::move(v1::insert_one_result::internal::result(opts))},
      _inserted_id_owned{std::move(v1::insert_one_result::internal::inserted_id(opts))},
      _inserted_id{_inserted_id_owned} {}

insert_one::operator v1::insert_one_result() const {
    return v1::insert_one_result::internal::make(v_noabi::to_v1(_result), bsoncxx::v_noabi::to_v1(_inserted_id_owned));
}

} // namespace result
} // namespace v_noabi
} // namespace mongocxx
