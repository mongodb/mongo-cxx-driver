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

#include <mongocxx/options/encrypt.hpp>

//

#include <mongocxx/v1/encrypt_options.hh>

#include <utility>

namespace mongocxx {
namespace v_noabi {
namespace options {

encrypt::encrypt(v1::encrypt_options opts)
    : _key_id{std::move(v1::encrypt_options::internal::key_id(opts))},
      _key_alt_name{std::move(v1::encrypt_options::internal::key_alt_name(opts))},
      _algorithm{std::move(v1::encrypt_options::internal::algorithm(opts))},
      _contention_factor{opts.contention_factor()},
      _query_type{opts.query_type()},
      _range_opts{std::move(v1::encrypt_options::internal::range_opts(opts))},
      _text_opts{std::move(v1::encrypt_options::internal::text_opts(opts))} {}

} // namespace options
} // namespace v_noabi
} // namespace mongocxx
