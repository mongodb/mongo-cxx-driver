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

#include <mongocxx/model/update_one.hpp>

//

#include <mongocxx/v1/bulk_write.hh>

#include <utility>

#include <bsoncxx/array/value.hpp>
#include <bsoncxx/document/value.hpp>

namespace mongocxx {
namespace v_noabi {
namespace model {

update_one::update_one(v1::bulk_write::update_one op)
    : _filter{bsoncxx::v_noabi::from_v1(std::move(v1::bulk_write::update_one::internal::filter(op)))},
      _update{bsoncxx::v_noabi::from_v1(std::move(v1::bulk_write::update_one::internal::update(op)))},
      _collation{[&]() -> decltype(_collation) {
          if (auto& opt = v1::bulk_write::update_one::internal::collation(op)) {
              return bsoncxx::v_noabi::from_v1(std::move(*opt));
          }
          return {};
      }()},
      _array_filters{[&]() -> decltype(_array_filters) {
          if (auto& opt = v1::bulk_write::update_one::internal::array_filters(op)) {
              return bsoncxx::v_noabi::from_v1(std::move(*opt));
          }
          return {};
      }()},
      _upsert{op.upsert()},
      _hint{std::move(v1::bulk_write::update_one::internal::hint(op))},
      _sort{[&]() -> decltype(_sort) {
          if (auto& opt = v1::bulk_write::update_one::internal::sort(op)) {
              return bsoncxx::v_noabi::from_v1(std::move(*opt));
          }
          return {};
      }()} {}

} // namespace model
} // namespace v_noabi
} // namespace mongocxx
