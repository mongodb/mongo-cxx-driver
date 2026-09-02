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

#include <bsoncxx/v1/types/value.hpp>

#include <mongocxx/v1/encrypt_options.hh>

#include <utility>

#include <bsoncxx/private/suppress_deprecation_warnings.hh>

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
      _string_opts{std::move(v1::encrypt_options::internal::string_opts(opts))},
      _text_opts{std::move(v1::encrypt_options::internal::text_opts(opts))} {}

encrypt::operator v1::encrypt_options() const {
    using bsoncxx::v_noabi::to_v1;
    using mongocxx::v_noabi::to_v1;

    v1::encrypt_options ret;

    if (_key_id) {
        ret.key_id(bsoncxx::v1::types::value{to_v1(*_key_id)});
    }

    if (_key_alt_name) {
        ret.key_alt_name(*_key_alt_name);
    }

    if (_algorithm) {
        ret.algorithm(*_algorithm);
    }

    if (_contention_factor) {
        ret.contention_factor(*_contention_factor);
    }

    if (_query_type) {
        ret.query_type(*_query_type);
    }

    if (_range_opts) {
        ret.range_opts(to_v1(*_range_opts));
    }

    if (_string_opts) {
        ret.string_opts(*_string_opts);
    }

    // Setting the deprecated "textOpts" field necessarily refers to deprecated API.
    BSONCXX_SUPPRESS_DEPRECATION_WARNINGS_BEGIN
    if (_text_opts) {
        ret.text_opts(*_text_opts);
    }
    BSONCXX_SUPPRESS_DEPRECATION_WARNINGS_END

    return ret;
}

} // namespace options
} // namespace v_noabi
} // namespace mongocxx
