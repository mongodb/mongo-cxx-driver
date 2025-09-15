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

#include <bsoncxx/decimal128.hpp>

//

#include <bsoncxx/v1/decimal128.hpp>
#include <bsoncxx/v1/exception.hpp>

#include <bsoncxx/exception/error_code.hpp>
#include <bsoncxx/exception/exception.hpp>

#include <bsoncxx/private/type_traits.hh>

namespace bsoncxx {
namespace v_noabi {

static_assert(is_explicitly_convertible<decimal128&&, v1::decimal128>::value, "v_noabi -> v1 must be explicit");
static_assert(is_explicitly_convertible<decimal128 const&, v1::decimal128>::value, "v_noabi -> v1 must be explicit");
static_assert(is_implicitly_convertible<v1::decimal128&&, decimal128>::value, "v1 -> v_noabi must be implicit");
static_assert(is_implicitly_convertible<v1::decimal128 const&, decimal128>::value, "v1 -> v_noabi must be implicit");

decimal128::decimal128(v1::stdx::string_view str) try : _d128{str} {
} catch (v1::exception const&) {
    throw v_noabi::exception{v_noabi::error_code::k_invalid_decimal128};
}

} // namespace v_noabi
} // namespace bsoncxx
