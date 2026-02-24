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

#include <mongocxx/read_preference.hh>

//

#include <mongocxx/v1/read_preference.hh>

#include <chrono>

#include <mongocxx/exception/error_code.hpp>
#include <mongocxx/exception/logic_error.hpp>

#include <mongocxx/private/mongoc.hh>

namespace {
namespace static_assertions {
namespace read_mode {

using lv1 = mongocxx::v1::read_preference::read_mode;
using lv_noabi = mongocxx::v_noabi::read_preference::read_mode;

template <lv1 lhs, lv_noabi rhs>
struct check {
    static_assert(
        static_cast<int>(lhs) == static_cast<int>(rhs),
        "read_preference::read_mode: v1 and v_noabi must have the same values");
};

template struct check<lv1::k_primary, lv_noabi::k_primary>;
template struct check<lv1::k_primary_preferred, lv_noabi::k_primary_preferred>;
template struct check<lv1::k_secondary, lv_noabi::k_secondary>;
template struct check<lv1::k_secondary_preferred, lv_noabi::k_secondary_preferred>;
template struct check<lv1::k_nearest, lv_noabi::k_nearest>;

} // namespace read_mode
} // namespace static_assertions
} // namespace

namespace mongocxx {
namespace v_noabi {

read_preference& read_preference::max_staleness(std::chrono::seconds max_staleness) {
    auto const v = max_staleness.count();

    if (v < -1 || v == 0) {
        throw logic_error{v_noabi::error_code::k_invalid_parameter};
    }

    _rp.max_staleness(max_staleness);

    return *this;
}

mongoc_read_prefs_t const* read_preference::internal::as_mongoc(read_preference const& self) {
    return v1::read_preference::internal::as_mongoc(self._rp);
}

} // namespace v_noabi
} // namespace mongocxx
