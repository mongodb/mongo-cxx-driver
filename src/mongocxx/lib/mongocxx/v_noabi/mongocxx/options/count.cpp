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

#include <mongocxx/options/count.hpp>

//

#include <mongocxx/v1/count_options.hh>

#include <utility>

#include <bsoncxx/document/value.hpp>

namespace mongocxx {
namespace v_noabi {
namespace options {

count::count(v1::count_options opts)
    : _collation{[&]() -> decltype(_collation) {
          if (auto& opt = v1::count_options::internal::collation(opts)) {
              return bsoncxx::v_noabi::from_v1(std::move(*opt));
          }
          return {};
      }()},
      _hint{std::move(v1::count_options::internal::hint(opts))},
      _comment{std::move(v1::count_options::internal::comment(opts))},
      _limit{opts.limit()},
      _max_time{opts.max_time()},
      _skip{opts.skip()},
      _read_preference{std::move(v1::count_options::internal::read_preference(opts))} {}

} // namespace options
} // namespace v_noabi
} // namespace mongocxx
