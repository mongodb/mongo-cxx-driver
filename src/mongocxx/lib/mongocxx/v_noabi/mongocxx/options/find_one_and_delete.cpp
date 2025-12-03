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

#include <mongocxx/options/find_one_and_delete.hpp>

//

#include <mongocxx/v1/find_one_and_delete_options.hh>

#include <utility>

#include <bsoncxx/document/value.hpp>

namespace mongocxx {
namespace v_noabi {
namespace options {

find_one_and_delete::find_one_and_delete(v1::find_one_and_delete_options opts)
    : _collation{[&]() -> decltype(_collation) {
          if (auto& opt = v1::find_one_and_delete_options::internal::collation(opts)) {
              return bsoncxx::v_noabi::from_v1(std::move(*opt));
          }
          return {};
      }()},
      _max_time{opts.max_time()},
      _projection{[&]() -> decltype(_projection) {
          if (auto& opt = v1::find_one_and_delete_options::internal::projection(opts)) {
              return bsoncxx::v_noabi::from_v1(std::move(*opt));
          }
          return {};
      }()},
      _ordering{[&]() -> decltype(_projection) {
          if (auto& opt = v1::find_one_and_delete_options::internal::sort(opts)) {
              return bsoncxx::v_noabi::from_v1(std::move(*opt));
          }
          return {};
      }()},
      _write_concern{std::move(v1::find_one_and_delete_options::internal::write_concern(opts))},
      _hint{std::move(v1::find_one_and_delete_options::internal::hint(opts))},
      _let{[&]() -> decltype(_let) {
          if (auto& opt = v1::find_one_and_delete_options::internal::let(opts)) {
              return bsoncxx::v_noabi::from_v1(std::move(*opt));
          }
          return {};
      }()},
      _comment{std::move(v1::find_one_and_delete_options::internal::comment(opts))} {}

} // namespace options
} // namespace v_noabi
} // namespace mongocxx
