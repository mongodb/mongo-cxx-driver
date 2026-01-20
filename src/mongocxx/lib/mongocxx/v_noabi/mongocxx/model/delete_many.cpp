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

#include <mongocxx/model/delete_many.hpp>

//

#include <mongocxx/v1/bulk_write.hh>

#include <utility>

#include <bsoncxx/document/value.hpp>

namespace mongocxx {
namespace v_noabi {
namespace model {

delete_many::delete_many(v1::bulk_write::delete_many op)
    : _filter{bsoncxx::v_noabi::from_v1(std::move(v1::bulk_write::delete_many::internal::filter(op)))},
      _collation{[&]() -> decltype(_collation) {
          if (auto& opt = v1::bulk_write::delete_many::internal::collation(op)) {
              return bsoncxx::v_noabi::from_v1(std::move(*opt));
          }
          return {};
      }()},
      _hint{std::move(v1::bulk_write::delete_many::internal::hint(op))} {}

} // namespace model
} // namespace v_noabi
} // namespace mongocxx
