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

#include <mongocxx/options/data_key.hh>

//

#include <mongocxx/v1/data_key_options.hh>

#include <utility>

#include <bsoncxx/document/value.hpp>

namespace mongocxx {
namespace v_noabi {
namespace options {

data_key::data_key(v1::data_key_options key)
    : _master_key{[&]() -> decltype(_master_key) {
          decltype(_master_key) ret;
          if (auto& opt = v1::data_key_options::internal::master_key(key)) {
              ret = bsoncxx::v_noabi::from_v1(std::move(*opt));
          }
          return ret;
      }()},
      _key_alt_names{std::move(v1::data_key_options::internal::key_alt_names(key))},
      _key_material{std::move(v1::data_key_options::internal::key_material(key))} {}

v1::data_key_options::internal::unique_ptr_type data_key::internal::to_mongoc(data_key const& self) {
    return v1::data_key_options::internal::to_mongoc(to_v1(self));
}

} // namespace options
} // namespace v_noabi
} // namespace mongocxx
