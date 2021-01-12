// Copyright 2020 MongoDB Inc.
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

#include <bsoncxx/config/prelude.hpp>

#include <bsoncxx/builder/core.hpp>
#include <bsoncxx/types/bson_value/value.hpp>

namespace bsoncxx {
BSONCXX_INLINE_NAMESPACE_BEGIN
namespace builder {
namespace list {
using namespace bsoncxx::types;

///
/// A JSON-like builder for creating BSON arrays.
///
class array {
    using value_type = array;
    using initializer_list_t = std::initializer_list<value_type>;

   public:
    ///
    /// Creates an empty array.
    ///
    array() : array({}){};

    ///
    /// Creates an array from a single value of type T.
    ///
    /// @warning T must be a BSON type, i.e., implicitly convertible to a
    /// bsoncxx::types::bson_value::value.
    ///
    /// @see bsoncxx::types::bson_value::value.
    ///
    template <typename T>
    array(T value) : _value{value} {
        _core.append(_value);
    }

    ///
    /// Creates a BSON array.
    ///
    /// @param init
    ///     the initializer list used to construct the BSON array
    ///
    /// @see bsoncxx::builder::list
    /// @see bsoncxx::builder::document
    ///
    array(initializer_list_t init) : _is_array{true} {
        for (auto&& ele : init)
            append_array_or_value(_core, ele);
    }

    operator bsoncxx::array::value() {
        return this->extract();
    }

    bsoncxx::array::value extract() {
        return _core.extract_array();
    }

    array& operator+=(value_type o) {
        append_array_or_value(_core, o);
        return *this;
    }

   private:
    void append_array_or_value(core& core, const array& ele) {
        ele._is_array ? core.append(ele._core.view_array()) : core.append(ele._value);
    }

    bool _is_array{false};
    bson_value::value _value{nullptr};
    core _core{true};
};

}  // namespace list
}  // namespace builder
BSONCXX_INLINE_NAMESPACE_END
}  // namespace bsoncxx
