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

#include <mongocxx/model/replace_one.hpp>

//

#include <mongocxx/v1/bulk_write.hh>

#include <utility>

#include <bsoncxx/document/value.hpp>

namespace mongocxx {
namespace v_noabi {
namespace model {

replace_one::replace_one(v1::bulk_write::replace_one op)
    : _filter{bsoncxx::v_noabi::from_v1(std::move(v1::bulk_write::replace_one::internal::filter(op)))},
      _replacement{bsoncxx::v_noabi::from_v1(std::move(v1::bulk_write::replace_one::internal::replacement(op)))},
      _collation{[&]() -> decltype(_collation) {
          if (auto& opt = v1::bulk_write::replace_one::internal::collation(op)) {
              return bsoncxx::v_noabi::from_v1(std::move(*opt));
          }
          return {};
      }()},
      _upsert{op.upsert()},
      _hint{std::move(v1::bulk_write::replace_one::internal::hint(op))},
      _sort{[&]() -> decltype(_sort) {
          if (auto& opt = v1::bulk_write::replace_one::internal::sort(op)) {
              return bsoncxx::v_noabi::from_v1(std::move(*opt));
          }
          return {};
      }()} {}

} // namespace model
} // namespace v_noabi
} // namespace mongocxx
