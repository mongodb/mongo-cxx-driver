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

#include <mongocxx/options/aggregate.hpp>

//

#include <mongocxx/v1/aggregate_options.hh>

#include <utility>

#include <bsoncxx/document/value.hpp>
#include <bsoncxx/types/bson_value/value.hpp>

namespace mongocxx {
namespace v_noabi {
namespace options {

aggregate::aggregate(v1::aggregate_options opts)
    : _allow_disk_use{opts.allow_disk_use()},
      _batch_size{opts.batch_size()},
      _collation{[&]() -> decltype(_collation) {
          if (auto& opt = v1::aggregate_options::internal::collation(opts)) {
              return bsoncxx::v_noabi::from_v1(std::move(*opt));
          }
          return {};
      }()},
      _let{[&]() -> decltype(_let) {
          if (auto& opt = v1::aggregate_options::internal::let(opts)) {
              return bsoncxx::v_noabi::from_v1(std::move(*opt));
          }
          return {};
      }()},
      _max_time{opts.max_time()},
      _read_preference{std::move(v1::aggregate_options::internal::read_preference(opts))},
      _bypass_document_validation{opts.bypass_document_validation()},
      _hint{std::move(v1::aggregate_options::internal::hint(opts))},
      _write_concern{std::move(v1::aggregate_options::internal::write_concern(opts))},
      _read_concern{std::move(v1::aggregate_options::internal::read_concern(opts))},
      _comment{std::move(v1::aggregate_options::internal::comment(opts))} {}

} // namespace options
} // namespace v_noabi
} // namespace mongocxx
