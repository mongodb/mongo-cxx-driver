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

#pragma once

#include <bsoncxx/v1/types/value.hh>

#include <utility>

#include <bsoncxx/types/bson_value/view.hpp>

#include <mongocxx/private/bson.hh>

namespace mongocxx {
namespace detail {

struct scoped_bson_value {
    bsoncxx::v1::types::value value;

    // Allow obtaining a pointer to this->value even in rvalue expressions.
    bson_value_t* get() noexcept {
        auto& v = bsoncxx::v1::types::value::internal::get_bson_value(value);

        // CSFLE API requires empty strings to be not-null.
        if (v.value_type == BSON_TYPE_UTF8 && v.value.v_utf8.str == nullptr) {
            v.value.v_utf8.str = static_cast<char*>(bson_malloc0(1u));
        }

        return &v;
    }

    // Communicate this->value is to be initialized via the resulting pointer.
    bson_value_t* value_for_init() noexcept {
        return &bsoncxx::v1::types::value::internal::get_bson_value(value);
    }

    explicit operator bsoncxx::v1::types::value() && {
        return std::move(value);
    }

    explicit operator bsoncxx::v1::types::value() const& {
        return value;
    }

    scoped_bson_value() = default;

    template <typename T>
    explicit scoped_bson_value(T const& v) : value{convert(v)} {}

    explicit scoped_bson_value(bsoncxx::v1::types::view const& v) : value{v} {}

    explicit scoped_bson_value(bsoncxx::v_noabi::types::bson_value::view const& v)
        : scoped_bson_value{bsoncxx::v_noabi::to_v1(v)} {}

   private:
    template <typename BType>
    auto convert(BType const& v) -> decltype(bsoncxx::v1::types::value{bsoncxx::v_noabi::to_v1(v)}) {
        return bsoncxx::v1::types::value{bsoncxx::v_noabi::to_v1(v)};
    }
};

} // namespace detail
} // namespace mongocxx
