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

#include <mongocxx/read_concern.hh>

//

#include <mongocxx/v1/read_concern.hh>

#include <mongocxx/exception/error_code.hpp>
#include <mongocxx/exception/exception.hpp>

#include <mongocxx/private/mongoc.hh>

namespace {
namespace static_assertions {
namespace level {

using lv1 = mongocxx::v1::read_concern::level;
using lv_noabi = mongocxx::v_noabi::read_concern::level;

template <lv1 lhs, lv_noabi rhs>
struct check {
    static_assert(
        static_cast<int>(lhs) == static_cast<int>(rhs),
        "read_concern::level: v1 and v_noabi must have the same values");
};

template struct check<lv1::k_local, lv_noabi::k_local>;
template struct check<lv1::k_majority, lv_noabi::k_majority>;
template struct check<lv1::k_linearizable, lv_noabi::k_linearizable>;
template struct check<lv1::k_server_default, lv_noabi::k_server_default>;
template struct check<lv1::k_unknown, lv_noabi::k_unknown>;
template struct check<lv1::k_available, lv_noabi::k_available>;
template struct check<lv1::k_snapshot, lv_noabi::k_snapshot>;

} // namespace level
} // namespace static_assertions
} // namespace

namespace mongocxx {
namespace v_noabi {

void read_concern::acknowledge_level(read_concern::level rc_level) {
    switch (rc_level) {
        case read_concern::level::k_local:
        case read_concern::level::k_majority:
        case read_concern::level::k_linearizable:
        case read_concern::level::k_server_default:
        case read_concern::level::k_available:
        case read_concern::level::k_snapshot:
            _rc.acknowledge_level(static_cast<v1::read_concern::level>(rc_level));
            break;

        default:
        case read_concern::level::k_unknown:
            // Backward compatibility: k_unknown is an exception.
            throw v_noabi::exception{v_noabi::error_code::k_unknown_read_concern};
    }
}

mongoc_read_concern_t const* read_concern::internal::as_mongoc(read_concern const& self) {
    return v1::read_concern::internal::as_mongoc(self._rc);
}

} // namespace v_noabi
} // namespace mongocxx
