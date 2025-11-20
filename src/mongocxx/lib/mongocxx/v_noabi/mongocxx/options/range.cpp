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

#include <mongocxx/options/range.hpp>

//

#include <mongocxx/v1/range_options.hh>

#include <utility>

namespace mongocxx {
namespace v_noabi {
namespace options {

range::range(v1::range_options opts)
    : _min{std::move(v1::range_options::internal::min(opts))},
      _max{std::move(v1::range_options::internal::max(opts))},
      _sparsity{opts.sparsity()},
      _trim_factor{opts.trim_factor()},
      _precision{opts.precision()} {}

} // namespace options
} // namespace v_noabi
} // namespace mongocxx
