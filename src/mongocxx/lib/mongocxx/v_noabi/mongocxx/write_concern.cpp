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

#include <mongocxx/write_concern.hh>

//

#include <mongocxx/v1/write_concern.hh>

#include <chrono>
#include <cstdint>
#include <tuple>

#include <mongocxx/exception/error_code.hpp>
#include <mongocxx/exception/exception.hpp>
#include <mongocxx/exception/logic_error.hpp>

#include <mongocxx/write_concern.hh>

#include <mongocxx/private/mongoc.hh>

namespace {
namespace static_assertions {
namespace level {

using lv1 = mongocxx::v1::write_concern::level;
using lv_noabi = mongocxx::v_noabi::write_concern::level;

template <lv1 lhs, lv_noabi rhs>
struct check {
    static_assert(
        static_cast<int>(lhs) == static_cast<int>(rhs),
        "write_concern::level: v1 and v_noabi must have the same values");
};

template struct check<lv1::k_default, lv_noabi::k_default>;
template struct check<lv1::k_majority, lv_noabi::k_majority>;
template struct check<lv1::k_tag, lv_noabi::k_tag>;
template struct check<lv1::k_unacknowledged, lv_noabi::k_unacknowledged>;
template struct check<lv1::k_acknowledged, lv_noabi::k_acknowledged>;

} // namespace level
} // namespace static_assertions
} // namespace

namespace mongocxx {
namespace v_noabi {

namespace {

struct static_assertions {
    using lv1 = v1::write_concern::level;
    using lv_noabi = v_noabi::write_concern::level;

    static_assert(static_cast<int>(lv1::k_default) == static_cast<int>(lv_noabi::k_default), "");
    static_assert(static_cast<int>(lv1::k_majority) == static_cast<int>(lv_noabi::k_majority), "");
    static_assert(static_cast<int>(lv1::k_tag) == static_cast<int>(lv_noabi::k_tag), "");
    static_assert(static_cast<int>(lv1::k_unacknowledged) == static_cast<int>(lv_noabi::k_unacknowledged), "");
    static_assert(static_cast<int>(lv1::k_acknowledged) == static_cast<int>(lv_noabi::k_acknowledged), "");
};

} // namespace

void write_concern::nodes(std::int32_t confirm_from) {
    if (confirm_from < 0) {
        throw mongocxx::v_noabi::logic_error{error_code::k_invalid_parameter};
    }

    _wc.nodes(confirm_from);
}

void write_concern::acknowledge_level(write_concern::level confirm_level) {
    if (confirm_level == level::k_tag) {
        if (this->acknowledge_level() != level::k_tag) {
            throw exception{error_code::k_unknown_write_concern};
        } else {
            return;
        }
    }

    _wc.acknowledge_level(static_cast<v1::write_concern::level>(confirm_level));
}

void write_concern::timeout(std::chrono::milliseconds timeout) {
    auto const count = timeout.count();

    if ((count < 0) || (count >= std::int64_t{INT32_MAX})) {
        throw logic_error{error_code::k_invalid_parameter};
    }

    _wc.timeout(timeout);
}

bool operator==(write_concern const& lhs, write_concern const& rhs) {
    return std::forward_as_tuple(
               lhs.journal(), lhs.nodes(), lhs.acknowledge_level(), lhs.tag(), lhs.majority(), lhs.timeout()) ==
           std::forward_as_tuple(
               rhs.journal(), rhs.nodes(), rhs.acknowledge_level(), rhs.tag(), rhs.majority(), rhs.timeout());
}

mongoc_write_concern_t const* write_concern::internal::as_mongoc(write_concern const& self) {
    return v1::write_concern::internal::as_mongoc(self._wc);
}

} // namespace v_noabi
} // namespace mongocxx
