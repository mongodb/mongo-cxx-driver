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

#include <utility>

#include <bsoncxx/types/bson_value/view.hpp>

#include <bsoncxx/private/convert.hh>

#include <mongocxx/private/bson.hh>

namespace mongocxx {
namespace detail {

struct scoped_bson_value {
    bson_value_t value = {};

    // Allow obtaining a pointer to this->value even in rvalue expressions.
    bson_value_t* get() noexcept {
        return &value;
    }

    // Communicate this->value is to be initialized via the resulting pointer.
    bson_value_t* value_for_init() noexcept {
        return &this->value;
    }

    template <typename T>
    auto convert(T const& value)
        // Use trailing return type syntax to SFINAE without triggering GCC -Wignored-attributes
        // warnings due to using decltype within template parameters.
        -> decltype(bsoncxx::v_noabi::types::convert_to_libbson(
            std::declval<T const&>(),
            std::declval<bson_value_t*>())) {
        bsoncxx::v_noabi::types::convert_to_libbson(value, &this->value);
    }

    template <typename T>
    explicit scoped_bson_value(T const& value) {
        convert(value);
    }

    explicit scoped_bson_value(bsoncxx::v_noabi::types::bson_value::view const& view) {
        // Argument order is reversed for bsoncxx::v_noabi::types::bson_value::view.
        bsoncxx::v_noabi::types::convert_to_libbson(&this->value, view);
    }

    ~scoped_bson_value() {
        bson_value_destroy(&value);
    }

    // Expectation is that value_for_init() will be used to initialize this->value.
    scoped_bson_value() = default;

    scoped_bson_value(scoped_bson_value const&) = delete;
    scoped_bson_value(scoped_bson_value&&) = delete;
    scoped_bson_value& operator=(scoped_bson_value const&) = delete;
    scoped_bson_value& operator=(scoped_bson_value&&) = delete;
};

} // namespace detail
} // namespace mongocxx
