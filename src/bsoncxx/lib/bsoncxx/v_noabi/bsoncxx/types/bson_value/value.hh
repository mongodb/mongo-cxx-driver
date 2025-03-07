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

#include <bsoncxx/types/bson_value/value.hpp>

//

#include <bsoncxx/types/bson_value/view.hpp>

#include <bsoncxx/private/bson.hh>

namespace bsoncxx {
namespace v_noabi {
namespace types {
namespace bson_value {

class value::impl {
   public:
    impl(bson_value_t const* value) {
        bson_value_copy(value, &_value);
    }

    impl() {
        // Initialize the value to null for safe destruction.
        _value.value_type = BSON_TYPE_NULL;
        _value.padding = 0;
    }

    ~impl() {
        bson_value_destroy(&_value);
    }

    impl(impl&&) = delete;
    impl operator=(impl&&) = delete;
    impl(impl const&) = delete;
    impl operator=(impl const&) = delete;

    bson_value::view view() const noexcept {
        // ABI backward compatibility. Const is restored in `view::_init`.
        return bson_value::view{const_cast<void*>(static_cast<void const*>(&_value))};
    }

    bson_value_t _value;
};

// Helper to create a value from an existing bson_value_t
// (for mongocxx callers who cannot be added as friends)
inline bson_value::value make_owning_bson(void* internal_value) {
    return bson_value::value{internal_value};
}

} // namespace bson_value
} // namespace types
} // namespace v_noabi
} // namespace bsoncxx
