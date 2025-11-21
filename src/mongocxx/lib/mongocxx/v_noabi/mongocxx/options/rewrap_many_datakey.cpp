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

#include <mongocxx/options/rewrap_many_datakey.hpp>

//

#include <mongocxx/v1/rewrap_many_datakey_options.hh>

#include <utility>

#include <bsoncxx/document/value.hpp>

namespace mongocxx {
namespace v_noabi {
namespace options {

rewrap_many_datakey::rewrap_many_datakey(v1::rewrap_many_datakey_options opts)
    : _provider{std::move(v1::rewrap_many_datakey_options::internal::provider(opts))},
      _master_key{[&]() -> decltype(_master_key) {
          if (auto& opt = v1::rewrap_many_datakey_options::internal::master_key(opts)) {
              return bsoncxx::v_noabi::from_v1(std::move(*opt));
          }
          return {};
      }()} {}

} // namespace options
} // namespace v_noabi
} // namespace mongocxx
